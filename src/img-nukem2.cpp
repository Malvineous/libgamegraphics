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
class Image_Nukem2: virtual public Image_EGAPlanar
{
	public:
		Image_Nukem2(std::unique_ptr<stream::inout> content);
		virtual ~Image_Nukem2();

		virtual Image::Caps caps() const;
		virtual void convert(const Pixels& newContent,
			const Pixels& newMask);
		virtual void palette(std::shared_ptr<const Palette> newPalette);
		using Image_EGAPlanar::palette;

	protected:
		std::shared_ptr<Palette> vgaPal;       ///< Palette
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
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("mni");
	return vcExtensions;
}

std::vector<std::string> ImageType_Nukem2::games() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Duke Nukem II");
	return vcGames;
}

ImageType::Certainty ImageType_Nukem2::isInstance(stream::input& content) const
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
	:	Image_EGAPlanar(
			std::move(content),
			0,
			Point{N2IMG_WIDTH, N2IMG_HEIGHT},
			EGAPlaneLayout{
				PlanePurpose::Blue1,
				PlanePurpose::Green1,
				PlanePurpose::Red1,
				PlanePurpose::Intensity1,
				PlanePurpose::Unused,
				PlanePurpose::Unused,
			},
			nullptr
		)
{
	// Load palette
	try {
		auto vgaPal = std::make_shared<Palette>();
		vgaPal->reserve(16);
		uint8_t buf[N2IMG_PALSIZE];
		memset(buf, 0, sizeof(buf));
		this->content->seekg(N2IMG_WIDTH * N2IMG_HEIGHT / 2, stream::start);
		this->content->read(buf, sizeof(buf));
		// If the palette data is cut off (short read) the rest of the entries will
		// be black.
		int i = 0;
		while (i < N2IMG_PALSIZE) {
			PaletteEntry p;
			if (buf[i] >= 0x40) buf[i] = 0x3F;
			p.red   = (buf[i] << 2) | (buf[i] >> 4);
			i++;
			if (buf[i] >= 0x40) buf[i] = 0x3F;
			p.green = (buf[i] << 2) | (buf[i] >> 4);
			i++;
			if (buf[i] >= 0x40) buf[i] = 0x3F;
			p.blue  = (buf[i] << 2) | (buf[i] >> 4);
			i++;
			p.alpha = 255;
			vgaPal->push_back(p);
		}
		this->palette(vgaPal);
	} catch (...) {
		std::cerr << "[img-nukem2] I/O error reading palette, leaving as default\n";
		this->palette(createPalette_DefaultEGA());
	}
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
	this->Image_EGAPlanar::convert(newContent, newMask);

	auto vgaPal = this->palette();
	if (!vgaPal) vgaPal = createPalette_DefaultEGA();

	// Append the palette
	assert(vgaPal->size() <= 16);
	uint8_t buf[N2IMG_PALSIZE];
	memset(buf, 0, sizeof(buf));
	int i = 0;
	for (auto& p : *vgaPal) {
		buf[i++] = p.red >> 2;
		buf[i++] = p.green >> 2;
		buf[i++] = p.blue >> 2;
	}

	this->content->truncate(N2IMG_WIDTH * N2IMG_HEIGHT / 2 + N2IMG_PALSIZE);
	this->content->seekp(N2IMG_WIDTH * N2IMG_HEIGHT / 2, stream::start);
	this->content->write(buf, N2IMG_PALSIZE);
	return;
}

void Image_Nukem2::palette(std::shared_ptr<const Palette> newPalette)
{
	if (newPalette->size() > 16) {
		throw stream::error("Duke Nukem II full-screen images can only support 16 "
			"or fewer colours.");
	}
	this->Image::palette(newPalette);
	return;
}


} // namespace gamegraphics
} // namespace camoto
