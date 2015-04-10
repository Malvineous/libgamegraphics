/**
 * @file  tls-ega-apogee.hpp
 * @brief Common EGA tileset routines used by Apogee games
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

#ifndef _CAMOTO_TLS_EGA_APOGEE_HPP_
#define _CAMOTO_TLS_EGA_APOGEE_HPP_

#include "img-ega.hpp"
#include "tileset-fat.hpp"
#include "tileset-fat-fixed_tile_size.hpp"

namespace camoto {
namespace gamegraphics {

class Tileset_EGAApogee:
	virtual public Tileset_FAT,
	virtual public Tileset_FAT_FixedTileSize
{
	public:
		Tileset_EGAApogee(std::unique_ptr<stream::inout> content,
			Point tileDimensions, PlaneCount numPlanes, unsigned int idealWidth,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_EGAApogee();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		using Tileset_FAT::dimensions;
		virtual Point dimensions() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(FileHandle& id);
		virtual FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;

	protected:
		Point tileDimensions;
		PlaneCount numPlanes;
		unsigned int idealWidth;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_EGA_APOGEE_HPP_
