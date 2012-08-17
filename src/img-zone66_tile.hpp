/**
 * @file   img-zone66_tile.hpp
 * @brief  Image implementation for Zone66 tiles.
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
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
#include "baseimage.hpp"

namespace camoto {
namespace gamegraphics {

/// Filetype handler for Zone 66 tiles.
class Zone66TileImageType: virtual public ImageType {

	public:

		Zone66TileImageType();

		virtual ~Zone66TileImageType();

		virtual std::string getCode() const;

		virtual std::string getFriendlyName() const;

		virtual std::vector<std::string> getFileExtensions() const;

		virtual std::vector<std::string> getGameList() const;

		virtual Certainty isInstance(stream::input_sptr fsImage) const;

		virtual ImagePtr create(stream::inout_sptr psImage,
			SuppData& suppData) const;

		virtual ImagePtr open(stream::inout_sptr fsImage,
			SuppData& suppData) const;

		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage) const;

};

/// Zone66 Image implementation for a tile within a tileset.
class Zone66TileImage: virtual public BaseImage {
	protected:
		stream::inout_sptr data;
		PaletteTablePtr pal;
		int width, height;

	public:
		Zone66TileImage(stream::inout_sptr data,
			PaletteTablePtr pal);

		virtual ~Zone66TileImage();

		virtual int getCaps();

		virtual void getDimensions(unsigned int *width, unsigned int *height);

		virtual void setDimensions(unsigned int width, unsigned int height);

		virtual StdImageDataPtr toStandard();

		virtual StdImageDataPtr toStandardMask();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);

		virtual PaletteTablePtr getPalette();

		virtual void setPalette(PaletteTablePtr newPalette);

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_ZONE66_TILE_HPP_
