/**
 * @file   img-zone66_tile.hpp
 * @brief  Image implementation for Zone66 tiles.
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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
#include <camoto/gamegraphics/image.hpp>

namespace camoto {
namespace gamegraphics {

/// Filetype handler for Zone 66 tiles.
class Zone66TileImageType: virtual public ImageType {

	public:

		Zone66TileImageType()
			throw ();

		virtual ~Zone66TileImageType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(iostream_sptr fsImage) const
			throw (std::ios::failure);

		virtual ImagePtr create(iostream_sptr psImage, FN_TRUNCATE fnTruncate,
			MP_SUPPDATA& suppData) const
			throw (std::ios::failure);

		virtual ImagePtr open(iostream_sptr fsImage, FN_TRUNCATE fnTruncate,
			MP_SUPPDATA& suppData) const
			throw (std::ios::failure);

		virtual MP_SUPPLIST getRequiredSupps(const std::string& filenameImage) const
			throw ();

};

/// Zone66 Image implementation for a tile within a tileset.
class Zone66TileImage: virtual public Image {
	protected:
		iostream_sptr data;
		FN_TRUNCATE fnTruncate;
		PaletteTablePtr pal;
		int width, height;

	public:
		Zone66TileImage(iostream_sptr data, FN_TRUNCATE fnTruncate,
			PaletteTablePtr pal)
			throw ();

		virtual ~Zone66TileImage()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setDimensions(unsigned int width, unsigned int height)
			throw (std::ios::failure);

		virtual StdImageDataPtr toStandard()
			throw (std::ios::failure);

		virtual StdImageDataPtr toStandardMask()
			throw ();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw (std::ios::failure);

		virtual PaletteTablePtr getPalette()
			throw ();

		virtual void setPalette(PaletteTablePtr newPalette)
			throw (std::ios::failure);

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_ZONE66_TILE_HPP_
