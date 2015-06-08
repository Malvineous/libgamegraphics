/**
 * @file  tls-catacomb.hpp
 * @brief Catacomb/Catacomb II CGA and EGA tilesets
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

#ifndef _CAMOTO_TLS_CATACOMB_HPP_
#define _CAMOTO_TLS_CATACOMB_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>

namespace camoto {
namespace gamegraphics {

/// Common code for Catacomb tilesets.
class TilesetType_Catacomb: virtual public TilesetType
{
	public:
		TilesetType_Catacomb();
		virtual ~TilesetType_Catacomb();

		virtual std::vector<std::string> fileExtensions() const;
		virtual std::vector<std::string> games() const;
		virtual SuppFilenames getRequiredSupps(stream::input& content,
			const std::string& filename) const;
};

/// CGA-specific code for Catacomb tilesets.
class TilesetType_CatacombCGA: virtual public TilesetType_Catacomb
{
	public:
		TilesetType_CatacombCGA();
		virtual ~TilesetType_CatacombCGA();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual Certainty isInstance(stream::input& content) const;
		virtual std::shared_ptr<Tileset> create(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual std::shared_ptr<Tileset> open(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
};

/// EGA-specific code for Catacomb tilesets.
class TilesetType_CatacombEGA: virtual public TilesetType_Catacomb
{
	public:
		TilesetType_CatacombEGA();
		virtual ~TilesetType_CatacombEGA();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual Certainty isInstance(stream::input& content) const;
		virtual std::shared_ptr<Tileset> create(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual std::shared_ptr<Tileset> open(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_CATACOMB_HPP_
