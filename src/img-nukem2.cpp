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

#include <camoto/iostream_helpers.hpp>
#include "img-nukem2.hpp"

/// Width of image, in pixels
#define N2IMG_WIDTH 320

// Height of image, in pixels
#define N2IMG_HEIGHT 200

/// Length of trailing palette data, in bytes
#define N2IMG_PALSIZE (16 * 3)

namespace camoto {
namespace gamegraphics {

Nukem2ImageType::Nukem2ImageType()
{
}

Nukem2ImageType::~Nukem2ImageType()
{
}

std::string Nukem2ImageType::getCode() const
{
	return "img-nukem2";
}

std::string Nukem2ImageType::getFriendlyName() const
{
	return "Duke Nukem II full-screen image";
}

std::vector<std::string> Nukem2ImageType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("mni");
	return vcExtensions;
}

std::vector<std::string> Nukem2ImageType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Duke Nukem II");
	return vcGames;
}

ImageType::Certainty Nukem2ImageType::isInstance(stream::input_sptr psImage) const
{
	// Files are a fixed size.
	// TESTED BY: img_nukem2_isinstance_c01
	if (psImage->size() != (N2IMG_WIDTH * N2IMG_HEIGHT / 2 + N2IMG_PALSIZE)) return DefinitelyNo;

	// Make sure the palette values are within range.
	uint8_t buf[N2IMG_PALSIZE];
	memset(buf, 0, sizeof(buf));
	psImage->seekg(N2IMG_WIDTH * N2IMG_HEIGHT / 2, stream::start);
	psImage->read(buf, sizeof(buf));

	for (int i = 0; i < N2IMG_PALSIZE; i++) {
		if (buf[i] > 0x40) {
			// TESTED BY: img_nukem2_isinstance_c02
			return DefinitelyNo;
		}
	}

	// TESTED BY: img_nukem2_isinstance_c00
	return DefinitelyYes;
}

ImagePtr Nukem2ImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new Nukem2Image(psImage));
}

ImagePtr Nukem2ImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new Nukem2Image(psImage));
}

SuppFilenames Nukem2ImageType::getRequiredSupps(const std::string& filenameImage) const
{
	return SuppFilenames();
}


Nukem2Image::Nukem2Image(stream::inout_sptr data)
	:	data(data)
{
	PLANE_LAYOUT planes;
	planes[PLANE_BLUE] = 1;
	planes[PLANE_GREEN] = 2;
	planes[PLANE_RED] = 3;
	planes[PLANE_INTENSITY] = 4;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 0;
	this->setParams(data, 0, N2IMG_WIDTH, N2IMG_HEIGHT, planes);

	// Load palette
	this->vgaPal.reset(new PaletteTable());
	this->vgaPal->reserve(16);
	uint8_t buf[N2IMG_PALSIZE];
	memset(buf, 0, sizeof(buf));
	this->data->seekg(N2IMG_WIDTH * N2IMG_HEIGHT / 2, stream::start);
	this->data->read(buf, sizeof(buf));
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
		this->vgaPal->push_back(p);
	}

}

Nukem2Image::~Nukem2Image()
{
}

int Nukem2Image::getCaps()
{
	return ColourDepthEGA | HasPalette;
}

void Nukem2Image::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = N2IMG_WIDTH;
	*height = N2IMG_HEIGHT;
	return;
}

void Nukem2Image::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
{
	this->EGAPlanarImage::fromStandard(newContent, newMask);

	// Append the palette
	assert(this->vgaPal->size() <= 16);
	uint8_t buf[N2IMG_PALSIZE];
	memset(buf, 0, sizeof(buf));
	int i = 0;
	for (PaletteTable::const_iterator p = this->vgaPal->begin(); p < this->vgaPal->end(); p++) {
		buf[i++] = p->red >> 2;
		buf[i++] = p->green >> 2;
		buf[i++] = p->blue >> 2;
	}
	this->data->seekp(0, stream::start);
	this->data->write(buf, N2IMG_PALSIZE);
	this->data->truncate_here();
	return;
}

PaletteTablePtr Nukem2Image::getPalette()
{
	return this->vgaPal;
}

void Nukem2Image::setPalette(PaletteTablePtr newPalette)
{
	if (newPalette->size() > 16) {
		throw stream::error("Duke Nukem II full-screen images can only support 16 "
			"or fewer colours.");
	}
	this->vgaPal = newPalette;
	return;
}


} // namespace gamegraphics
} // namespace camoto
