/**
 * @file   img-pcx.cpp
 * @brief  Image specialisation for PCX format images.
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/bind.hpp>
#include <camoto/bitstream.hpp>
#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp>
#include <camoto/stream_filtered.hpp>
#include <camoto/stream_sub.hpp>
#include "img-pcx.hpp"

/// Pad out to a multiple of two bytes
/// @todo Does this work for files where it was four?
#define PLANE_PAD 2

namespace camoto {
namespace gamegraphics {

/// Resize a stream when the end part of it has been turned into a substream
void truncateParent(stream::output_sptr parent, stream::output_sub_sptr sub,
	stream::pos off, stream::len len)
	throw (stream::write_error)
{
	parent->truncate(off + len);
	sub->resize(len);
	return;
}

class filter_pcx_unrle: public filter {

	protected:
		uint8_t val;  ///< Previous byte read
		int count;    ///< How many times to repeat prev

	public:

		filter_pcx_unrle() :
			count(0)
		{
		};

		void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn)
			throw (filter_error)
		{
			stream::len r = 0, w = 0;
			// While there's more space to write, and either more data to read or
			// more data to write
			while (              // while there is...
				(w < *lenOut)      // more space to write into, and
				&& (
					(r < *lenIn)     // more data to read, or
					|| (this->count) // more data to write
				)
			) {

				// Loop while there's no bytes to write but more to read
				while ((this->count == 0) && (r < *lenIn)) {
					if ((*in & 0xC0) == 0xC0) { // RLE trigger
						if (*lenIn - r < 2) {
							// No value byte following the count we just peeked at
							*lenOut = w;
							*lenIn = r;
							if (r == 0) {
								// We haven't read any bytes yet, so this is the start of the
								// buffer and we've only got this one byte!
								std::cerr << "[img-pcx] PCX data ended in the middle of an RLE "
									"code!  Returning partial image." << std::endl;
							} else {
								// We've already read some bytes, so the buffer probably just
								// ends in the middle of the code.  We'll return what we've got
								// and pick up the next time we're called.
							}
							return;
						}
						this->count = *in & 0x3F;
						in++; // count the byte above
						this->val = *in;
						in++;
						r += 2;
					} else {
						this->val = *in;
						in++; // count the byte read above
						r++;
						this->count = 1;
					}
				}

				// Write out any repeats we've got stored up
				while ((w < *lenOut) && (this->count)) {
					*out++ = this->val;
					this->count--;
					w++;
				}

				// Now we've either written out all the repeats (count == 0) or we've
				// filled the output buffer (w == *lenOut), or both
			}
			*lenOut = w;
			*lenIn = r;
			return;
		}

};

class filter_pcx_rle: public filter {

	protected:
		uint8_t val;  ///< Previous byte read
		int count;    ///< How many times to repeat prev

	public:

		filter_pcx_rle() :
			count(0),
			val(0)
		{
		};

		void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn)
			throw (filter_error)
		{
			stream::len r = 0, w = 0;
			// +2 == make sure there's always enough room to write one RLE pair
			bool eof = false;
			while ((w + 2 < *lenOut) && ((r < *lenIn) || (this->count))) {
				eof = (r >= *lenIn);
				if ((!eof) && (this->count < 63) && (*in == this->val)) {
					in++;
					r++;
					this->count++;
				} else {
					// Byte changed or max count reached, write out old byte
					if ((this->count > 2) || (this->val >= 0xC0)) {
						assert(this->count < 64);
						*out++ = 0xC0 | this->count;
						*out++ = this->val;
						w += 2;
					} else if (this->count == 2) {
						assert((this->val & 0xC0) != 0xC0);
						*out++ = this->val;
						*out++ = this->val;
						w += 2;
					} else if (this->count == 1) {
						assert((this->val & 0xC0) != 0xC0);
						*out++ = this->val;
						w++;
					} // else count == 0, it's the first byte, ignore the change
					if (!eof) {
						this->val = *in++;
						r++;
						this->count = 1;
					} else this->count = 0;
				}
			}
			*lenOut = w;
			*lenIn = r;
			return;
		}

};

stream::len putNextChar(stream::output_sptr src, uint8_t *lastChar, uint8_t out)
{
	*lastChar = out;
	return src->try_write(&out, 1);
}


PCXBaseImageType::PCXBaseImageType(int bitsPerPlane, int numPlanes)
	throw () :
		bitsPerPlane(bitsPerPlane),
		numPlanes(numPlanes)
{
}

PCXBaseImageType::~PCXBaseImageType()
	throw ()
{
}

std::string PCXBaseImageType::getCode() const
	throw ()
{
	std::stringstream code;
	code << "img-pcx-" << (int)this->bitsPerPlane << "b"
		<< (int)this->numPlanes << "p";
	return code.str();
}

std::vector<std::string> PCXBaseImageType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pcx");
	return vcExtensions;
}

ImageType::Certainty PCXBaseImageType::isInstance(stream::input_sptr psImage) const
	throw (stream::error)
{
	psImage->seekg(0, stream::start);

	uint8_t sig, ver, enc, bpp;
	psImage
		>> u8(sig)
		>> u8(ver)
		>> u8(enc)
		>> u8(bpp)
	;

	// TESTED BY: img_pcx_*_isinstance_c01
	if (sig != 0x0A) return DefinitelyNo;

	// TESTED BY: img_pcx_*_isinstance_c02
	if (
		(ver != 0x00) &&
		(ver != 0x02) &&
		(ver != 0x03) &&
		(ver != 0x05)
	) return DefinitelyNo; // unsupported version

	uint8_t pln;
	psImage->seekg(65, stream::start);
	psImage >> u8(pln);

	// TESTED BY: img_pcx_*_isinstance_c03
	if (bpp != this->bitsPerPlane) return DefinitelyNo;

	// TESTED BY: img_pcx_*_isinstance_c04
	if (pln != this->numPlanes) return DefinitelyNo;

	// TESTED BY: img_pcx_*_isinstance_c00
	return DefinitelyYes;
}

ImagePtr PCXBaseImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
	throw (stream::error)
{
	// Create a 16-colour 1-bit-per-plane image
	psImage->write(
		"\x0A" // sig
		"\x05" // ver
		"\x01" // encoding
		, 3);
	psImage << u8(this->bitsPerPlane);
	psImage->write(
		"\x00\x00" // xmin
		"\x00\x00" // ymin
		"\x00\x00" // xmax
		"\x00\x00" // ymax
		"\x00\x00" // xdpi
		"\x00\x00" // ydpi
		, 12
	);

	PaletteTablePtr pal = createDefaultCGAPalette();
	assert(pal->size() == 16);
	for (int i = 0; i < 16; i++) {
		psImage
			<< u8(pal->at(i).red)
			<< u8(pal->at(i).green)
			<< u8(pal->at(i).blue)
		;
	}

	psImage
		<< u8(0) // reserved
		<< u8(this->numPlanes)
	;
	psImage->write(
		"\x00\x00" // bytes per scanline
		"\x01\x00" // palette flag
		"\x00\x00" // x scroll
		"\x00\x00" // y scroll
		, 8
	);

	// Padding
	for (int i = 0; i < 54; i++) {
		psImage->write("\0", 1);
	}

	return ImagePtr(new PCXImage(psImage, this->bitsPerPlane, this->numPlanes));
}

ImagePtr PCXBaseImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
	throw (stream::error)
{
	return ImagePtr(new PCXImage(psImage, this->bitsPerPlane, this->numPlanes));
}

SuppFilenames PCXBaseImageType::getRequiredSupps(const std::string& filenameImage) const
	throw ()
{
	return SuppFilenames();
}


PCX_PlanarEGA_ImageType::PCX_PlanarEGA_ImageType()
	throw () :
		PCXBaseImageType(1, 4)
{
}

PCX_PlanarEGA_ImageType::~PCX_PlanarEGA_ImageType()
	throw ()
{
}

std::string PCX_PlanarEGA_ImageType::getFriendlyName() const
	throw ()
{
	return "PCX image (16-colour planar EGA)";
}

std::vector<std::string> PCX_PlanarEGA_ImageType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Word Rescue");
	return vcGames;
}


PCX_LinearVGA_ImageType::PCX_LinearVGA_ImageType()
	throw () :
		PCXBaseImageType(8, 1)
{
}

PCX_LinearVGA_ImageType::~PCX_LinearVGA_ImageType()
	throw ()
{
}

std::string PCX_LinearVGA_ImageType::getFriendlyName() const
	throw ()
{
	return "PCX image (256-colour linear VGA)";
}

std::vector<std::string> PCX_LinearVGA_ImageType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Halloween Harry");
	return vcGames;
}


PCXImage::PCXImage(stream::inout_sptr data, uint8_t bitsPerPlane, uint8_t numPlanes)
	throw (stream::error) :
		data(data),
		bitsPerPlane(bitsPerPlane),
		numPlanes(numPlanes)
{
	stream::len lenData = this->data->size();

	this->data->seekg(1, stream::start);
	uint8_t bpp, pln;
	uint16_t xmin, ymin, xmax, ymax;
	this->data
		>> u8(this->ver)
		>> u8(this->encoding)
		>> u8(bpp)
		>> u16le(xmin)
		>> u16le(ymin)
		>> u16le(xmax)
		>> u16le(ymax)
	;
	this->width = xmax - xmin + 1;
	this->height = ymax - ymin + 1;

	this->data->seekg(65, stream::start);
	this->data
		>> u8(pln)
	;

	if ((bpp != this->bitsPerPlane) || (pln != this->numPlanes)) {
		throw stream::error(createString("This file is in PCX " << (int)bpp << "b"
			<< (int)pln << "p format, cannot open as PCX " << (int)this->bitsPerPlane
			<< "b" << (int)this->numPlanes << "p."));
	}
}

PCXImage::~PCXImage()
	throw ()
{
}

int PCXImage::getCaps()
	throw ()
{
	int depth;
	int numColours = 1 << (this->numPlanes * this->bitsPerPlane);
	if (numColours > 16) depth = Image::ColourDepthVGA;
	else if (numColours > 4) depth = Image::ColourDepthEGA;
	else if (numColours > 1) depth = Image::ColourDepthCGA;
	else depth = Image::ColourDepthMono;

	return Image::CanSetDimensions | Image::HasPalette | Image::CanSetPalette
		| depth;
}

void PCXImage::getDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->width;
	*height = this->height;
	return;
}

void PCXImage::setDimensions(unsigned int width, unsigned int height)
	throw (stream::error)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	this->width = width;
	this->height = height;
	return;
}

StdImageDataPtr PCXImage::toStandard()
	throw (stream::error)
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));

	unsigned long dataSize = width * height;
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);

	this->data->seekg(66, stream::start);
	int16_t bytesPerPlaneScanline;
	this->data
		>> u16le(bytesPerPlaneScanline)
	;
	if (bytesPerPlaneScanline % 2) throw stream::error("Invalid PCX file (bytes "
		"per scanline is not an even number)");

	// Decode the RLE image data if necessary
	stream::input_sptr filtered;
	if (this->encoding == 1) {
		// Find the end of the RLE data
		stream::len lenRLE = this->data->size() - 128; // 128 == header
		if (this->ver >= 5) { // 3.0 or better, look for VGA pal
			try {
				uint8_t palSig = 0;
				this->data->seekg(-769, stream::end);
				this->data >> u8(palSig);
				if (palSig == 0x0C) {
					// There is a VGA palette
					lenRLE -= 769;
				}
			} catch (const stream::error&) {
				// no palette
			}
		}

		stream::input_sub_sptr sub(new stream::input_sub());
		sub->open(this->data, 128, lenRLE);
		filter_sptr filt(new filter_pcx_unrle());
		stream::input_filtered_sptr fs(new stream::input_filtered());
		fs->open(sub, filt);
		filtered = fs;
	} else {
		filtered = this->data;
	}

	uint8_t *line = imgData;
	int repeat = 0;
	uint8_t byte;
	bitstream_sptr bits(new bitstream(bitstream::bigEndian));
	/// @todo write bitstream version that takes input- and output-only streams (rather than r/w ones only)
	//bitstream_sptr bits(new bitstream(filtered, bitstream::bigEndian));

	fn_getnextchar cbNext = boost::bind(&stream::input::try_read, filtered, _1, 1);
	int val;
	bool eof = false;
	for (int y = 0; y < height; y++) {
		memset(line, 0, width); // blank out line
		for (int p = 0; p < this->numPlanes; p++) {
			for (int x = 0; x < width; x++) {
				if (!eof) {
					if (bits->read(cbNext, this->bitsPerPlane, &val) != this->bitsPerPlane) {
						std::cerr << "[img-pcx] PCX data ended early!  Returning "
							"partial image." << std::endl;
						val = 0;
						eof = true;
						// Just read zero for the rest of the missing data
					}
				}
				line[x] |= val << (p * this->bitsPerPlane);
			}
			// Skip over any EOL padding.
			bits->flushByte();
			int pad = bytesPerPlaneScanline - ((this->bitsPerPlane * width + 7) / 8);
			if (pad < 0) throw stream::error("Corrupted PCX file - bad value for "
				"'bytes per scanline'");
			uint8_t dummy;
			while (pad--) cbNext(&dummy);
		}
		line += width;
	}
	return ret;
}

StdImageDataPtr PCXImage::toStandardMask()
	throw ()
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	// Return an entirely opaque mask
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	memset(imgData, 0, dataSize);

	return ret;
}

void PCXImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw (stream::error)
{
	// Remember the current palette before we start overwriting things, in case
	// it hasn't been set
	if (!this->pal) this->getPalette();

	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));

	int16_t bytesPerPlaneScanline = width * this->bitsPerPlane / 8;
	// Pad out to a multiple of PLANE_PAD bytes
	bytesPerPlaneScanline += bytesPerPlaneScanline % PLANE_PAD;

	// Assume worst case and enlarge file enough to fit complete data
	stream::len maxSize = 128+bytesPerPlaneScanline*2 * this->numPlanes * height + 768+1;
	this->data->truncate(maxSize);

	this->data->seekg(0, stream::start);
	this->data
		<< u8(0x0A)
		<< u8(this->ver)
		<< u8(this->encoding)
		<< u8(this->bitsPerPlane)
		<< u16le(0) // xmin
		<< u16le(0) // ymin
		<< u16le(width - 1)
		<< u16le(height - 1)
		<< u16le(75) // dpi
		<< u16le(75)
	;

	/// @todo Handle CGA graphics-mode palette

	int palSize = this->pal->size();
	for (int i = 0; i < std::min(palSize, 16); i++) {
		this->data
			<< u8(this->pal->at(i).red)
			<< u8(this->pal->at(i).green)
			<< u8(this->pal->at(i).blue)
		;
	}
	// Pad out to 16 colours if needed
	for (int i = palSize; i < 16; i++) {
		this->data->write("\0\0\0", 3);
	}

	this->data
		<< u8(0) // reserved
		<< u8(this->numPlanes)
		<< u16le(bytesPerPlaneScanline)
		<< u16le(1) // colour palette
		<< u16le(0)
		<< u16le(0)
	;
	// Padding
	for (int i = 0; i < 54; i++) {
		this->data->write("\0", 1);
	}

	assert(this->data->tellp() == 128);
	// Encode the RLE image data if necessary
	stream::output_sptr filtered;
	if (this->encoding == 1) {
		stream::output_sub_sptr sub(new stream::output_sub());
		sub->open(this->data, 128, maxSize - 128, boost::bind(&truncateParent, this->data, sub, 128, _1));
		filter_sptr filt(new filter_pcx_rle());
		stream::output_filtered_sptr fs(new stream::output_filtered());
		fs->open(sub, filt, NULL);
		filtered = fs;
	} else {
		filtered = this->data;
	}

	uint8_t *line = newContent.get();
	bitstream_sptr bits(new bitstream(bitstream::bigEndian));
	uint8_t lastChar;
	fn_putnextchar cbNext = boost::bind(putNextChar, filtered, &lastChar, _1);
	int planeMask = (1 << this->bitsPerPlane) - 1;
	int pad = bytesPerPlaneScanline - ((this->bitsPerPlane * width + 7) / 8);
	int val;

	for (int y = 0; y < height; y++) {
		for (int p = 0; p < this->numPlanes; p++) {
			int bitsInPlane = (p * this->bitsPerPlane);
			for (int x = 0; x < width; x++) {
				val = (line[x] >> bitsInPlane) & planeMask;
				bits->write(cbNext, this->bitsPerPlane, val);
			}
			uint8_t next, mask;
			bits->peekByte(&next, &mask);
			if (mask) {
				// Have to pad up to the next byte boundary, so try to find the most
				// efficient way to do so.
				int prevBits = lastChar & ~mask;
				if ((next | prevBits) == lastChar) {
					int bitpad = 8 - ((this->bitsPerPlane * width) % 8);
					assert(bitpad != 0);
					bits->write(cbNext, bitpad, prevBits);
				}
			}
			// Pad up to next byte boundary
			bits->flushByte(cbNext);

			// Pad scanline to multiple of PLANE_PAD bytes
			for (int z = 0; z < pad; z++) cbNext(lastChar);
		}
		line += width;
	}
	filtered->flush();
	filtered.reset();

	// Write the VGA palette if ver 5 and 256 colour pal
	if ((this->ver >= 5) && (palSize > 16)) {
		this->data << u8(0x0C); // palette presence flag
		for (int i = 0; i < std::min(palSize, 256); i++) {
			this->data
				<< u8(this->pal->at(i).red)
				<< u8(this->pal->at(i).green)
				<< u8(this->pal->at(i).blue)
			;
		}
		// Pad out to 256 colours if needed
		for (int i = palSize; i < 256; i++) {
			this->data->write("\0\0\0", 3);
		}
	}

	this->data->truncate_here();
	return;
}

PaletteTablePtr PCXImage::getPalette()
	throw (stream::error)
{
	if (!this->pal) {

		if (this->ver == 3) { // 2.8 w/out palette
			this->pal = createDefaultCGAPalette();
			return this->pal;
		}

		/// @todo Handle CGA graphics-mode palette

		uint8_t palSig = 0;
		if (this->ver >= 5) { // 3.0 or better, look for VGA pal
			try {
				this->data->seekg(-769, stream::end);
				this->data >> u8(palSig);
			} catch (const stream::error&) {
				palSig = 0;
			}
		}

		int palSize;
		if (palSig == 0x0C) {
			// 256-colour palette
			palSize = 256;
		} else {
			// Default to EGA palette in file header
			palSize = 16;
			this->data->seekg(16, stream::start);
		}

		uint8_t *rawPal = new uint8_t[palSize * 3];
		this->data->read((char *)rawPal, palSize*3);
		this->pal.reset(new PaletteTable());
		this->pal->reserve(palSize);
		uint8_t *r = rawPal;
		for (int c = 0; c < palSize; c++) {
			PaletteEntry p;
			p.red = *r++;
			p.green = *r++;
			p.blue = *r++;
			p.alpha = 255;
			this->pal->push_back(p);
		}
		delete[] rawPal;
	}
	return this->pal;
}

void PCXImage::setPalette(PaletteTablePtr newPalette)
	throw (stream::error)
{
	this->pal = newPalette;
	return;
}

} // namespace gamegraphics
} // namespace camoto
