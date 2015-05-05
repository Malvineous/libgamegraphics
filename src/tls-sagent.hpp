/**
 * @file  tls-sagent.hpp
 * @brief Secret Agent encryption wrapper around Crystal Caves tileset handler
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

#ifndef _CAMOTO_TLS_SAGENT_HPP_
#define _CAMOTO_TLS_SAGENT_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tls-ccaves-main.hpp"

namespace camoto {
namespace gamegraphics {

class TilesetType_SAgent2k: virtual public TilesetType_CCavesMain
{
	public:
		TilesetType_SAgent2k();
		virtual ~TilesetType_SAgent2k();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual std::vector<std::string> fileExtensions() const;
		virtual std::vector<std::string> games() const;
};

class TilesetType_SAgent8k: virtual public TilesetType_CCavesMain
{
	public:
		TilesetType_SAgent8k();
		virtual ~TilesetType_SAgent8k();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual std::vector<std::string> fileExtensions() const;
		virtual std::vector<std::string> games() const;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_SAGENT_HPP_
