/**
 * @file   tls-harry-hsb.hpp
 * @brief  Halloween Harry .HSB tileset handler.
 *
 * Copyright (C) 2010-2012 Adam Nielsen <malvineous@shikadi.net>
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

#ifndef _CAMOTO_TLS_HARRY_HSB_HPP_
#define _CAMOTO_TLS_HARRY_HSB_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "pal-vga-raw.hpp"
#include "img-vga-raw.hpp"
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

/// Halloween Harry .HSB tileset type
class HarryHSBTilesetType: virtual public TilesetType
{
	public:
		HarryHSBTilesetType()
			throw ();

		virtual ~HarryHSBTilesetType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(stream::input_sptr fsTileset) const
			throw (stream::error);

		virtual TilesetPtr create(stream::inout_sptr psTileset,
			SuppData& suppData) const
			throw (stream::error);

		virtual TilesetPtr open(stream::inout_sptr fsTileset,
			SuppData& suppData) const
			throw (stream::error);

		virtual SuppFilenames getRequiredSupps(const std::string& filenameTileset) const
			throw ();
};

/// Image inside Halloween Harry .HSB tileset
class HarryHSBImage: virtual public VGAImage
{
	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param data
		 *   VGA image data.
		 *
		 * @param pal
		 *   Image palette.
		 */
		HarryHSBImage(stream::inout_sptr data, PaletteTablePtr pal)
			throw (stream::error);

		virtual ~HarryHSBImage()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setDimensions(unsigned int width, unsigned int height)
			throw (stream::error);

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw ();

		virtual PaletteTablePtr getPalette()
			throw (stream::error);

	protected:
		uint16_t width;
		uint16_t height;
		PaletteTablePtr pal;
};

/// Halloween Harry .HSB tileset
class HarryHSBTileset: virtual public FATTileset
{
	public:
		HarryHSBTileset(stream::inout_sptr data,
			PaletteTablePtr pal)
			throw (stream::error);

		virtual ~HarryHSBTileset()
			throw ();

		virtual int getCaps()
			throw ();

		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content)
			throw (stream::error);

		PaletteTablePtr getPalette()
			throw ();

		void setPalette(PaletteTablePtr newPalette)
			throw (stream::error);

	protected:
		PaletteTablePtr pal;
};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_HARRY_HPP_
