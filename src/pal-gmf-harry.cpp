/**
 * @file  pal-gmf-harry.cpp
 * @brief Interface to the palette stored inside Halloween Harry levels.
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
#include <camoto/util.hpp> // make_unique
#include "pal-gmf-harry.hpp"

namespace camoto {
namespace gamegraphics {

ImageType_Palette_HarryGMF::ImageType_Palette_HarryGMF()
{
}

ImageType_Palette_HarryGMF::~ImageType_Palette_HarryGMF()
{
}

std::string ImageType_Palette_HarryGMF::code() const
{
	return "pal-gmf-harry";
}

std::string ImageType_Palette_HarryGMF::friendlyName() const
{
	return "Halloween Harry VGA palette";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> ImageType_Palette_HarryGMF::fileExtensions() const
{
	return {"gmf"};
}

std::vector<std::string> ImageType_Palette_HarryGMF::games() const
{
	return {};
}

ImageType::Certainty ImageType_Palette_HarryGMF::isInstance(stream::input& content) const
{
	auto len = content.size();
	// Too short
	if (len < 0x1D + 768) return DefinitelyNo;

	content.seekg(0, stream::start);

	char sig[0x12];
	content.read(sig, 0x12);
	if (strncmp(sig, "\x11SubZero Game File", 0x12) != 0) return DefinitelyNo;

	content.seekg(0x1D, stream::start);

	// Check palette is within range
	char pal[768];
	content.read(pal, 768);
	for (int i = 0; i < 768; i++) {
		// TESTED BY: TODO
		if (pal[i] > 0x40) return DefinitelyNo;
	}

	return DefinitelyYes;
}

std::unique_ptr<Image> ImageType_Palette_HarryGMF::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Palette_HarryGMF>(std::move(content));
}

std::unique_ptr<Image> ImageType_Palette_HarryGMF::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Palette_HarryGMF>(std::move(content));
}

SuppFilenames ImageType_Palette_HarryGMF::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	// No supplemental types/empty list
	return {};
}


Palette_HarryGMF::Palette_HarryGMF(std::unique_ptr<stream::inout> content)
	:	content(std::move(content))
{
	assert(this->content);
}

Palette_HarryGMF::~Palette_HarryGMF()
{
}

Image::Caps Palette_HarryGMF::caps() const
{
	return Caps::HasPalette | Caps::SetPalette;
}

ColourDepth Palette_HarryGMF::colourDepth() const
{
	return ColourDepth::VGA;
}

std::shared_ptr<const Palette> Palette_HarryGMF::palette()
{
	auto pal = std::make_shared<Palette>();
	pal->reserve(256);

	uint8_t buf[768];
	memset(buf, 0, 768);
	this->content->seekg(0x1d, stream::start);
	this->content->read((char *)buf, 768);
	// If the palette data is cut off (short read) the rest of the entries will
	// be black.
	int i = 0;
	while (i < 768) {
		PaletteEntry p;
		p.red   = (buf[i] << 2) | (buf[i] >> 4);  i++;
		p.green = (buf[i] << 2) | (buf[i] >> 4);  i++;
		p.blue  = (buf[i] << 2) | (buf[i] >> 4);  i++;
		p.alpha = (i == 3) ? 0 : 255; // first entry is transparency
		pal->push_back(p);
	}

	return pal;
}

void Palette_HarryGMF::palette(std::shared_ptr<const Palette> newPalette)
{
	uint8_t buf[768];
	memset(buf, 0, 768);
	int i = 0;
	for (Palette::const_iterator p = newPalette->begin(); p < newPalette->end(); p++) {
		buf[i++] = p->red >> 2;
		buf[i++] = p->green >> 2;
		buf[i++] = p->blue >> 2;
	}
	this->content->truncate(768);
	this->content->seekp(0x1D, stream::start);
	this->content->write(buf, 768);
	return;
}

} // namespace gamegraphics
} // namespace camoto
