/**
 * @file  img-pcx.cpp
 * @brief Image specialisation for PCX format images.
 *
 * Copyright (C) 2010-2015 Adam Nielsen <malvineous@shikadi.net>
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

#include <cassert>
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
void truncateParent(std::shared_ptr<stream::output> parent, stream::pos off,
	stream::output_sub* sub, stream::len len)
{
	parent->truncate(off + len);
	sub->resize(len);
	return;
}

class filter_pcx_unrle: virtual public filter
{
	protected:
		uint8_t val;        ///< Previous byte read
		unsigned int count; ///< How many times to repeat prev

	public:
		virtual void reset(stream::len lenInput)
		{
			this->count = 0;
			return;
		}

		virtual void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn)
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

class filter_pcx_rle: virtual public filter
{
	protected:
		uint8_t val;        ///< Previous byte read
		unsigned int count; ///< How many times to repeat prev
		stream::len lenScanline; ///< Bytes per scanline (RLE boundary)
		stream::pos posScanline; ///< How far into current scanline

	public:
		filter_pcx_rle(stream::len lenScanline)
			:	lenScanline(lenScanline),
				posScanline(0)
		{
		}

		virtual void reset(stream::len lenInput)
		{
			this->val = 0;
			this->count = 0;
			this->posScanline = 0;
			return;
		}

		virtual void transform(uint8_t *out, stream::len *lenOut,
			const uint8_t *in, stream::len *lenIn)
		{
			stream::len r = 0, w = 0;
			// +2 == make sure there's always enough room to write one RLE pair
			bool eof = false;
			while ((w + 2 < *lenOut) && ((r < *lenIn) || (this->count))) {
				eof = (r >= *lenIn);
				if (
					(!eof)
					&& (this->count < 63)
					&& (*in == this->val)
					&& (
						(this->posScanline == 0)
						|| ((this->posScanline % this->lenScanline) != 0)
					)
				) {
					in++;
					r++;
					this->count++;
					this->posScanline++;
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
						this->posScanline++;
						this->count = 1;
					} else this->count = 0;
				}
			}
			*lenOut = w;
			*lenIn = r;
			return;
		}
};

stream::len putNextChar(std::shared_ptr<stream::output> src, uint8_t *lastChar, uint8_t out)
{
	*lastChar = out;
	return src->try_write(&out, 1);
}


ImageType_PCXBase::ImageType_PCXBase(int bitsPerPlane, int numPlanes,
	bool useRLE)
	:	bitsPerPlane(bitsPerPlane),
		numPlanes(numPlanes),
		useRLE(useRLE)
{
}

ImageType_PCXBase::~ImageType_PCXBase()
{
}

std::string ImageType_PCXBase::code() const
{
	auto code = createString("img-pcx-" << (int)this->bitsPerPlane << "b"
		<< (int)this->numPlanes << "p");
	if (!this->useRLE) code.append("-norle");
	return code;
}

std::vector<std::string> ImageType_PCXBase::fileExtensions() const
{
	return {"pcx"};
}

ImageType::Certainty ImageType_PCXBase::isInstance(
	stream::input& content) const
{
	// Header too short
	// TESTED BY: img_pcx_*_isinstance_c05
	if (content.size() < 128) return DefinitelyNo;

	content.seekg(0, stream::start);

	uint8_t sig, ver, enc, bpp;
	content
		>> u8(sig)
		>> u8(ver)
		>> u8(enc)
		>> u8(bpp)
	;

	// Bad signature
	// TESTED BY: img_pcx_*_isinstance_c01
	if (sig != 0x0A) return DefinitelyNo;

	// Unsupported version
	// TESTED BY: img_pcx_*_isinstance_c02
	if (
		(ver != 0x00) &&
		(ver != 0x02) &&
		(ver != 0x03) &&
		(ver != 0x05)
	) return DefinitelyNo;

	// Wrong number of bits per plane
	// TESTED BY: img_pcx_*_isinstance_c03
	if (bpp != this->bitsPerPlane) return DefinitelyNo;

	// The RLE flag is for writing only, we will accept files either way.

	uint8_t pln;
	content.seekg(65, stream::start);
	content >> u8(pln);

	// Wrong number of planes
	// TESTED BY: img_pcx_*_isinstance_c04
	if (pln != this->numPlanes) return DefinitelyNo;

	// TESTED BY: img_pcx_*_isinstance_c00
	return DefinitelyYes;
}

std::unique_ptr<Image> ImageType_PCXBase::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->seekp(0, stream::start);

	*content
		<< u8(0x0A) // sig
		<< u8(0x05) // ver
		<< u8(this->useRLE ? 0x01 : 0x00) // encoding
		<< u8(this->bitsPerPlane);

	content->write(
		"\x00\x00" // xmin
		"\x00\x00" // ymin
		"\x00\x00" // xmax
		"\x00\x00" // ymax
		"\x00\x00" // xdpi
		"\x00\x00" // ydpi
		, 12
	);

	std::shared_ptr<Palette> pal = createPalette_DefaultEGA();
	assert(pal->size() == 16);
	for (int i = 0; i < 16; i++) {
		*content
			<< u8(pal->at(i).red)
			<< u8(pal->at(i).green)
			<< u8(pal->at(i).blue)
		;
	}

	*content
		<< u8(0) // reserved
		<< u8(this->numPlanes)
	;
	content->write(
		"\x00\x00" // bytes per scanline
		"\x01\x00" // palette flag
		"\x00\x00" // x scroll
		"\x00\x00" // y scroll
		, 8
	);

	// Padding
	for (int i = 0; i < 54; i++) {
		content->write("\0", 1);
	}

	return this->open(std::move(content), suppData);
}

std::unique_ptr<Image> ImageType_PCXBase::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Image_PCX>(
		std::move(content), this->bitsPerPlane, this->numPlanes, this->useRLE
	);
}

SuppFilenames ImageType_PCXBase::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	return {};
}


ImageType_PCX_PlanarEGA::ImageType_PCX_PlanarEGA()
	:	ImageType_PCXBase(1, 4, true)
{
}

ImageType_PCX_PlanarEGA::~ImageType_PCX_PlanarEGA()
{
}

std::string ImageType_PCX_PlanarEGA::friendlyName() const
{
	return "PCX image (16-colour planar EGA)";
}

std::vector<std::string> ImageType_PCX_PlanarEGA::games() const
{
	return {"Word Rescue"};
}


ImageType_PCX_LinearVGA::ImageType_PCX_LinearVGA()
	:	ImageType_PCXBase(8, 1, true)
{
}

ImageType_PCX_LinearVGA::~ImageType_PCX_LinearVGA()
{
}

std::string ImageType_PCX_LinearVGA::friendlyName() const
{
	return "PCX image (256-colour linear VGA)";
}

std::vector<std::string> ImageType_PCX_LinearVGA::games() const
{
	return {"Halloween Harry"};
}


ImageType_PCX_LinearVGA_NoRLE::ImageType_PCX_LinearVGA_NoRLE()
	:	ImageType_PCXBase(8, 1, false)
{
}

ImageType_PCX_LinearVGA_NoRLE::~ImageType_PCX_LinearVGA_NoRLE()
{
}

std::string ImageType_PCX_LinearVGA_NoRLE::friendlyName() const
{
	return "PCX image (256-colour linear VGA; no RLE)";
}

std::vector<std::string> ImageType_PCX_LinearVGA_NoRLE::games() const
{
	return {};
}


Image_PCX::Image_PCX(std::unique_ptr<stream::inout> content,
	uint8_t bitsPerPlane, uint8_t numPlanes, bool useRLE)
	:	content(std::move(content)),
		bitsPerPlane(bitsPerPlane),
		numPlanes(numPlanes),
		useRLE(useRLE)
{
	this->content->seekg(1, stream::start);
	uint8_t bpp, pln;
	uint16_t xmin, ymin, xmax, ymax;
	*this->content
		>> u8(this->ver)
		>> u8(this->encoding)
		>> u8(bpp)
		>> u16le(xmin)
		>> u16le(ymin)
		>> u16le(xmax)
		>> u16le(ymax)
	;
	this->dims.x = xmax - xmin + 1;
	this->dims.y = ymax - ymin + 1;

	this->content->seekg(65, stream::start);
	*this->content
		>> u8(pln)
	;

	if ((bpp != this->bitsPerPlane) || (pln != this->numPlanes)) {
		throw stream::error(createString("This file is in PCX " << (int)bpp << "b"
			<< (int)pln << "p format, cannot open as PCX " << (int)this->bitsPerPlane
			<< "b" << (int)this->numPlanes << "p."));
	}

	// Load the palette
	if (this->ver == 3) { // 2.8 w/out palette
		this->pal = createPalette_DefaultEGA();
	} else {

		/// @todo Handle CGA graphics-mode palette

		uint8_t palSig = 0;
		if (this->ver >= 5) { // 3.0 or better, look for VGA pal
			try {
				this->content->seekg(-769, stream::end);
				*this->content >> u8(palSig);
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
			this->content->seekg(16, stream::start);
		}

		auto newPal = std::make_shared<Palette>();
		newPal->reserve(palSize);
		for (int c = 0; c < palSize; c++) {
			PaletteEntry p;
			*this->content
				>> u8(p.red)
				>> u8(p.green)
				>> u8(p.blue)
			;
			p.alpha = 255;
			newPal->push_back(p);
		}
		this->pal = newPal;
	}
	assert(this->pal);
}

Image_PCX::~Image_PCX()
{
}

Image::Caps Image_PCX::caps() const
{
	return
		  Caps::SetDimensions
		| Caps::HasPalette
		| Caps::SetPalette
	;
}

ColourDepth Image_PCX::colourDepth() const
{
	int numColours = 1 << (this->numPlanes * this->bitsPerPlane);
	if (numColours > 16) return ColourDepth::VGA;
	else if (numColours > 4) return ColourDepth::EGA;
	else if (numColours > 1) return ColourDepth::CGA;
	return ColourDepth::Mono;
}

Point Image_PCX::dimensions() const
{
	return this->dims;
}

void Image_PCX::dimensions(const Point& newDimensions)
{
	assert(this->caps() & Caps::SetDimensions);
	this->dims = newDimensions;
	return;
}

Pixels Image_PCX::convert() const
{
	auto dims = this->dimensions();
	assert((dims.x != 0) && (dims.y != 0));

	Pixels imgData(dims.x * dims.y, '\x00');

	this->content->seekg(66, stream::start);
	int16_t bytesPerScanline;
	*this->content
		>> u16le(bytesPerScanline)
	;
//	if (bytesPerScanline % 2) throw stream::error("Invalid PCX file (bytes "
//		"per scanline is not an even number)");

	// Find the end of the pixel data
	stream::len lenRLE = this->content->size() - 128; // 128 == header
	if (this->ver >= 5) { // 3.0 or better, look for VGA pal
		try {
			uint8_t palSig = 0;
			this->content->seekg(-769, stream::end);
			*this->content >> u8(palSig);
			if (palSig == 0x0C) {
				// There is a VGA palette
				lenRLE -= 769;
			}
		} catch (const stream::error&) {
			// no palette
		}
	}
	std::shared_ptr<stream::input> content_pixels =
		std::make_shared<stream::input_sub>(this->content, 128, lenRLE);

	// Decode the RLE pixel data if necessary
	if (this->encoding == 1) {
		content_pixels = std::make_shared<stream::input_filtered>(
			content_pixels,
			std::make_shared<filter_pcx_unrle>()
		);
	}

	auto line = &imgData[0];
	/// @todo write bitstream version that takes input- and output-only streams (rather than r/w ones only)
	auto bits = std::make_unique<bitstream>(bitstream::bigEndian);

	fn_getnextchar cbNext = std::bind(&stream::input::try_read, content_pixels, std::placeholders::_1, 1);
	unsigned int val;
	bool eof = false;
	for (unsigned int y = 0; y < dims.y; y++) {
		memset(line, 0, dims.x); // blank out line
		auto posScanlineStart = content_pixels->tellg();
		for (unsigned int p = 0; p < this->numPlanes; p++) {
			for (unsigned int x = 0; x < dims.x; x++) {
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
		}

		// If we didn't read bytesPerScanline bytes, now we have to skip data until
		// we reach that point.
		auto lenScanlineRead = content_pixels->tellg() - posScanlineStart;
		uint8_t dummy;
		auto pad = bytesPerScanline - std::min<stream::pos>(bytesPerScanline, lenScanlineRead);
		while (pad--) cbNext(&dummy);

		line += dims.x;
	}
	return imgData;
}

Pixels Image_PCX::convert_mask() const
{
	// Return an entirely opaque mask
	auto dims = this->dimensions();
	return Pixels(dims.x * dims.y, '\x00');
}

void Image_PCX::convert(const Pixels& newContent, const Pixels& newMask)
{
	// Remember the current palette before we start overwriting things, in case
	// it hasn't been set
	auto pal = this->palette();
	assert(pal);

	auto dims = this->dimensions();
	assert((dims.x != 0) && (dims.y != 0));

	const unsigned int bytesPerPlaneScanline = toNearestMultiple(dims.x * this->bitsPerPlane, 8) / 8;
	unsigned int bytesPerScanline = bytesPerPlaneScanline * this->numPlanes;
	// Pad out to a multiple of PLANE_PAD bytes
	bytesPerScanline = toNearestMultiple(bytesPerScanline, PLANE_PAD);

	// Assume worst case and enlarge file enough to fit complete data
	stream::len maxSize = 128+bytesPerScanline * dims.y + 768+1;
	this->content->truncate(maxSize);

	this->content->seekp(0, stream::start);
	*this->content
		<< u8(0x0A)
		<< u8(this->ver)
		<< u8(this->useRLE ? this->encoding : 0x00)
		<< u8(this->bitsPerPlane)
		<< u16le(0) // xmin
		<< u16le(0) // ymin
		<< u16le(dims.x - 1)
		<< u16le(dims.y - 1)
		<< u16le(75) // dpi
		<< u16le(75)
	;

	/// @todo Handle CGA graphics-mode palette

	int palSize = pal->size();
	for (int i = 0; i < std::min(palSize, 16); i++) {
		*this->content
			<< u8(pal->at(i).red)
			<< u8(pal->at(i).green)
			<< u8(pal->at(i).blue)
		;
	}
	// Pad out to 16 colours if needed
	for (int i = palSize; i < 16; i++) {
		this->content->write("\0\0\0", 3);
	}

	*this->content
		<< u8(0) // reserved
		<< u8(this->numPlanes)
		<< u16le(bytesPerScanline)
		<< u16le(1) // colour palette
		<< u16le(0)
		<< u16le(0)
	;
	// Padding
	for (int i = 0; i < 54; i++) {
		this->content->write("\0", 1);
	}

	assert(this->content->tellp() == 128);
	std::shared_ptr<stream::output> content_pixels = std::make_shared<stream::output_sub>(
		this->content,
		128, maxSize - 128,
		std::bind(&truncateParent, this->content, 128, std::placeholders::_1, std::placeholders::_2)
	);

	// Encode the RLE image data if necessary
	if (this->useRLE && (this->encoding == 1)) {
		content_pixels = std::make_shared<stream::output_filtered>(
			content_pixels,
			std::make_shared<filter_pcx_rle>(bytesPerScanline),
			nullptr
		);
	}

	auto line = &newContent[0];
	auto bits = std::make_unique<bitstream>(bitstream::bigEndian);
	uint8_t lastChar;
	fn_putnextchar cbNext = std::bind(putNextChar, content_pixels, &lastChar, std::placeholders::_1);
	int planeMask = (1 << this->bitsPerPlane) - 1;
	int val;

	for (unsigned int y = 0; y < dims.y; y++) {
		auto posScanlineStart = content_pixels->tellp();
		for (unsigned int p = 0; p < this->numPlanes; p++) {
			int bitsInPlane = (p * this->bitsPerPlane);
			for (unsigned int x = 0; x < dims.x; x++) {
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
					int bitpad = 8 - ((this->bitsPerPlane * dims.x) % 8);
					assert(bitpad != 0);
					bits->write(cbNext, bitpad, prevBits);
				}
			}
			// Pad single-plane-scanline up to next byte boundary
			bits->flushByte(cbNext);
		}

		// Pad scanline to bytesPerScanline bytes
		auto lenScanlineWritten = content_pixels->tellp() - posScanlineStart;
		auto pad = bytesPerScanline - std::min<stream::pos>(bytesPerScanline, lenScanlineWritten);
		while (pad--) cbNext(lastChar);

		line += dims.x;
	}
	content_pixels->flush();
	content_pixels.reset();

	// Write the VGA palette if ver 5 and 256 colour pal
	if ((this->ver >= 5) && (palSize > 16)) {
		*this->content << u8(0x0C); // palette presence flag
		for (int i = 0; i < std::min(palSize, 256); i++) {
			*this->content
				<< u8(pal->at(i).red)
				<< u8(pal->at(i).green)
				<< u8(pal->at(i).blue)
			;
		}
		// Pad out to 256 colours if needed
		for (int i = palSize; i < 256; i++) {
			this->content->write("\0\0\0", 3);
		}
	}

	this->content->truncate_here();
	return;
}

} // namespace gamegraphics
} // namespace camoto
