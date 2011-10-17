/**
 * @file   pal-vga-raw.hpp
 * @brief  Palette interface to 768-byte raw 6-bit VGA palette files.
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

#ifndef _CAMOTO_GAMEGRAPHICS_PAL_VGA_RAW_HPP_
#define _CAMOTO_GAMEGRAPHICS_PAL_VGA_RAW_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "img-palette.hpp"

namespace camoto {
namespace gamegraphics {

/// Filetype handler for VGA palette files.
class VGAPaletteImageType: virtual public ImageType {

	public:

		VGAPaletteImageType()
			throw ();

		virtual ~VGAPaletteImageType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(stream::inout_sptr fsImage) const
			throw (stream::error);

		virtual ImagePtr create(stream::inout_sptr psImage,
			SuppData& suppData) const
			throw (stream::error);

		virtual ImagePtr open(stream::inout_sptr fsImage,
			SuppData& suppData) const
			throw (stream::error);

		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage) const
			throw ();

};

/// Palette interface to 768-byte raw 6-bit VGA palette files.
class VGAPalette: virtual public Palette {

	private:
		stream::inout_sptr data;

	public:
		VGAPalette(stream::inout_sptr data)
			throw (stream::error);

		virtual ~VGAPalette()
			throw ();

		virtual PaletteTablePtr getPalette()
			throw (stream::error);

		virtual void setPalette(PaletteTablePtr newPalette)
			throw (stream::error);
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_PAL_VGA_RAW_HPP_
