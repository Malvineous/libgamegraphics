/**
 * @file  tls-harry-chr.hpp
 * @brief Halloween Harry .CHR tileset handler.
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

#ifndef _CAMOTO_TLS_HARRY_CHR_HPP_
#define _CAMOTO_TLS_HARRY_CHR_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"
#include "tileset-fat-fixed_tile_size.hpp"

namespace camoto {
namespace gamegraphics {

class TilesetType_HarryCHR: virtual public TilesetType
{
	public:
		TilesetType_HarryCHR();
		virtual ~TilesetType_HarryCHR();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual std::vector<std::string> fileExtensions() const;
		virtual std::vector<std::string> games() const;
		virtual Certainty isInstance(stream::input& content) const;
		virtual std::shared_ptr<Tileset> create(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual std::shared_ptr<Tileset> open(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameTileset)
			const;
};

class Tileset_HarryCHR:
	virtual public Tileset_FAT,
	virtual public Tileset_FAT_FixedTileSize
{
	public:
		Tileset_HarryCHR(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_HarryCHR();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(FileHandle& id);
		virtual FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;
};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_HARRY_CHR_HPP_
