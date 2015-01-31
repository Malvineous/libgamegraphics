/**
 * @file  img-cga.cpp
 * @brief Image implementation adding support for CGA graphics.
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

#include "img-cga.hpp"

namespace camoto {
namespace gamegraphics {

Image_CGA::Image_CGA(stream::inout_sptr data, stream::pos off,
	unsigned int width, unsigned int height, CGAPaletteType cgaPal)
	:	parent(data),
		data(new bitstream(data, bitstream::bigEndian)),
		off(off),
		width(width),
		height(height),
		cgaPal(cgaPal)
{
}

Image_CGA::~Image_CGA()
{
}

int Image_CGA::getCaps()
{
	return ColourDepthCGA | HasPalette;
}

void Image_CGA::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->width;
	*height = this->height;
	return;
}

void Image_CGA::setDimensions(unsigned int width, unsigned int height)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	this->width = width;
	this->height = height;
	return;
}

StdImageDataPtr Image_CGA::toStandard()
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	this->data->seek(this->off << 3, stream::start);

	// Read the data as 2bpp and write it to the buffer as 8bpp
	while (dataSize--) {
		unsigned int val;
		this->data->read(2, &val);
		*imgData++ = (uint8_t)val;
	}

	return ret;
}

StdImageDataPtr Image_CGA::toStandardMask()
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

void Image_CGA::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	// Cut off any leftover data or resize so there's enough space
	this->parent->truncate((dataSize >> 2) + this->off);

	// No conversion needed, write out as-is
	uint8_t *imgData = (uint8_t *)newContent.get();
	this->data->seek(this->off << 3, stream::start);

	// Write the data as 2bpp
	while (dataSize--) {
		this->data->write(2, *imgData++);
	}

	return;
}

PaletteTablePtr Image_CGA::getPalette()
{
	return createPalette_CGA(this->cgaPal);
}


//
// ImageType_CGARawLinear
//

ImageType_CGARawLinear::ImageType_CGARawLinear()
{
}

ImageType_CGARawLinear::~ImageType_CGARawLinear()
{
}

std::string ImageType_CGARawLinear::getCode() const
{
	return "img-cga-raw-linear-fullscreen";
}

std::string ImageType_CGARawLinear::getFriendlyName() const
{
	return "Raw Linear CGA fullscreen image";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> ImageType_CGARawLinear::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	return vcExtensions;
}

std::vector<std::string> ImageType_CGARawLinear::getGameList() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty ImageType_CGARawLinear::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();

	// TESTED BY: TODO
	if (len == 16000) return PossiblyYes;

	// TESTED BY: TODO
	return DefinitelyNo;
}

ImagePtr ImageType_CGARawLinear::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	psImage->truncate(16000);
	psImage->seekp(0, stream::start);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 250; i++) psImage->write(buf, 64);

	SuppData dummy;
	return this->open(psImage, dummy);
}

ImagePtr ImageType_CGARawLinear::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	Image_CGA *cga = new Image_CGA(psImage, 0, 320, 200,
		CGAPal_CyanMagentaBright);
	ImagePtr img(cga);
	return img;
}

SuppFilenames ImageType_CGARawLinear::getRequiredSupps(const std::string& filenameImage) const
{
	return SuppFilenames();
}

} // namespace gamegraphics
} // namespace camoto
