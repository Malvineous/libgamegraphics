/**
 * @file  tls-jill.hpp
 * @brief Jill of the Jungle tileset
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

#ifndef _CAMOTO_TLS_JILL_HPP_
#define _CAMOTO_TLS_JILL_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"
#include "img-vga.hpp"

namespace camoto {
namespace gamegraphics {

class TilesetType_Jill: virtual public TilesetType
{
	public:
		TilesetType_Jill();
		virtual ~TilesetType_Jill();

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
};

class Tileset_Jill: virtual public Tileset_FAT
{
	public:
		Tileset_Jill(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_Jill();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;
		virtual std::shared_ptr<const Palette> palette() const;

		// Tileset_FAT
		virtual std::shared_ptr<Tileset> openTileset(const FileHandle& id);
		virtual const FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;
		virtual void updateFileOffset(const FATEntry *pid, stream::len offDelta);
		virtual void updateFileSize(const FATEntry *pid, stream::len sizeDelta);

	protected:
		void loadPalette();

		bool loadedPal;
};

class Tileset_JillSub: virtual public Tileset_FAT
{
	public:
		Tileset_JillSub(std::unique_ptr<stream::inout> content,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_JillSub();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(const FileHandle& id);
		virtual void preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void postRemoveFile(const FATEntry *pid);

	protected:
		std::vector<uint8_t> colourMap;
};

/// Image implementation for a Jill of the Jungle tile.
class Image_Jill: virtual public Image
{
	public:
		typedef std::function<void()> fn_changed_t;
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param data
		 *   Image data, including width/height header.
		 *
		 * @param colourMap
		 *   Colour mapping table (not a palette) from the parent tileset.
		 */
		Image_Jill(/*std::shared_ptr<Tileset_JillSub> tileset, */const Point& dims,
			Pixels&& pix, Pixels&& mask, std::shared_ptr<const Palette> pal,
			fn_changed_t fnChanged);
		virtual ~Image_Jill();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual void dimensions(const Point& newDimensions);
		virtual Pixels convert() const;
		virtual Pixels convert_mask() const;
		virtual void convert(const Pixels& newContent,
			const Pixels& newMask);

	protected:
		Point dims;
		Pixels pix;
		Pixels mask;
		fn_changed_t fnChanged;
};


} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_JILL_HPP_
