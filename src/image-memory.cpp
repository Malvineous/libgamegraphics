/**
 * @file  image-memory.cpp
 * @brief Implementation of an in-memory Image.
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

#include <camoto/gamegraphics/image-memory.hpp>

namespace camoto {
namespace gamegraphics {

Image_Memory::Image_Memory(const Point& dims, const Pixels& pixels,
	const Pixels& mask, const Point& hotspot, const Point& hitrect,
	std::shared_ptr<const Palette> pal)
	:	dims(dims),
		pixels(pixels),
		mask(mask),
		ptHotspot(hotspot),
		ptHitrect(hitrect)
{
	this->pal = pal;
}

Image_Memory::~Image_Memory()
{
}

Image::Caps Image_Memory::caps() const
{
	return Caps::Default | (this->pal ? Caps::HasPalette : Caps::Default);
}

ColourDepth Image_Memory::colourDepth() const
{
	return ColourDepth::VGA;
}

Point Image_Memory::dimensions() const
{
	return this->dims;
}

Point Image_Memory::hotspot() const
{
	return this->ptHotspot;
}

Point Image_Memory::hitrect() const
{
	return this->ptHitrect;
}

Pixels Image_Memory::convert() const
{
	return this->pixels;
}

Pixels Image_Memory::convert_mask() const
{
	return this->mask;
}

void Image_Memory::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	this->pixels = newContent;
	this->mask = newMask;
	return;
}

} // namespace gamegraphics
} // namespace camoto
