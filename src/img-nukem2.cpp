/**
 * @file  img-nukem2.cpp
 * @brief Image implementation for Duke Nukem II full screen images.
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
#include <iostream>
#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp> // make_unique
#include "img-ega-planar.hpp"
#include "pal-vga-raw.hpp"
#include "img-nukem2.hpp"

/// Width of image, in pixels
#define N2IMG_WIDTH 320

// Height of image, in pixels
#define N2IMG_HEIGHT 200

/// Length of trailing palette data, in bytes
#define N2IMG_PALSIZE (16 * 3)

namespace camoto {
namespace gamegraphics {

/// Duke Nukem II full-screen Image implementation.
class Image_Nukem2: public Image_EGA_Planar
{
	public:
		Image_Nukem2(std::unique_ptr<stream::inout> content);
		virtual ~Image_Nukem2();

		virtual Caps caps() const;
		virtual void convert(const Pixels& newContent,
			const Pixels& newMask);
		virtual void palette(std::shared_ptr<const Palette> newPalette);
		using Image_EGA::palette;
};

ImageType_Nukem2::ImageType_Nukem2()
{
}

ImageType_Nukem2::~ImageType_Nukem2()
{
}

std::string ImageType_Nukem2::code() const
{
	return "img-nukem2";
}

std::string ImageType_Nukem2::friendlyName() const
{
	return "Duke Nukem II full-screen image";
}

std::vector<std::string> ImageType_Nukem2::fileExtensions() const
{
	return {"mni"};
}

std::vector<std::string> ImageType_Nukem2::games() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Duke Nukem II");
	return vcGames;
}

ImageType::Certainty ImageType_Nukem2::isInstance(
	stream::input& content) const
{
	// Files are a fixed size.
	// TESTED BY: img_nukem2_isinstance_c01
	if (content.size() != (N2IMG_WIDTH * N2IMG_HEIGHT / 2 + N2IMG_PALSIZE)) {
		return DefinitelyNo;
	}

	// Make sure the palette values are within range.
	uint8_t buf[N2IMG_PALSIZE];
	memset(buf, 0, sizeof(buf));
	content.seekg(N2IMG_WIDTH * N2IMG_HEIGHT / 2, stream::start);
	content.read(buf, sizeof(buf));

	for (int i = 0; i < N2IMG_PALSIZE; i++) {
		if (buf[i] > 0x40) {
			// Palette entry out of range
			// TESTED BY: img_nukem2_isinstance_c02
			return DefinitelyNo;
		}
	}

	// TESTED BY: img_nukem2_isinstance_c00
	return DefinitelyYes;
}

std::unique_ptr<Image> ImageType_Nukem2::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(N2IMG_WIDTH * N2IMG_HEIGHT / 2 + N2IMG_PALSIZE);
	return std::make_unique<Image_Nukem2>(std::move(content));
}

std::unique_ptr<Image> ImageType_Nukem2::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_unique<Image_Nukem2>(std::move(content));
}

SuppFilenames ImageType_Nukem2::getRequiredSupps(
	const std::string& filenameImage) const
{
	return SuppFilenames();
}


Image_Nukem2::Image_Nukem2(std::unique_ptr<stream::inout> content)
	:	Image_EGA_Planar(
			std::move(content),
			0,
			Point{N2IMG_WIDTH, N2IMG_HEIGHT},
			EGAPlaneLayout{
				EGAPlanePurpose::Blue1,
				EGAPlanePurpose::Green1,
				EGAPlanePurpose::Red1,
				EGAPlanePurpose::Intensity1,
				EGAPlanePurpose::Unused,
				EGAPlanePurpose::Unused,
			},
			nullptr
		)
{
	// Load palette
	auto vgaPal = createPalette_DefaultEGA();
	try {
		this->content->seekg(N2IMG_WIDTH * N2IMG_HEIGHT / 2, stream::start);
		// If the palette data is cut off (short read) the rest of the entries will
		// be default EGA.
		for (unsigned int i = 0; i < 16; i++) {
			PaletteEntry p;
			*this->content
				>> u8(p.red)
				>> u8(p.green)
				>> u8(p.blue)
			;
			p.red = pal_6to8(p.red);
			p.green = pal_6to8(p.green);
			p.blue = pal_6to8(p.blue);
			p.alpha = 255;
			(*vgaPal)[i] = p;
		}
	} catch (...) {
		std::cerr << "[img-nukem2] I/O error reading palette, leaving as "
			"partially overridden default\n";
	}
	this->palette(std::move(vgaPal));
}

Image_Nukem2::~Image_Nukem2()
{
}

Image::Caps Image_Nukem2::caps() const
{
	return Caps::HasPalette | Caps::SetPalette;
}

void Image_Nukem2::convert(const Pixels& newContent,
	const Pixels& newMask)
{
	this->Image_EGA_Planar::convert(newContent, newMask);

	auto vgaPal = this->palette();
	if (!vgaPal) vgaPal = createPalette_DefaultEGA();

	// Append the palette
	assert(vgaPal->size() <= 16);
	uint8_t buf[N2IMG_PALSIZE];
	memset(buf, 0, sizeof(buf));
	int i = 0;
	for (auto& p : *vgaPal) {
		buf[i++] = pal_8to6(p.red);
		buf[i++] = pal_8to6(p.green);
		buf[i++] = pal_8to6(p.blue);
	}

	this->content->truncate(N2IMG_WIDTH * N2IMG_HEIGHT / 2 + N2IMG_PALSIZE);
	this->content->seekp(N2IMG_WIDTH * N2IMG_HEIGHT / 2, stream::start);
	this->content->write(buf, N2IMG_PALSIZE);
	return;
}

void Image_Nukem2::palette(std::shared_ptr<const Palette> newPalette)
{
	if (newPalette && (newPalette->size() > 16)) {
		throw stream::error("Duke Nukem II full-screen images can only support 16 "
			"or fewer colours.");
	}
	this->Image::palette(newPalette);
	return;
}


} // namespace gamegraphics
} // namespace camoto
