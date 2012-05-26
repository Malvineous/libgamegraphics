/**
 * @file   img-cga.cpp
 * @brief  Image implementation adding support for CGA graphics.
 *
 * Copyright (C) 2010-2012 Adam Nielsen <malvineous@shikadi.net>
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

CGAImage::CGAImage(stream::inout_sptr data, stream::pos off,
	unsigned int width, unsigned int height, CGAPaletteType cgaPal)
	throw ()
	: parent(data),
	  data(new bitstream(data, bitstream::bigEndian)),
	  off(off),
	  width(width),
	  height(height),
	  cgaPal(cgaPal)
{
}

CGAImage::~CGAImage()
	throw ()
{
}

int CGAImage::getCaps()
	throw ()
{
	return ColourDepthCGA | HasPalette;
}

void CGAImage::getDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->width;
	*height = this->height;
	return;
}

void CGAImage::setDimensions(unsigned int width, unsigned int height)
	throw (stream::error)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	this->width = width;
	this->height = height;
	return;
}

StdImageDataPtr CGAImage::toStandard()
	throw ()
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
		int val;
		this->data->read(2, &val);
		*imgData++ = (uint8_t)val;
	}

	return ret;
}

StdImageDataPtr CGAImage::toStandardMask()
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

void CGAImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw ()
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

PaletteTablePtr CGAImage::getPalette()
	throw (stream::error)
{
	return createPalette_CGA(this->cgaPal);
}


//
// CGARawLinearImageType
//

CGARawLinearImageType::CGARawLinearImageType()
	throw ()
{
}

CGARawLinearImageType::~CGARawLinearImageType()
	throw ()
{
}

std::string CGARawLinearImageType::getCode() const
	throw ()
{
	return "img-cga-raw-linear-fullscreen";
}

std::string CGARawLinearImageType::getFriendlyName() const
	throw ()
{
	return "Raw Linear CGA fullscreen image";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> CGARawLinearImageType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	return vcExtensions;
}

std::vector<std::string> CGARawLinearImageType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty CGARawLinearImageType::isInstance(stream::input_sptr psImage) const
	throw (stream::error)
{
	stream::pos len = psImage->size();

	// TESTED BY: TODO
	if (len == 16000) return PossiblyYes;

	// TESTED BY: TODO
	return DefinitelyNo;
}

ImagePtr CGARawLinearImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
	throw (stream::error)
{
	psImage->truncate(16000);
	psImage->seekp(0, stream::start);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 250; i++) psImage->write(buf, 64);

	SuppData dummy;
	return this->open(psImage, dummy);
}

ImagePtr CGARawLinearImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
	throw (stream::error)
{
	CGAImage *cga = new CGAImage(psImage, 0, 320, 200,
		CGAPal_CyanMagentaBright);
	ImagePtr img(cga);
	return img;
}

SuppFilenames CGARawLinearImageType::getRequiredSupps(const std::string& filenameImage) const
	throw ()
{
	return SuppFilenames();
}

} // namespace gamegraphics
} // namespace camoto
