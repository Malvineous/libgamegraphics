/**
 * @file   img-gra-vinyl.cpp
 * @brief  Vinyl Goddess From Mars SCR image format.
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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
#include "img-scr-vinyl.hpp"
#include "pal-vga-raw.hpp"

/// Offset where planar VGA data begins
#define SCR_DATA_OFFSET 0

/// Depth of the Vinyl palette file
#define SCR_PALETTE_DEPTH 8

namespace camoto {
namespace gamegraphics {

std::string VinylSCRImageType::getCode() const
{
	return "img-scr-vinyl";
}

std::string VinylSCRImageType::getFriendlyName() const
{
	return "Vinyl Goddess From Mars SCR image";
}

std::vector<std::string> VinylSCRImageType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("gra");
	return vcExtensions;
}

std::vector<std::string> VinylSCRImageType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Vinyl Goddess From Mars");
	return vcGames;
}

ImageType::Certainty VinylSCRImageType::isInstance(stream::input_sptr psImage) const
{
	stream::len len = psImage->size();
	if (len != 320*200) return DefinitelyNo;
	return Unsure;
}

ImagePtr VinylSCRImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	psImage->truncate(SCR_DATA_OFFSET);
	psImage->seekp(0, stream::start);
	psImage
		<< u16le(0)
		<< u16le(0)
	;

	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], SCR_PALETTE_DEPTH));
		pal = palFile->getPalette();
	}
	return ImagePtr(new VinylSCRImage(psImage, pal));
}

ImagePtr VinylSCRImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], SCR_PALETTE_DEPTH));
		pal = palFile->getPalette();
	}
	return ImagePtr(new VinylSCRImage(psImage, pal));
}

SuppFilenames VinylSCRImageType::getRequiredSupps(const std::string& filenameImage) const
{
	SuppFilenames supps;
	std::string filenameBase =
		filenameImage.substr(0, filenameImage.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".pal";
	return supps;
}


VinylSCRImage::VinylSCRImage(stream::inout_sptr data, PaletteTablePtr pal)
	:	data(data),
		pal(pal)
{
}

VinylSCRImage::~VinylSCRImage()
{
}

int VinylSCRImage::getCaps()
{
	return Image::ColourDepthVGA
		| (this->pal ? Image::HasPalette : 0)
	;
}

void VinylSCRImage::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = 320;
	*height = 200;
	return;
}

StdImageDataPtr VinylSCRImage::toStandard()
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	unsigned long dataSize = width * height;

	uint8_t *rawData = new uint8_t[dataSize];
	StdImageDataPtr raw(rawData);
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	this->data->seekg(SCR_DATA_OFFSET, stream::start);
	this->data->read(rawData, dataSize);

	// Convert the planar data to linear
	unsigned int planeWidth = width / 4;
	unsigned int planeSize = planeWidth * height;
	for (unsigned int i = 0; i < dataSize; i++) {
		imgData[i % planeSize * 4 + i / planeSize] = rawData[i];
	}
	return ret;
}

StdImageDataPtr VinylSCRImage::toStandardMask()
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

void VinylSCRImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask)
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	unsigned int dataSize = width * height;

	uint8_t *rawData = new uint8_t[dataSize];
	StdImageDataPtr raw(rawData);

	// Convert the linear data to planar
	unsigned int planeWidth = width / 4;
	unsigned int planeSize = planeWidth * height;
	for (unsigned int i = 0; i < dataSize; i++) {
		rawData[i] = newContent[i % planeSize * 4 + i / planeSize];
	}

	this->data->seekp(SCR_DATA_OFFSET, stream::start);
	this->data->write(rawData, dataSize);

	return;
}

PaletteTablePtr VinylSCRImage::getPalette()
{
	return this->pal;
}

} // namespace gamegraphics
} // namespace camoto
