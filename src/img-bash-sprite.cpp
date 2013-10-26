/**
 * @file   img-bash-sprite.cpp
 * @brief  Image implementation for Monster Bash sprite frames.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Monster_Bash_Sprite_Format
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

#include <camoto/stream_memory.hpp>
#include <camoto/iostream_helpers.hpp>
#include "img-bash-sprite.hpp"
#include "img-ega-planar.hpp"

namespace camoto {
namespace gamegraphics {

BashSpriteImage::BashSpriteImage(stream::inout_sptr data)
	:	data(data)
{
	this->data->seekg(0, stream::start);
	uint8_t reserved;
	this->data
		>> u8(this->flags)
		>> u8(this->height)
		>> u8(this->width)
		>> u8(reserved)
		>> s16le(this->hotspotX)
		>> s16le(this->hotspotY)
		>> u16le(this->rectX)
		>> u16le(this->rectY)
	;
}

BashSpriteImage::~BashSpriteImage()
{
}

int BashSpriteImage::getCaps()
{
	return ColourDepthEGA | CanSetDimensions;
}

void BashSpriteImage::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->width;
	*height = this->height;
	return;
}

void BashSpriteImage::setDimensions(unsigned int width, unsigned int height)
{
	if (this->data->size() < 12) {
		// Need to enlarge stream to write image size
		this->data->truncate(12);
	}
	this->data->seekp(1, stream::start);

/// @todo Move this into a dedicated setHotspot/setHitmap function
	if ((width != this->width) || (height != this->height)) {
		this->hotspotX = this->hotspotY = 0;
		this->rectX = width;
		this->rectY = height;
	}

	this->width = width;
	this->height = height;

	this->data
		<< u8(this->height)
		<< u8(this->width)
		/// @todo Move this into a dedicated setHotspot/setHitmap function
		<< u8(0)
		<< s16le(this->hotspotX)
		<< s16le(this->hotspotY)
		<< u16le(this->rectX)
		<< u16le(this->rectY)
	;
	return;
}

StdImageDataPtr BashSpriteImage::toStandard()
{
	ImagePtr ega = this->toEGA();
	return ega->toStandard();
}

StdImageDataPtr BashSpriteImage::toStandardMask()
{
	ImagePtr ega = this->toEGA();
	return ega->toStandardMask();
}

void BashSpriteImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask)
{
	assert((this->width != 0) && (this->height != 0));

	stream::inout_sptr content(new stream::memory);
	{
		PLANE_LAYOUT planes;
		planes[PLANE_BLUE] = 1;
		planes[PLANE_GREEN] = 2;
		planes[PLANE_RED] = 3;
		planes[PLANE_INTENSITY] = 4;
		planes[PLANE_HITMAP] = 0;
		planes[PLANE_OPACITY] = 5;

		EGAPlanarImage *ega = new EGAPlanarImage();
		ImagePtr conv(ega);
		ega->setParams(content, 0, this->width, this->height, planes);
		ega->fromStandard(newContent, newMask);
	}
	content->seekg(0, stream::start);

	unsigned int planeSize = (this->width + 7) / 8 * this->height;
	unsigned int dataSize = (planeSize + 1) * 5 + 1;
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ptrData(imgData);
	memset(imgData, 0, dataSize);

	// Plane IDs
	imgData[0] = 0x01; // transparency also sets blue pixels
	imgData[(planeSize + 1)] = 0x01;
	imgData[(planeSize + 1) * 2] = 0x02;
	imgData[(planeSize + 1) * 3] = 0x04;
	imgData[(planeSize + 1) * 4] = 0x08;
	imgData[(planeSize + 1) * 5] = 0x00;

	uint8_t *plane1 = new uint8_t[planeSize];
	StdImageDataPtr ptrPlane1(plane1);

	// Read blue, green, red and intensity as-is
	for (unsigned int p = 1; p < 5; p++) {
		content->read(imgData + (planeSize + 1) * p + 1, planeSize);
	}

	// Read transparency (last in source EGA) into the first target plane
	content->read(imgData + 1, planeSize);

	// Undo the blue pixels that were set by the transparency+blue plane
	for (unsigned int i = 0; i < planeSize; i++) {
		imgData[(planeSize + 1) + 1 + i] ^= imgData[1 + i];
	}

	this->data->truncate(12 + dataSize);
	this->data->seekp(12, stream::start);
	this->data->write(imgData, dataSize);
	return;
}

ImagePtr BashSpriteImage::toEGA()
{
	assert((this->width != 0) && (this->height != 0));

	unsigned int lenRow = (this->width + 7) / 8;

	unsigned int lenPlane = lenRow * this->height;
	unsigned int lenOut = lenPlane * 5;
	uint8_t *imgData = new uint8_t[lenOut];
	StdImageDataPtr ptrData(imgData);
	memset(imgData, 0, lenOut);

	uint8_t *inPlane = new uint8_t[lenPlane];
	StdImageDataPtr ptrInPlane(inPlane);

	stream::pos lenRemaining = this->data->size() - 12;
	this->data->seekg(12, stream::start);
	// Convert the data into EGA planar format
	bool firstIncomingPlane = true;
	while (lenRemaining >= lenRow + 1) {
		uint8_t plane;
		this->data >> u8(plane);
		if (plane == 0x00) break; // EOF
		this->data->read(inPlane, lenPlane);
		lenRemaining -= lenPlane + 1;
		if (firstIncomingPlane) {
			// First incoming plane is transparency (output plane 5)
			memcpy(imgData + lenPlane * 4, inPlane, lenPlane);
			firstIncomingPlane = false;
		}
		for (unsigned int p = 0; p < 4; p++) {
			if ((plane >> p) & 1) {
				// This image plane contains data for EGA plane p
				uint8_t *targetPlane = imgData + lenPlane * p;
				for (unsigned int i = 0; i < lenPlane; i++) {
					targetPlane[i] ^= inPlane[i];
				}
			}
		}
	}
	stream::inout_sptr content(new stream::memory);
	content->write(imgData, lenOut);

	PLANE_LAYOUT planes;
	planes[PLANE_BLUE] = 1;
	planes[PLANE_GREEN] = 2;
	planes[PLANE_RED] = 3;
	planes[PLANE_INTENSITY] = 4;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 5;

	EGAPlanarImage *ega = new EGAPlanarImage();
	ImagePtr conv(ega);
	ega->setParams(content, 0, this->width, this->height, planes);
	return conv;
}

} // namespace gamegraphics
} // namespace camoto
