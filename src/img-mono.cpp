/**
 * @file  img-mono.cpp
 * @brief Image implementation adding support for monochrome graphics.
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

#include "img-mono.hpp"
#include "img-ega-planar.hpp"

namespace camoto {
namespace gamegraphics {

ImageType_Mono::ImageType_Mono()
{
}

ImageType_Mono::~ImageType_Mono()
{
}

std::string ImageType_Mono::getCode() const
{
	return "img-mono-raw-fullscreen";
}

std::string ImageType_Mono::getFriendlyName() const
{
	return "Raw monochrome fullscreen image";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> ImageType_Mono::getFileExtensions() const
{
	return {};
}

std::vector<std::string> ImageType_Mono::getGameList() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty ImageType_Mono::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();

	// TESTED BY: TODO
	if (len == 8000) return PossiblyYes;

	// TESTED BY: TODO
	return DefinitelyNo;
}

ImagePtr ImageType_Mono::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	psImage->truncate(8000);
	psImage->seekp(0, stream::start);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 125; i++) psImage->write(buf, 64);

	SuppData dummy;
	return this->open(psImage, dummy);
}

ImagePtr ImageType_Mono::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	Image_EGAPlanar *ega = new Image_EGAPlanar();
	ImagePtr img(ega);

	PLANE_LAYOUT planes;
	planes[PLANE_BLUE] = 0;
	planes[PLANE_GREEN] = 0;
	planes[PLANE_RED] = 0;
	planes[PLANE_INTENSITY] = 1;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 0;
	ega->setParams(psImage, 0, 320, 200, planes);

	return img;
}

SuppFilenames ImageType_Mono::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	return SuppFilenames();
}

} // namespace gamegraphics
} // namespace camoto
