/**
 * @file  img-bash-sprite.cpp
 * @brief Image implementation for Monster Bash sprite frames.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Monster_Bash_Sprite_Format
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
#include <camoto/stream_string.hpp>
#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp> // make_unique
#include "img-bash-sprite.hpp"
#include "img-ega-planar.hpp"

namespace camoto {
namespace gamegraphics {

/// Flag set when the image is more than 64 pixels wide.
#define MBF_WIDE (1<<6)

Image_BashSprite::Image_BashSprite(std::unique_ptr<stream::inout> content)
	:	content(std::move(content))
{
	this->content->seekg(0, stream::start);
	uint8_t reserved, x, y;
	*this->content
		>> u8(this->flags)
		>> u8(y)
		>> u8(x)
		>> u8(reserved)
		>> s16le(this->ptHotspot.x)
		>> s16le(this->ptHotspot.y)
		>> u16le(this->ptHitRect.x)
		>> u16le(this->ptHitRect.y)
	;
	this->dims.x = x;
	this->dims.y = y;
	this->ptHotspot.x *= -1;
	this->ptHotspot.y *= -1;
}

Image_BashSprite::~Image_BashSprite()
{
}

Image::Caps Image_BashSprite::caps() const
{
	return
		Caps::SetDimensions
		| Caps::HasHotspot
		| Caps::HasHitRect
	;
}

ColourDepth Image_BashSprite::colourDepth() const
{
	return ColourDepth::EGA;
}

Point Image_BashSprite::dimensions() const
{
	return this->dims;
}

void Image_BashSprite::dimensions(const Point& newDimensions)
{
	if (this->content->size() < 12) {
		// Need to enlarge stream to write image size
		this->content->truncate(12);
	}
	this->content->seekp(0, stream::start);

	this->dims = newDimensions;

	if (this->dims.x > 64) this->flags |= MBF_WIDE;
	else this->flags &= ~MBF_WIDE;

	*this->content
		<< u8(this->flags)
		<< u8(this->dims.y)
		<< u8(this->dims.x)
	;
	return;
}

Point Image_BashSprite::hotspot() const
{
	return this->ptHotspot;
}

void Image_BashSprite::hotspot(const Point& newHotspot)
{
	if (this->content->size() < 12) {
		// Need to enlarge stream to write image size
		this->content->truncate(12);
	}
	this->content->seekp(4, stream::start);

	this->ptHotspot = newHotspot;

	*this->content
		<< s16le(-this->ptHotspot.x)
		<< s16le(-this->ptHotspot.y)
	;
	return;
}

Point Image_BashSprite::hitrect() const
{
	return this->ptHitRect;
}

void Image_BashSprite::hitrect(const Point& newHitRect)
{
	if (this->content->size() < 12) {
		// Need to enlarge stream to write image size
		this->content->truncate(12);
	}
	this->content->seekp(8, stream::start);

	this->ptHitRect = newHitRect;

	*this->content
		<< u16le(this->ptHitRect.x)
		<< u16le(this->ptHitRect.y)
	;
	return;
}

Pixels Image_BashSprite::convert() const
{
	std::shared_ptr<Image> ega = this->toEGA();
	return ega->convert();
}

Pixels Image_BashSprite::convert_mask() const
{
	std::shared_ptr<Image> ega = this->toEGA();
	return ega->convert_mask();
}

void Image_BashSprite::convert(const Pixels& newContent, const Pixels& newMask)
{
	auto dims = this->dimensions();
	assert((dims.x != 0) && (dims.y != 0));

	auto egaContent = std::make_unique<stream::string>();
	auto egaData = &egaContent->data;
	auto ega = std::make_unique<Image_EGA_Planar>(
		std::move(egaContent),
		0,
		this->dimensions(),
		EGAPlaneLayout{
			EGAPlanePurpose::Opaque0,
				EGAPlanePurpose::Blue1,
				EGAPlanePurpose::Green1,
				EGAPlanePurpose::Red1,
				EGAPlanePurpose::Intensity1
				},
		this->pal
	);
	ega->convert(newContent, newMask);

	unsigned int widthBytes = (dims.x + 7) / 8;
	unsigned int planeSize = widthBytes * dims.y;
	unsigned int dataSize = (planeSize + 1) * 5 + 1;
	Pixels imgData(dataSize, 0x00);

	// The transparency plane mask isn't a plane mask, but rather the width
	// of the image in bytes.
	imgData[0] = widthBytes;

	// Plane IDs
	imgData[(planeSize + 1)] = 0x01;
	imgData[(planeSize + 1) * 2] = 0x02;
	imgData[(planeSize + 1) * 3] = 0x04;
	imgData[(planeSize + 1) * 4] = 0x08;
	imgData[(planeSize + 1) * 5] = 0x00; // terminator

	Pixels plane1(planeSize, 0x00);

	// Read blue, green, red and intensity as-is
	for (unsigned int p = 0; p < 5; p++) {
		memcpy(
			imgData.data() + (planeSize + 1) * p + 1,
			egaData->data() + planeSize * p,
			planeSize
		);
	}

	// If the image is not a multiple of eight pixels, the game still draws it
	// as such.  This means the transparency bits after the right-edge of the
	// image up until the next eight-pixel boundary must still be set to
	// transparent, otherwise pixels will be drawn in the game that aren't part
	// of the image.
	unsigned int unusedBits = dims.x % 8;
	if (unusedBits) {
		unsigned int padBits = ((256 >> unusedBits) - 1);
		for (unsigned int p = 1; p < 5; p++) {
			for (unsigned int y = 0; y < dims.y; y++) {
				imgData[(planeSize + 1) * p + 1 + y * widthBytes + (widthBytes - 1)] &= ~padBits;
			}
		}
		// And set these bits in the transparency layer to make them transparent
		for (unsigned int y = 0; y < dims.y; y++) {
			imgData[1 + y * widthBytes + (widthBytes - 1)] |= padBits;
		}
	}

	this->content->truncate(12 + dataSize);
	this->content->seekp(12, stream::start);
	this->content->write(imgData.data(), dataSize);
	return;
}

std::unique_ptr<Image> Image_BashSprite::toEGA() const
{
	auto dims = this->dimensions();
	assert((dims.x != 0) && (dims.y != 0));

	unsigned int lenRow = (dims.x + 7) / 8;

	unsigned int lenPlane = lenRow * dims.y;
	unsigned int lenOut = lenPlane * 5;
	Pixels imgData(lenOut, 0x00);
	Pixels inPlane(lenPlane, 0x00);

	stream::pos lenRemaining = this->content->size() - 12;
	this->content->seekg(12, stream::start);
	// Convert the data into EGA planar format
	bool firstIncomingPlane = true;
	while (lenRemaining >= lenRow + 1) {
		uint8_t plane;
		*this->content >> u8(plane);
		if (plane == 0x00) break; // EOF
		this->content->read(inPlane.data(), lenPlane);
		lenRemaining -= lenPlane + 1;
		if (firstIncomingPlane) {
			// First incoming plane is transparency (output plane 5)
			memcpy(imgData.data() + lenPlane * 4, inPlane.data(), lenPlane);
			firstIncomingPlane = false;
			// Transparency plane does not affect any other planes
			continue;
		}
		for (unsigned int p = 0; p < 4; p++) {
			if ((plane >> p) & 1) {
				// This image plane contains data for EGA plane p
				auto targetPlane = imgData.data() + lenPlane * p;
				for (unsigned int i = 0; i < lenPlane; i++) {
					targetPlane[i] ^= inPlane[i];
				}
			}
		}
	}
	auto egaContent = std::make_unique<stream::string>(
		std::string((const char *)imgData.data(), lenOut)
	);

	return std::make_unique<Image_EGA_Planar>(
		std::move(egaContent), 0, this->dimensions(),
		EGAPlaneLayout{
			EGAPlanePurpose::Blue1,
			EGAPlanePurpose::Green1,
			EGAPlanePurpose::Red1,
			EGAPlanePurpose::Intensity1,
			EGAPlanePurpose::Opaque0
		},
		this->pal
	);
}

} // namespace gamegraphics
} // namespace camoto
