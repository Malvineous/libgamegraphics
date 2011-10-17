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

#include <boost/pointer_cast.hpp>
#include <camoto/bitstream.hpp>
#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp>
#include <camoto/lzw.hpp> // for bitstream helper functions
#include "img-pcx.hpp"

namespace camoto {
namespace gamegraphics {

class pcx_unrle_filter: public io::multichar_input_filter {

	protected:
		int val;   ///< Previous byte read
		int count; ///< How many times to repeat prev

	public:

		pcx_unrle_filter() :
			count(0)
		{
		};

		template<typename Source>
		stream::len read(Source& src, char* s, stream::len n)
		{
			int r = 0;
			while (r < n) {
				while (this->count == 0) {
					this->val = boost::iostreams::get(src);
					if (this->val < 0) return (r == 0) ? this->val : r; // if no bytes read return error, otherwise # bytes read
					if ((this->val & 0xC0) == 0xC0) { // RLE trigger
						this->count = this->val & 0x3F;
						this->val = boost::iostreams::get(src);
						if (this->val < 0) return (r == 0) ? this->val : r; // if no bytes read return error, otherwise # bytes read
					} else {
						this->count = 1;
					}
				}
				assert(this->count > 0);

				*s++ = this->val;
				this->count--;
				r++;
			}
			return r;
		}

};

int nextChar(stream::input_sptr src, uint8_t *out)
{
	src->read((char *)out, 1);
	return 1;
}

class pcx_rle_filter: public io::multichar_input_filter {

	protected:
		int val;   ///< Previous byte read
		int count; ///< How many times to repeat prev
		int written; ///< Total bytes written

	public:

		struct category: io::multichar_input_filter_tag, io::flushable_tag { };

		pcx_rle_filter() :
			count(0),
			val(0),
			written(0)
		{
		};

		template<typename Source>
		stream::len read(Source& src, char* s, stream::len n)
		{
			int r = 0;
			n -= 2; // make sure there's always enough room to write one RLE pair
			while (r < n) {
				if (this->count > 62) {
					// Have to write now
					*s++ = 0xC0 | this->count;
					*s++ = this->val;
					r += 2;
					this->written += 2;
					this->count = 0;
					continue; // in case we've reached read limit
				}
				int c = boost::iostreams::get(src);
				if (c < 0) return (r == 0) ? c : r; // if no bytes read return error, otherwise # bytes read
				if (c == this->val) {
					this->count++;
				} else {
					// Byte changed, write out old byte
					if ((this->count > 2) || (this->val >= 0xC0)) {
						*s++ = 0xC0 | this->count;
						*s++ = this->val;
						r += 2;
						this->written += 2;
					} else if (this->count == 2) {
						*s++ = this->val;
						*s++ = this->val;
						r += 2;
						this->written += 2;
					} else { // this->count == 1
						*s++ = this->val;
						r++;
						this->written++;
					}
					this->val = c;
					this->count = 1;
				}
			}
			return r;
		}

		template<typename Sink>
		bool flush(Sink& dst)
		{
			if ((this->count > 2) || (this->val >= 0xC0)) {
				boost::iostreams::put(dst, 0xC0 | this->count);
				boost::iostreams::put(dst, this->val);
				this->written += 2;
			} else if (this->count == 2) {
				boost::iostreams::put(dst, this->val);
				boost::iostreams::put(dst, this->val);
				this->written += 2;
			} else if (this->count == 1) {
				boost::iostreams::put(dst, this->val);
				this->written++;
			}
			return true;
		}

		int getBytesWritten()
		{
			return this->written;
		}

};

int putNextChar(stream::output_sptr src, uint8_t out)
{
	src->write((char *)&out, 1);
	return 1;
}

PCXImageType::PCXImageType()
	throw ()
{
}

PCXImageType::~PCXImageType()
	throw ()
{
}

std::string PCXImageType::getCode() const
	throw ()
{
	return "img-pcx";
}

std::string PCXImageType::getFriendlyName() const
	throw ()
{
	return "PCX image";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> PCXImageType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pcx");
	return vcExtensions;
}

std::vector<std::string> PCXImageType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	// Too many to bother listing, and format used outside of games anyway
	return vcGames;
}

ImageType::Certainty PCXImageType::isInstance(stream::inout_sptr psImage) const
	throw (stream::error)
{
	psImage->seekg(0, stream::end);
	unsigned long lenData = psImage->tellg();
	psImage->seekg(0, stream::start);

	uint8_t sig, ver;
	psImage >> u8(sig) >> u8(ver);

	// TESTED BY: img_pcx_isinstance_c01
	if (sig != 0x0A) return DefinitelyNo;

	// TESTED BY: img_pcx_isinstance_c02
	if (
		(ver != 0x00) &&
		(ver != 0x02) &&
		(ver != 0x03) &&
		(ver != 0x05)
	) return DefinitelyNo; // unsupported version

	// TESTED BY: img_pcx_isinstance_c00
	return PossiblyYes;
}

ImagePtr PCXImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
	throw (stream::error)
{
	// TODO: Create blank PCX
	return ImagePtr(new PCXImage(psImage));
}

ImagePtr PCXImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
	throw (stream::error)
{
	return ImagePtr(new PCXImage(psImage));
}

SuppFilenames PCXImageType::getRequiredSupps(const std::string& filenameImage) const
	throw ()
{
	return SuppFilenames();
}


PCXImage::PCXImage(stream::inout_sptr data)
	throw (stream::error) :
		data(data)
{
	this->data->seekg(0, stream::end);
	unsigned long lenData = this->data->tellg();

	this->data->seekg(1, stream::start);
	uint16_t xmin, ymin, xmax, ymax;
	this->data
		>> u8(this->ver)
		>> u8(this->encoding)
		>> u8(this->bitsPerPlane)
		>> u16le(xmin)
		>> u16le(ymin)
		>> u16le(xmax)
		>> u16le(ymax)
	;
	this->width = xmax - xmin + 1;
	this->height = ymax - ymin + 1;

	this->data->seekg(65, stream::start);
	this->data
		>> u8(this->numPlanes)
	;
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

	this->data->seekg(128, stream::start);
/*
	this->data->seekg(0, stream::end);
	unsigned long lenData = this->data->tellg();
	stream::sub_sptr sub(new stream::sub(this->data, 128, lenData - 128));
*/

/* TEMP
	filtered_stream::input_sptr pinf(new filtered_istream());
	if (this->encoding == 1) {
		pinf->push(pcx_unrle_filter());
	}
	pinf->pushShared(this->data); // TODO: make stream::sub?
	//pinf->pushShared(sub);
*/
	uint8_t *line = imgData;
	int repeat = 0;
	uint8_t byte;
	bitstream_sptr bits(new bitstream(bitstream::bigEndian));
//TEMP	fn_getnextchar cbNext = boost::bind(nextChar, boost::dynamic_pointer_cast<std::istream>(pinf), _1);
	fn_getnextchar cbNext;
	int val;
	for (int y = 0; y < height; y++) {
		memset(line, 0, width); // blank out line
		for (int p = 0; p < this->numPlanes; p++) {
			for (int x = 0; x < width; x++) {
				bits->read(cbNext, this->bitsPerPlane, &val);
				line[x] |= val << (p * this->bitsPerPlane);
			}
			// Skip over any EOL padding.  Doing it this way avoids the need to
			// write a seek() function in the unrle filter.
			bits->flushByte();
			int pad = bytesPerPlaneScanline - ((this->bitsPerPlane * width + 7) / 8);
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
	// Pad out to a multiple of four bytes
	bytesPerPlaneScanline += bytesPerPlaneScanline % 4;

	// Assume worst case and enlarge file enough to fit complete data
	this->data->truncate(128+bytesPerPlaneScanline*2 * this->numPlanes * height + 768+1);

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

	// TODO: CGA palette

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

	stream::output_sptr poutf;
/* TEMP
	filtered_stream::output_sptr poutf(new filtered_ostream());
	pcx_rle_filter rle;
	if (this->encoding == 1) poutf->push(io::invert(rle));
	poutf->pushShared(this->data); // TODO: make stream::sub?
	//pinf->pushShared(sub);
*/
	uint8_t *line = newContent.get();
	int repeat = 0;
	uint8_t byte;
	bitstream_sptr bits(new bitstream(bitstream::bigEndian));
//TEMP	fn_putnextchar cbNext = boost::bind(putNextChar, boost::dynamic_pointer_cast<std::ostream>(poutf), _1);
	fn_putnextchar cbNext;
	int val;

	int planeMask = (1 << this->bitsPerPlane) - 1;
	int pad = bytesPerPlaneScanline - ((this->bitsPerPlane * width + 7) / 8);

	for (int y = 0; y < height; y++) {
		for (int p = 0; p < this->numPlanes; p++) {
			int bitsInPlane = (p * this->bitsPerPlane);
			for (int x = 0; x < width; x++) {
				val = (line[x] >> bitsInPlane) & planeMask;
				bits->write(cbNext, this->bitsPerPlane, val);
			}
			// Pad up to next byte boundary
			bits->flushByte(cbNext);

			// Pad scanline to multiple of four bytes
			for (int z = 0; z < pad; z++) cbNext(0x00);
		}
		line += width;
	}

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

	poutf->flush();
	poutf->seekp(0, stream::cur); // workaround for boost
	poutf.reset(); // force flush
	this->data->flush();
	//stream::pos pos = this->data->tellp();
/*TEMP
	stream::pos pos = 128 + rle.getBytesWritten(); // TODO: plus VGA pal
	this->data->truncate(pos);
std::cout << "trunc to " << pos << "\n";
*/
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

		// TODO: Handle CGA palette

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
