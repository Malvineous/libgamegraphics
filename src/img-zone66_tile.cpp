/**
 * @file   img-zone66_tile.cpp
 * @brief  Image implementation for Zone66 tiles.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Zone_66_Tileset_Format
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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

#include <camoto/iostream_helpers.hpp>
#include "pal-vga-raw.hpp"
#include "img-zone66_tile.hpp"

/// Offset where the VGA image data begins
#define Z66_IMG_OFFSET 4

namespace camoto {
namespace gamegraphics {

Zone66TileImageType::Zone66TileImageType()
	throw ()
{
}

Zone66TileImageType::~Zone66TileImageType()
	throw ()
{
}

std::string Zone66TileImageType::getCode() const
	throw ()
{
	return "img-zone66_tile";
}

std::string Zone66TileImageType::getFriendlyName() const
	throw ()
{
	return "Zone 66 tile";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> Zone66TileImageType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	return vcExtensions;
}

std::vector<std::string> Zone66TileImageType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Zone 66");
	return vcGames;
}

ImageType::Certainty Zone66TileImageType::isInstance(iostream_sptr psImage) const
	throw (std::ios::failure)
{
	psImage->seekg(0, std::ios::end);
	io::stream_offset len = psImage->tellg();

	psImage->seekg(0, std::ios::beg);
	uint16_t width, height;
	psImage >> u16le(width) >> u16le(height);
	int imgSize = width * height;
	int pos = 0;
	while (pos < imgSize) {
		uint8_t code;
		psImage >> u8(code);
		len--;
		switch (code) {
			case 0x00: return DefinitelyNo; // doesn't make sense
			case 0xFF: // end of image
				if (len == 0) return DefinitelyYes; // EOF marker at EOF
				return DefinitelyNo; // EOF marker with trailing data
			case 0xFE: // skip to EOL
				pos += width - (pos % width);
				break;
			case 0xFD:
				psImage >> u8(code);
				pos += code;
				len -= code+1;
				break;
			default:
				pos += code;
				len -= code;
				break;
		}
	}

	// Read all data but didn't have 0xFF at EOF
	if ((len == 0) && (pos == imgSize)) return DefinitelyYes;

	// TESTED BY: TODO //fmt_grp_duke3d_isinstance_c01
	return DefinitelyNo;
}

ImagePtr Zone66TileImageType::create(iostream_sptr psImage,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	psImage
		<< u16le(0)  // width
		<< u16le(0)  // height
		<< u8(0xFF)  // end-of-image code
	;
	ImagePtr palFile(new VGAPalette(
		suppData[SuppItem::Palette].stream,
		suppData[SuppItem::Palette].fnTruncate
	));
	PaletteTablePtr pal = palFile->getPalette();
	return ImagePtr(new Zone66TileImage(psImage, fnTruncate, pal));
}

ImagePtr Zone66TileImageType::open(iostream_sptr psImage,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	ImagePtr palFile(new VGAPalette(
		suppData[SuppItem::Palette].stream,
		suppData[SuppItem::Palette].fnTruncate
	));
	PaletteTablePtr pal = palFile->getPalette();
	return ImagePtr(new Zone66TileImage(psImage, fnTruncate, pal));
}

MP_SUPPLIST Zone66TileImageType::getRequiredSupps(const std::string& filenameImage) const
	throw ()
{
	MP_SUPPLIST supps;
	supps[SuppItem::Palette] = "tpal.z66"; // TODO: case sensitivity?
	return supps;
}


Zone66TileImage::Zone66TileImage(iostream_sptr data,
	FN_TRUNCATE fnTruncate, PaletteTablePtr pal)
	throw () :
		data(data),
		fnTruncate(fnTruncate),
		pal(pal)
{
}


Zone66TileImage::~Zone66TileImage()
	throw ()
{
}

int Zone66TileImage::getCaps()
	throw ()
{
	return ColourDepthVGA | CanSetDimensions | HasPalette;
}

void Zone66TileImage::getDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	this->data->seekg(0, std::ios::beg);
	this->data >> u16le(*width) >> u16le(*height);
	return;
}

void Zone66TileImage::setDimensions(unsigned int width, unsigned int height)
	throw (std::ios::failure)
{
	this->data->seekp(0, std::ios::beg);
	this->data << u16le(width) << u16le(height);
	return;
}

StdImageDataPtr Zone66TileImage::toStandard()
	throw (std::ios::failure)
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	this->data->seekg(Z66_IMG_OFFSET, std::ios::beg);

	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	bool justDidReset = false;
	for (int i = 0; i < dataSize; ) {
		uint8_t code;
		this->data >> u8(code);
		switch (code) {
			case 0xFD: // Skip the given number of pixels
				this->data >> u8(code);
				// Pad with black
				while (code--) imgData[i++] = 0;
				break;

			case 0xFE: { // Skip to the next line
				// Pad rest of line (if any) with black
				int restOfLine = width - (i % width);
				// Don't skip a line when we're at EOL, unless we just omitted a skip
				if ((restOfLine != width) || (justDidReset)) {
					while (restOfLine--) imgData[i++] = 0;
				}
				justDidReset = true;
				continue;
			}

			case 0xFF: // End of image
				// Pad rest of image (if any) with black
				while (i < dataSize) imgData[i++] = 0;
				break;

			case 0x00: // shouldn't happen
				throw std::ios::failure("corrupted data");

			default:
				if (i + code <= dataSize) {
					this->data->rdbuf()->sgetn((char *)&imgData[i], code);
					i += code;
				} else {
					throw std::ios::failure("bad data, tried to write past end of image");
				}
				break;
		}
		justDidReset = false; // last code wasn't 0xFE/nextline
	}

	return ret;
}

StdImageDataPtr Zone66TileImage::toStandardMask()
	throw ()
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	// Return an entirely opaque mask
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	memset(imgData, 1, dataSize);

	return ret;
}

void Zone66TileImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw (std::ios::failure)
{
	throw std::ios::failure("not implemented yet");
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	uint8_t *imgData = (uint8_t *)newContent.get();
	this->data->seekp(Z66_IMG_OFFSET, std::ios::beg);
	this->data->rdbuf()->sputn((char *)imgData, dataSize);

	return;
}

PaletteTablePtr Zone66TileImage::getPalette()
	throw ()
{
	return this->pal;
}

void Zone66TileImage::setPalette(PaletteTablePtr newPalette)
	throw (std::ios::failure)
{
	// This doesn't save anything to the file as the palette is stored externally.
	this->pal = newPalette;
	return;
}

} // namespace gamegraphics
} // namespace camoto
