/**
 * @file  pal-vga-raw.cpp
 * @brief Palette interface to 768-byte raw 6-bit VGA palette files.
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

#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp> // make_unique
#include "pal-vga-raw.hpp"

namespace camoto {
namespace gamegraphics {

ImageType_Palette_VGA::ImageType_Palette_VGA()
{
}

ImageType_Palette_VGA::~ImageType_Palette_VGA()
{
}

std::string ImageType_Palette_VGA::code() const
{
	return "pal-vga-raw";
}

std::string ImageType_Palette_VGA::friendlyName() const
{
	return "Standard VGA palette";
}

std::vector<std::string> ImageType_Palette_VGA::fileExtensions() const
{
	return {"pal"};
}

std::vector<std::string> ImageType_Palette_VGA::games() const
{
	return {};
}

ImageType::Certainty ImageType_Palette_VGA::isInstance(stream::input& content)
	const
{
	stream::pos len = content.size();

	// Allow palettes with 256 entries, but also ones with only 16 (Duke II)
	if ((len != 16 * 3) && (len != 256 * 3)) return DefinitelyNo;

	uint8_t buf[768];
	memset(buf, 0, 768);
	content.seekg(0, stream::start);
	content.try_read(buf, 768);
	for (int i = 0; i < 768; i++) {
		// Some palettes do use 64 instead of the max value of 63
		if (buf[i] > 64) return DefinitelyNo;
	}

	// See if the first colour is black, which is even more likely to mean it's
	// a VGA palette.
	if ((buf[0] == 0) && (buf[1] == 0) && (buf[2] == 0)) return DefinitelyYes;

	// TESTED BY: TODO
	return PossiblyYes;
}

std::unique_ptr<Image> ImageType_Palette_VGA::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Palette_VGA>(std::move(content), 6);
}

std::unique_ptr<Image> ImageType_Palette_VGA::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Palette_VGA>(std::move(content), 6);
}

SuppFilenames ImageType_Palette_VGA::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	// No supplemental types/empty list
	return {};
}


ImageType_VGA8Palette::ImageType_VGA8Palette()
{
}

ImageType_VGA8Palette::~ImageType_VGA8Palette()
{
}

std::string ImageType_VGA8Palette::code() const
{
	return "pal-vga-raw8";
}

std::string ImageType_VGA8Palette::friendlyName() const
{
	return "8-bit per channel (24-bit RGB) palette";
}

std::vector<std::string> ImageType_VGA8Palette::fileExtensions() const
{
	return {"pal"};
}

std::vector<std::string> ImageType_VGA8Palette::games() const
{
	return {};
}

ImageType::Certainty ImageType_VGA8Palette::isInstance(stream::input& content)
	const
{
	stream::pos len = content.size();

	if (len != 768) return DefinitelyNo;

	// See if the first colour is black, which is even more likely to mean it's
	// a VGA palette.
	content.seekg(0, stream::start);
	uint8_t buf[3];
	content.read((char *)buf, 3);
	if ((buf[0] == 0) && (buf[1] == 0) && (buf[2] == 0)) return DefinitelyYes;

	// TESTED BY: TODO
	return PossiblyYes;
}

std::unique_ptr<Image> ImageType_VGA8Palette::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Palette_VGA>(std::move(content), 8);
}

std::unique_ptr<Image> ImageType_VGA8Palette::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Palette_VGA>(std::move(content), 8);
}

SuppFilenames ImageType_VGA8Palette::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	// No supplemental types/empty list
	return {};
}


Palette_VGA::Palette_VGA(std::unique_ptr<stream::inout> content,
	unsigned int depth)
	:	content(std::move(content)),
		depth(depth)
{
}

Palette_VGA::~Palette_VGA()
{
}

Image::Caps Palette_VGA::caps() const
{
	return Caps::HasPalette | Caps::SetPalette;
}

ColourDepth Palette_VGA::colourDepth() const
{
	return ColourDepth::VGA;
}

std::shared_ptr<const Palette> Palette_VGA::palette() const
{
	this->content->seekg(0, stream::start);

	std::shared_ptr<Palette> pal(new Palette());
	pal->reserve(256);

	for (unsigned int i = 0; i < 256; i++) {
		PaletteEntry p;
		try {
			*this->content
				>> u8(p.red)
				>> u8(p.green)
				>> u8(p.blue)
			;
		} catch (const stream::incomplete_read&) {
			break;
		}
		if (this->depth == 6) {
			p.red = pal_6to8(p.red);
			p.green = pal_6to8(p.green);
			p.blue = pal_6to8(p.blue);
		}
		p.alpha = 255;
		pal->push_back(p);
	}
	// If the palette data is cut off (short read) the rest of the entries will
	// be missing.

	return pal;
}

void Palette_VGA::palette(std::shared_ptr<const Palette> newPalette)
{
	uint8_t buf[768];
	memset(buf, 0, 768);
	int i = 0;
	unsigned int shift;
	switch (this->depth) {
		default:
		case 6: shift = 2; break;
		case 8: shift = 0; break;
	}
	for (auto& p : *newPalette) {
		buf[i++] = p.red >> shift;
		buf[i++] = p.green >> shift;
		buf[i++] = p.blue >> shift;
	}
	this->content->truncate(768);
	this->content->seekp(0, stream::start);
	this->content->write(buf, 768);
	return;
}

} // namespace gamegraphics
} // namespace camoto
