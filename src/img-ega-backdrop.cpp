/**
 * @file   img-ega-backdrop.cpp
 * @brief  Image implementation of EGA "backdrop" style tiled images.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Cosmo_Tileset_Format
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
#include "img-ega-byteplanar-tiled.hpp"
#include "img-ega-backdrop.hpp"
#include "tls-ega-apogee.hpp"

using namespace camoto;
using namespace camoto::gamegraphics;

BackdropImageType::BackdropImageType(unsigned int width, unsigned int height,
	unsigned int tileWidth, unsigned int tileHeight, unsigned int planeCount)
	:	width(width),
		height(height),
		tileWidth(tileWidth),
		tileHeight(tileHeight),
		planeCount(planeCount)
{
}

ImageType::Certainty BackdropImageType::isInstance(stream::input_sptr psImage)
	const
{
	unsigned int tileSize = this->tileWidth / 8 * this->tileHeight
		* this->planeCount;
	unsigned int expectedSize = this->width * this->height * tileSize;

	// Files are a fixed size.
	// TESTED BY: TODO
	if (psImage->size() != expectedSize) return DefinitelyNo;

	// TESTED BY: TODO
	return PossiblyYes;
}

ImagePtr BackdropImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return this->open(psImage, suppData);
}

ImagePtr BackdropImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	PLANE_LAYOUT planes;
	planes[PLANE_BLUE] = 1;
	planes[PLANE_GREEN] = 2;
	planes[PLANE_RED] = 3;
	planes[PLANE_INTENSITY] = 4;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 0;

	EGABytePlanarTiledImage *ega = new EGABytePlanarTiledImage();
	ImagePtr img(ega);
	ega->setParams(
		psImage, 0, this->width * this->tileWidth, this->height * this->tileHeight,
		planes, PaletteTablePtr()
	);

	return img;
}

SuppFilenames BackdropImageType::getRequiredSupps(
	const std::string& filenameImage) const
{
	return SuppFilenames();
}


CosmoBackdropImageType::CosmoBackdropImageType()
	:	BackdropImageType(
			 8,  8, // tile size
			40, 18, // image size (in tiles)
			EGA_NUMPLANES_SOLID
		)
{
}

std::string CosmoBackdropImageType::getCode() const
{
	return "img-cosmo-backdrop";
}

std::string CosmoBackdropImageType::getFriendlyName() const
{
	return "Cosmo's Cosmic Adventures level backdrop";
}

std::vector<std::string> CosmoBackdropImageType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("mni");
	return vcExtensions;
}

std::vector<std::string> CosmoBackdropImageType::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Cosmo's Cosmic Adventures");
	return vcGames;
}
