/**
 * @file  image-from_tileset.hpp
 * @brief Combine some tiles together to form an Image.
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

#ifndef _CAMOTO_IMAGE_FROM_TILESET_HPP_
#define _CAMOTO_IMAGE_FROM_TILESET_HPP_

#include <camoto/gamegraphics/tileset.hpp>

namespace camoto {
namespace gamegraphics {

/// Create a single image from a collection of tiles.
class Image_FromTileset: public Image
{
	public:
		/// Create a single image from a collection of tiles.
		/**
		 * @param tileset
		 *   Tileset to source the tiles from.
		 *
		 * @param first
		 *   Index of the first tile to appear in the image.
		 *
		 * @param span
		 *   Number of tiles to skip to the next row.  Typically this is the
		 *   tileset's preferred width, in tiles.  Primarily used when the image
		 *   is a smaller width than the full tileset.
		 *
		 * @param dimsInTiles
		 *   Width and height of the final image, in tiles.
		 *
		 * @note Palette is sourced from the tileset if it has a palette, or the
		 *   first tile in the resulting image otherwise.  If neither of these have
		 *   a palette, caps() won't include HasPalette.
		 */
		Image_FromTileset(std::shared_ptr<Tileset> tileset,
			unsigned int first, unsigned int span, Point dimsInTiles);
		virtual ~Image_FromTileset();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual void dimensions(const Point& newDimensions);
		virtual Pixels convert() const;
		virtual Pixels convert_mask() const;
		virtual void convert(const Pixels& newContent, const Pixels& newMask);

	private:
		std::shared_ptr<Tileset> tileset;
		unsigned int first;
		unsigned int span;
		Point dimsInTiles;

		/// toImage: Populate this->pixels and this->mask, !toImage: populate tileset
		virtual void doConversion(bool toImage);

		// Cached content
		Pixels pixels;
		Pixels mask;
		Point dimsInPixels;
		std::shared_ptr<const Palette> pal;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMAGE_FROM_TILESET_HPP_
