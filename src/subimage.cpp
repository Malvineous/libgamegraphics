/**
 * @file   subimage.cpp
 * @brief  Image specialisation images within other images.
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

#include "subimage.hpp"

namespace camoto {
namespace gamegraphics {

SubImage::SubImage(ImagePtr img, int xOffset, int yOffset, int width, int height)
	throw (stream::error) :
		img(img),
		xOffset(xOffset),
		yOffset(yOffset),
		width(width),
		height(height)
{
}

SubImage::~SubImage()
	throw ()
{
}

int SubImage::getCaps()
	throw ()
{
	int parentCaps = this->img->getCaps();
	return parentCaps & Image::ColourDepthMask;
}

void SubImage::getDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->width;
	*height = this->height;
	return;
}

void SubImage::setDimensions(unsigned int width, unsigned int height)
	throw (stream::error)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	if ((width != this->width) || (height != this->height)) {
		throw stream::error("this image is a fixed size");
	}
	return;
}

StdImageDataPtr SubImage::toStandard()
	throw (stream::error)
{
	if (!this->parent) {
		this->parent = this->img->toStandard();
	}

	return this->extractPortion(this->parent);
}

StdImageDataPtr SubImage::toStandardMask()
	throw ()
{
	if (!this->parentMask) {
		this->parentMask = this->img->toStandardMask();
	}

	return this->extractPortion(this->parentMask);
}

void SubImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw (stream::error)
{
	if (!this->parent) {
		this->parent = this->img->toStandard();
	}
	if (!this->parentMask) {
		this->parentMask = this->img->toStandardMask();
	}

	uint8_t *parentData = this->parent.get();
	uint8_t *parentMask = this->parentMask.get();
	uint8_t *imgData = newContent.get();
	uint8_t *imgMask = newMask.get();

	unsigned int parentWidth, parentHeight;
	this->img->getDimensions(&parentWidth, &parentHeight);

	// How much left on each line after subimage
	int remX = parentWidth - this->xOffset - this->width;

	// Copy the data out of the subimage
	parentData += this->yOffset * parentWidth;
	parentMask += this->yOffset * parentWidth;
	for (int y = 0; y < this->height; y++) {
		parentData += this->xOffset;
		parentMask += this->xOffset;
		for (int x = 0; x < this->width; x++) {
			*parentData++ = *imgData++;
			*parentMask++ = *imgMask++;
		}
		parentData += remX;
		parentMask += remX;
	}

	// Update the parent image
	this->img->fromStandard(this->parent, this->parentMask);

	return;
}

StdImageDataPtr SubImage::extractPortion(const StdImageDataPtr& source)
	throw ()
{
	const uint8_t *parentData = source.get();

	unsigned long dataSize = this->width * this->height;
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);

	unsigned int parentWidth, parentHeight;
	this->img->getDimensions(&parentWidth, &parentHeight);

	// How much left on each line after subimage
	int remX = parentWidth - this->xOffset - this->width;

	// Copy the data out of the subimage
	parentData += this->yOffset * parentWidth;
	for (int y = 0; y < this->height; y++) {
		parentData += this->xOffset;
		for (int x = 0; x < this->width; x++) {
			*imgData++ = *parentData++;
		}
		parentData += remX;
	}

	return ret;
}

} // namespace gamegraphics
} // namespace camoto
