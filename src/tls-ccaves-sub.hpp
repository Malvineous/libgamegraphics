/**
 * @file  tls-ccaves-sub.hpp
 * @brief Crystal Caves sub tileset handler (containing images)
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

#ifndef _CAMOTO_TLS_CCAVES_SUB_HPP_
#define _CAMOTO_TLS_CCAVES_SUB_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"
#include "tileset-fat-fixed_tile_size.hpp"
#include "img-ega.hpp"

namespace camoto {
namespace gamegraphics {

class TilesetType_CCavesSub: virtual public TilesetType
{
	public:
		TilesetType_CCavesSub();
		virtual ~TilesetType_CCavesSub();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual std::vector<std::string> fileExtensions() const;
		virtual std::vector<std::string> games() const;
		virtual Certainty isInstance(stream::input& content) const;
		virtual std::shared_ptr<Tileset> create(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual std::shared_ptr<Tileset> open(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameGraphics)
			const;
};

class Tileset_CCavesSub:
	virtual public Tileset_FAT,
	virtual public Tileset_FAT_FixedTileSize
{
	public:
		Tileset_CCavesSub(std::unique_ptr<stream::inout> content,
			PlaneCount numPlanes);
		virtual ~Tileset_CCavesSub();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual void dimensions(const Point& newDimensions);
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(FileHandle& id);
		virtual FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;
		virtual void preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void postRemoveFile(const FATEntry *pid);

	protected:
		Point dims;
		PlaneCount numPlanes;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_CCAVES_SUB_HPP_
