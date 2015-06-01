/**
 * @file  tileset-from_image_list.hpp
 * @brief Create a tileset from a collection of other images.
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

#ifndef _CAMOTO_TILESET_FROM_IMAGE_LIST_HPP_
#define _CAMOTO_TILESET_FROM_IMAGE_LIST_HPP_

#include <camoto/gamegraphics/tileset.hpp>

namespace camoto {
namespace gamegraphics {

class Tileset_FromImageList: virtual public Tileset
{
	public:
		struct ImageListEntry: public File {
			/// Index into imageList
			unsigned int index;
			/// Index into possible split described by imageList[index]
			unsigned int subindex;
			/// Cached dimensions of tile within parent image, when
			/// imageList[index]->split != SingleImage
			Rect dims;

			/// Convert a FileHandle into an ImageListEntry pointer
			inline static ImageListEntry *cast(const Archive::FileHandle& id)
			{
				return dynamic_cast<ImageListEntry *>(
					const_cast<Archive::File*>(&*id)
				);
			}
		};

		struct Item {
			/// Image to source next tile(s) from.  All images must be the same colour
			/// depth.
			std::shared_ptr<Image> img;

			/// Sub-tileset to place here.  All images must be the same colour depth.
			std::shared_ptr<Tileset> tileset;

			/// How should this item be attached to the tileset being created?
			enum class AttachmentType {
				/// Add img onto the end of the tileset.
				Append,
				/// Add img or tileset as a sub-tileset.
				Child,
			} att;

			/// How should the image be split?
			enum class SplitType {
				/// Add the whole image as a single tile.  Tile is resizable if img is.
				/** If tileset is given instead of img, then whole tileset is added
				 */
				SingleTile,
				/// Split img into multiple tiles, all the same size.
				UniformTiles,
				/// Split img into multiple tiles, with each tile's origin and size
				/// given as a list.
				List,
			} split;

			/// Size of each tile, when split == UniformTiles.
			Point tileSize;

			/// Area containing tiles, if split == UniformTiles.
			/**
			 * x,y is the pixel of the first tile (typically 0,0)
			 * width,height is the width and height of the tile area.  Both must
			 * be a multiple of tileSize.x and tileSize.y respectively.
			 */
			Rect tileArea;

			/// Coordinates of each tile within img, if split == List.
			std::vector<Rect> coords;

			inline Item()
			{
			}

			inline Item(std::shared_ptr<Image> img, AttachmentType att,
				SplitType split, Point tileSize, Rect tileArea,
				std::vector<Rect> coords)
				:	img(img),
					att(att),
					split(split),
					tileSize(tileSize),
					tileArea(tileArea),
					coords(coords)
			{
			}

			protected:
				///< Do we need to call img->convert() on Tileset::flush()?
				bool imageChanged;

				// Image data is converted once for efficiency
				std::shared_ptr<Pixels> stdImg;  ///< Raw image data for img
				std::shared_ptr<Pixels> stdMask; ///< Raw image mask for img

				friend class Tileset_FromImageList;
		};
		// No palette because each added image will have one
		Tileset_FromImageList(std::vector<Item> imageList, unsigned int layoutWidth);
		virtual ~Tileset_FromImageList();

		virtual const Archive::FileVector& files() const;
		virtual FileHandle find(const std::string& strFilename) const;
		virtual bool isValid(const FileHandle& id) const;
		virtual std::unique_ptr<stream::inout> open(const FileHandle& id,
			bool useFilter);
		virtual std::shared_ptr<Archive> openFolder(const FileHandle& id);
		virtual FileHandle insert(const FileHandle& idBeforeThis,
			const std::string& strFilename, stream::len storedSize, std::string type,
			File::Attribute attr);
		virtual void remove(FileHandle& id);
		virtual void rename(FileHandle& id, const std::string& strNewName);
		virtual void move(const FileHandle& idBeforeThis, FileHandle& id);
		virtual void resize(FileHandle& id, stream::len newStoredSize,
			stream::len newRealSize);
		virtual void flush();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual unsigned int layoutWidth() const;
		virtual std::unique_ptr<Image> openImage(FileHandle& id);
		virtual std::shared_ptr<Tileset> openTileset(FileHandle& id);

	protected:
		std::vector<Item> imageList; ///< List of images and coordinates
		unsigned int prefLayoutWidth; ///< Value to return in layoutWidth()
		FileVector vcFAT;     ///< List of items

		void imageChanged();
};

inline std::shared_ptr<Tileset_FromImageList> make_Tileset_FromImageList(
	std::vector<Tileset_FromImageList::Item> imageList, unsigned int layoutWidth)
{
	return std::make_shared<Tileset_FromImageList>(imageList, layoutWidth);
}

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TILESET_FROM_IMAGE_LIST_HPP_
