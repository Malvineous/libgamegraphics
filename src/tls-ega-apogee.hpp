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

#include <camoto/gamegraphics/tilesettype.hpp>
#include <camoto/gamegraphics/palettetable.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

/// Number of planes in each sprite (masked) image
#define EGA_NUMPLANES_MASKED  5

/// Number of planes in each tile (nonmasked) image
#define EGA_NUMPLANES_SOLID   4

class Tileset_EGAApogee: virtual public Tileset_FAT
{
	public:
		Tileset_EGAApogee(stream::inout_sptr data, unsigned int tileWidth,
			unsigned int tileHeight, unsigned int numPlanes, unsigned int idealWidth,
			PaletteTablePtr pal);
		virtual ~Tileset_EGAApogee();

		virtual int getCaps();
		void resize(EntryPtr& id, stream::len newSize);
		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height);
		virtual unsigned int getLayoutWidth();
		virtual PaletteTablePtr getPalette();

		// Tileset_FAT
		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);

	protected:
		unsigned int tileWidth;
		unsigned int tileHeight;
		unsigned int numPlanes;
		unsigned int idealWidth;
		PaletteTablePtr pal;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_EGA_APOGEE_HPP_
