/**
 * @file   suppitem.hpp
 * @brief  Objects related to supplemental items.
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

#ifndef _CAMOTO_GAMEGRAPHICS_SUPPITEM_HPP_
#define _CAMOTO_GAMEGRAPHICS_SUPPITEM_HPP_

#include <vector>
#include <map>

#include <camoto/types.hpp>
#include <camoto/gamegraphics/tileset.hpp>

/// Main namespace
namespace camoto {
/// Namespace for this library
namespace gamegraphics {

/// Supplementary item.
/**
 * This class contains data about a supplementary item required to open a
 * particular type of file.
 *
 * @see TilesetType::getRequiredSupps(), ImageType::getRequiredSupps()
 */
struct SuppItem {

	/// Type of supplemental file.
	enum Type {
		FAT,         ///< FAT is stored externally
		Dictionary,  ///< Compression dictionary is external
		Palette,     ///< Palette data
	};

	/// The stream containing the supplemental data.
	iostream_sptr stream;

	/// The truncate callback (required)
	FN_TRUNCATE fnTruncate;

};

/// A list of required supplemental files and their filenames.
typedef std::map<SuppItem::Type, std::string> MP_SUPPLIST;

/// A list of the supplemental file types mapped to open file streams.
typedef std::map<SuppItem::Type, SuppItem> MP_SUPPDATA;

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_SUPPITEM_HPP_
