/**
 * @file   img-tv-fog.hpp
 * @brief  Terminal Velocity fog mapping table.
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

#ifndef _CAMOTO_IMG_TV_FOG_HPP_
#define _CAMOTO_IMG_TV_FOG_HPP_

#include <camoto/gamegraphics/imagetype.hpp>

namespace camoto {
namespace gamegraphics {

/// Filetype handler for Terminal Velocity fog mapping files.
class TVFogImageType: virtual public ImageType {

	public:

		TVFogImageType()
			throw ();

		virtual ~TVFogImageType()
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
			SuppData& suppData) const
			throw (std::ios::failure);

		virtual ImagePtr open(iostream_sptr fsImage, FN_TRUNCATE fnTruncate,
			SuppData& suppData) const
			throw (std::ios::failure);

		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage) const
			throw ();

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_TV_FOG_HPP_
