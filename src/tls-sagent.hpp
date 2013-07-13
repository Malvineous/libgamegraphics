/**
 * @file   tls-sagent.hpp
 * @brief  Secret Agent encryption wrapper around Crystal Caves tileset handler
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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

#ifndef _CAMOTO_TLS_SAGENT_HPP_
#define _CAMOTO_TLS_SAGENT_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tls-ccaves-main.hpp"

namespace camoto {
namespace gamegraphics {

class SAgent2kTilesetType: virtual public CCavesMainTilesetType
{
	public:
		SAgent2kTilesetType();
		virtual ~SAgent2kTilesetType();

		virtual std::string getCode() const;
		virtual std::string getFriendlyName() const;
		virtual std::vector<std::string> getFileExtensions() const;
		virtual std::vector<std::string> getGameList() const;
		virtual Certainty isInstance(stream::input_sptr fsGraphics) const;
		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const;
		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const;
};

class SAgent8kTilesetType: virtual public CCavesMainTilesetType
{
	public:
		SAgent8kTilesetType();
		virtual ~SAgent8kTilesetType();

		virtual std::string getCode() const;
		virtual std::string getFriendlyName() const;
		virtual std::vector<std::string> getFileExtensions() const;
		virtual std::vector<std::string> getGameList() const;
		virtual Certainty isInstance(stream::input_sptr fsGraphics) const;
		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const;
		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_SAGENT_HPP_
