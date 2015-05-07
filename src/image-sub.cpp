/**
 * @file  subimage.cpp
 * @brief Image specialisation images within other images.
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

#include "image-sub.hpp"

namespace camoto {
namespace gamegraphics {

Image_Sub::Image_Sub(std::shared_ptr<Pixels> stdImg,
	std::shared_ptr<Pixels> stdMask, Point dimsFull, Rect dimsViewport,
	ColourDepth depth, std::shared_ptr<const Palette> pal,
	fn_image_changed fnImageChanged)
	:	stdImg(stdImg),
		stdMask(stdMask),
		dimsFull(dimsFull),
		dimsViewport(dimsViewport),
		depth(depth),
		pal(pal),
		fnImageChanged(fnImageChanged)
{
}

Image_Sub::~Image_Sub()
{
}

Image::Caps Image_Sub::caps() const
{
	return this->pal ? Caps::HasPalette : Caps::Default;
}

ColourDepth Image_Sub::colourDepth() const
{
	return this->depth;
}

Point Image_Sub::dimensions() const
{
	return {this->dimsViewport.width, this->dimsViewport.height};
}

Pixels Image_Sub::convert() const
{
	return this->extractPortion(*this->stdImg);
}

Pixels Image_Sub::convert_mask() const
{
	return this->extractPortion(*this->stdMask);
}

void Image_Sub::convert(const Pixels& newContent, const Pixels& newMask)
{
	auto dstImg = this->stdImg.get();
	auto dstMask = this->stdMask.get();
	auto srcImg = newContent.data();
	auto srcMask = newMask.data();

	// Copy the data out of the subimage
	dstImg += this->dimsViewport.y * this->dimsFull.x;
	dstMask += this->dimsViewport.y * this->dimsFull.x;
	for (int y = 0; y < this->dimsViewport.height; y++) {
		//dstImg += this->dimsViewport.x;
		//dstMask += this->dimsViewport.x;
		memcpy(dstImg + this->dimsViewport.x, srcImg, this->dimsViewport.width);
		memcpy(dstMask + this->dimsViewport.x, srcMask, this->dimsViewport.width);
		dstImg += this->dimsFull.x;
		dstMask += this->dimsFull.x;
		srcImg += this->dimsViewport.width;
		srcMask += this->dimsViewport.width;
		/*
		for (unsigned int x = 0; x < this->width; x++) {
			*dstImg++ = *srcImg++;
			*dstMask++ = *srcMask++;
			}
		dstImg += remX;
		dstMask += remX;*/
	}

	// Notify parent for eventual image update
	this->fnImageChanged();

	return;
}

Pixels Image_Sub::extractPortion(const Pixels& source) const
{
	Pixels dest;
	dest.reserve(this->dimsViewport.width * this->dimsViewport.height);

	auto src = source.data();
	auto dst = dest.data();

	// Copy the data out of the main image
	src += this->dimsViewport.y * this->dimsFull.x;
	for (int y = 0; y < this->dimsViewport.height; y++) {
		/*
		for (unsigned int x = 0; x < this->width; x++) {
			*imgData++ = *parentData++;
		}
		*/
		memcpy(dst, src + this->dimsViewport.x, this->dimsViewport.width);
		src += this->dimsFull.x;
		dst += this->dimsViewport.width;
	}

	return dest;
}

} // namespace gamegraphics
} // namespace camoto
