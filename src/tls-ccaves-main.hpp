/**
 * @file  tls-ccaves-main.hpp
 * @brief Crystal Caves top-level tileset handler (containing sub tilesets)
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

#ifndef _CAMOTO_TLS_CCAVES_MAIN_HPP_
#define _CAMOTO_TLS_CCAVES_MAIN_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "img-ega.hpp"
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class TilesetType_CCavesMain: virtual public TilesetType
{
	public:
		TilesetType_CCavesMain();
		virtual ~TilesetType_CCavesMain();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual std::vector<std::string> fileExtensions() const;
		virtual std::vector<std::string> games() const;
		virtual Certainty isInstance(stream::input& content) const;
		virtual std::shared_ptr<Tileset> create(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual std::shared_ptr<Tileset> open(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(stream::input& content,
			const std::string& filename) const;

	protected:
		/// How much to pad each tileset (in bytes)
		stream::len pad;
};

class Tileset_CCavesMain: virtual public Tileset_FAT
{
	public:
		Tileset_CCavesMain(std::unique_ptr<stream::inout> content,
			PlaneCount numPlanes, stream::len pad);
		virtual ~Tileset_CCavesMain();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;

		// Archive
		virtual void resize(FileHandle& id, stream::len newStoredSize,
			stream::len newRealSize);

		// Tileset_FAT
		virtual std::shared_ptr<Tileset> openTileset(FileHandle& id);
		virtual FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;

	protected:
		PlaneCount numPlanes;

		/// How much to pad each tileset (in bytes)
		stream::len pad;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_CCAVES_MAIN_HPP_
