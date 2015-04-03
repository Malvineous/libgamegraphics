/**
 * @file  tileset-fat.cpp
 * @brief Generic FAT-based tileset handler.
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

#include <camoto/util.hpp> // std::make_unique
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

Tileset_FAT::Tileset_FAT(std::unique_ptr<stream::inout> data,
	stream::pos offFirstTile, int lenMaxName)
	: Archive_FAT(std::move(data), offFirstTile, lenMaxName)
{
}

Tileset_FAT::Tileset_FAT()
	: Archive_FAT(nullptr, 0, 0) // should never be called
{
}

Tileset_FAT::~Tileset_FAT()
{
}

std::unique_ptr<Image> Tileset_FAT::openImage(FileHandle& id)
{
	throw stream::error("Invalid file handle.");
}

std::shared_ptr<Tileset> Tileset_FAT::openTileset(FileHandle& id)
{
	throw stream::error("Invalid file handle.");
}

} // namespace gamearchive
} // namespace camoto
