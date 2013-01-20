/**
 * @file  img-ccomic.cpp
 * @brief Image specialisation for Captain Comic full-screen images.
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

#include <camoto/iostream_helpers.hpp>
#include <camoto/stream_filtered.hpp>
#include "filter-ccomic.hpp"
#include "img-ccomic.hpp"

/// Width of image, in pixels
#define CCIMG_WIDTH 320

// Height of image, in pixels
#define CCIMG_HEIGHT 200

namespace camoto {
namespace gamegraphics {

CComicImageType::CComicImageType()
{
}

CComicImageType::~CComicImageType()
{
}

std::string CComicImageType::getCode() const
{
	return "img-ccomic";
}

std::string CComicImageType::getFriendlyName() const
{
	return "Captain Comic full-screen image";
}

std::vector<std::string> CComicImageType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("ega");
	return vcExtensions;
}

std::vector<std::string> CComicImageType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Captain Comic");
	return vcGames;
}

ImageType::Certainty CComicImageType::isInstance(stream::input_sptr psImage) const
{
	// TESTED BY: img_ccomic_isinstance_c02
	if (psImage->size() < 2) return DefinitelyNo;

	psImage->seekg(0, stream::start);

	uint16_t lenPlane;
	psImage
		>> u16le(lenPlane)
	;

	// TESTED BY: img_ccomic_isinstance_c01
	if (lenPlane != 8000) return DefinitelyNo;

	lenPlane *= 4;  // count all planes
	try {
		while (lenPlane) {
			uint8_t code;
			psImage >> u8(code);
			if (code & 0x80) {
				unsigned int repeat = code & 0x7F;
				if (repeat > lenPlane) return DefinitelyNo;
				psImage->seekg(1, stream::cur);
				lenPlane -= repeat;
			} else {
				if (code > lenPlane) return DefinitelyNo;
				lenPlane -= code;
				psImage->seekg(code, stream::cur);
			}
		}
	} catch (const stream::seek_error) {
		// TESTED BY: img_ccomic_isinstance_c03
		return DefinitelyNo;
	} catch (const stream::incomplete_read) {
		// TESTED BY: img_ccomic_isinstance_c04
		return DefinitelyNo;
	}

	// Should be no trailing data
	// TESTED BY: img_ccomic_isinstance_c05
	if (psImage->tellg() != psImage->size()) return DefinitelyNo;

	// TESTED BY: img_ccomic_isinstance_c00
	return DefinitelyYes;
}

ImagePtr CComicImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new CComicImage(psImage));
}

ImagePtr CComicImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new CComicImage(psImage));
}

SuppFilenames CComicImageType::getRequiredSupps(const std::string& filenameImage) const
{
	return SuppFilenames();
}


CComicImage::CComicImage(stream::inout_sptr data)
	:	data(data)
{
	filter_sptr filtRead(new filter_ccomic_unrle());
	filter_sptr filtWrite(new filter_ccomic_rle());
	stream::filtered_sptr decoded(new stream::filtered());
	decoded->open(data, filtRead, filtWrite, NULL);

	PLANE_LAYOUT planes;
	memset(planes, 0, sizeof(planes));
	planes[PLANE_BLUE] = 1;
	planes[PLANE_GREEN] = 2;
	planes[PLANE_RED] = 3;
	planes[PLANE_INTENSITY] = 4;
	this->setParams(decoded, 0, 320, 200, planes);
}

CComicImage::~CComicImage()
{
}

} // namespace gamegraphics
} // namespace camoto
