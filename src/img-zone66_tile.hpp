/**
 * @file  img-zone66_tile.hpp
 * @brief Image implementation for Zone66 tiles.
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

#ifndef _CAMOTO_IMG_ZONE66_TILE_HPP_
#define _CAMOTO_IMG_ZONE66_TILE_HPP_

#include <camoto/gamegraphics/imagetype.hpp>

namespace camoto {
namespace gamegraphics {

/// Filetype handler for Zone 66 tiles.
class ImageType_Zone66Tile: virtual public ImageType
{
	public:
		ImageType_Zone66Tile();
		virtual ~ImageType_Zone66Tile();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual std::vector<std::string> fileExtensions() const;
		virtual std::vector<std::string> games() const;
		virtual Certainty isInstance(stream::input& content) const;
		virtual std::unique_ptr<Image> create(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual std::unique_ptr<Image> open(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage)
			const;
};

// Image_Zone66Tile is declared here so tls-zone66 can create instances directly

/// Zone66 Image implementation for a tile within a tileset.
class Image_Zone66Tile: virtual public Image
{
	public:
		Image_Zone66Tile(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Image_Zone66Tile();

		virtual Caps caps() const;
		ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual void dimensions(const Point& newDimensions);
		virtual Pixels convert() const;
		virtual Pixels convert_mask() const;
		virtual void convert(const Pixels& newContent,
			const Pixels& newMask);

	protected:
		std::unique_ptr<stream::inout> content;
		Point dims;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_ZONE66_TILE_HPP_
