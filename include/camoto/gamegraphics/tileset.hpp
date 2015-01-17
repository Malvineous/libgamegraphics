/**
 * @file   tileset.hpp
 * @brief  Declaration of top-level Tileset class, for accessing files
 *         storing game images.
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

#ifndef _CAMOTO_GAMEGRAPHICS_TILESET_HPP_
#define _CAMOTO_GAMEGRAPHICS_TILESET_HPP_

#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

#include <camoto/stream.hpp>
#include <camoto/error.hpp>
#include <camoto/metadata.hpp>
#include <camoto/gamegraphics/palettetable.hpp>
#include <camoto/gamegraphics/image.hpp>

#ifndef DLL_EXPORT
#define DLL_EXPORT
#endif

namespace camoto {
namespace gamegraphics {

/// Generic "file not found" exception.
class ENotFound: public error {
};

/// Generic "invalid tileset format" exception.
class EInvalidFormat: public error {
};

class Tileset;

/// Shared pointer to a Tileset file.
typedef boost::shared_ptr<Tileset> TilesetPtr;

/// Primary interface to a tileset file.
/**
 * This class represents a tileset file.  Its functions are used to manipulate
 * the contents of the tileset.
 *
 * A tileset file is made up of zero or more entries.  Each entry can be either
 * another tileset (like a subdirectory) or an image (like a file.)
 *
 * @note Multithreading: Only call one function in this class at a time.  Many
 *       of the functions seek around the underlying stream and thus will break
 *       if two or more functions are executing at the same time.
 */
class Tileset: virtual public Metadata
{
	public:
		/// File attribute flags.  Can be OR'd together.
		enum Attributes {
			Default       = 0x00,  ///< Default/none
			EmptySlot     = 0x01,  ///< There's currently no image at this location
			SubTileset    = 0x80,  ///< This entry contains a tileset/animation
		};

		/// Format capabilities.
		enum Caps {
			/// Set if all tiles are the same size and this can be changed.
			ChangeDimensions  = 0x01,

			/// Set if getPalette() returns valid data.
			HasPalette        = 0x02,

			/// Set if setPalette() can be used.  Must be used with HasPalette.
			CanSetPalette     = 0x04,

			/// Tiles have names
			HasNames          = 0x08,

			/// Set if the image is 8bpp (256 colour)
			ColourDepthVGA    = 0x00,

			/// Set if the image is 4bpp (16 colour)
			ColourDepthEGA    = 0x10,

			/// Set if the image is 2bpp (4 colour)
			ColourDepthCGA    = 0x20,

			/// Set if the image is 1bpp (black and white)
			ColourDepthMono   = 0x30,

			/// Mask to isolate the ColourDepth value.  This must always be used when
			/// checking the colour depth.
			ColourDepthMask   = 0x30,
		};

		/// Base class to represent entries in a tileset.
		/**
		 * Will be extended by descendent classes to hold format-specific data.
		 * The entries here will be valid for all tileset types.
		 */
		struct Entry {
			/// Name of this tile, if Attributes::HasNames is set.
			virtual std::string getName() const = 0;

			/// Is this entry valid? (set to false upon delete)
			virtual bool isValid() const = 0;

			/// One or more %Attributes flags
			virtual int getAttr() const = 0;
		};

		/// Shared pointer to a FileEntry
		typedef boost::shared_ptr<Entry> EntryPtr;

		/// Vector of shared FileEntry pointers
		typedef std::vector<EntryPtr> VC_ENTRYPTR;

		/// Get the capabilities of this tileset format.
		/**
		 * @return One or more of the \ref Caps enum values (OR'd together.)
		 */
		virtual int getCaps() = 0;

		/// Get a list of all tilesets and images in the file.
		/**
		 * @return A vector of %Entry with one element for each item in the tileset.
		 */
		virtual const VC_ENTRYPTR& getItems() const = 0;

		/// Open the sub-tileset at the given offset.
		/**
		 * @note If you make changes to the returned Tileset you will (of course)
		 *   need to call flush() on it to save the changes.  However then you must
		 *   call flush() on *this* tileset to fully commit the changes to the
		 *   underlying file.
		 *
		 * @note To implementors: This function defaults to triggering an
		 *   assertion failure, so this function only needs to be overridden if
		 *   there are sub-tilesets available.
		 *
		 * @param id
		 *   ID of the item to open, as returned by getItems().
		 *
		 * @return A shared pointer to another Tileset instance.
		 */
		virtual TilesetPtr openTileset(const EntryPtr& id) = 0;

		/// Export/import the given tile.
		/**
		 * @note To implementors: This function defaults to triggering an
		 *   assertion failure, so this function only needs to be overridden if
		 *   there are sub-images available.
		 *
		 * @param id
		 *   ID of the item to open, as returned by getItems().
		 *
		 * @return A shared pointer to an instance of the Image class.
		 */
		virtual ImagePtr openImage(const EntryPtr& id) = 0;

		/// Insert a new image/subtileset into the tileset.
		/**
		 * It will be inserted before idBeforeThis, or at the end of the tileset if
		 * idBeforeThis is not valid.
		 *
		 * @note For performance reasons, this operation is cached so it does not
		 *   immediately affect the tileset file.  When the time comes to
		 *   flush() the changes, all the insert/delete/resize operations are
		 *   done in a single pass.  However providing this class is the sole
		 *   method of accessing the underlying file, this is of no concern.
		 *
		 * @post Existing EntryPtrs become invalid.  Any open images remain valid.
		 *
		 * @param idBeforeThis
		 *   The new image will be inserted before this one.  If it is not valid,
		 *   the new image will be appended onto the end of the tileset.
		 *
		 * @param attr
		 *   File attributes (one or more %Attributes)
		 *
		 * @return An EntryPtr to the newly added image, which can be immediately
		 *   passed to open() if needed.
		 */
		virtual EntryPtr insert(const EntryPtr& idBeforeThis, int attr) = 0;

		/// Delete the given entry from the tileset.
		/**
		 * @note For performance reasons, this operation is cached so it does not
		 *   immediately affect the archive file.  When the time comes to flush()
		 *   the changes, all the insert/delete/resize operations are done in a
		 *   single pass.  However providing this class is the sole method of
		 *   accessing the archive file, this is of no concern.
		 *
		 * @param id
		 *   ID of the item to delete, as returned by getItems().
		 *
		 * @post Existing EntryPtrs become invalid.  Any open files remain valid.
		 */
		virtual void remove(EntryPtr& id) = 0;

		/// Enlarge or shrink an existing entry.
		/**
		 * @note For performance reasons, this operation is cached so it does not
		 *   immediately affect the archive file.  When the time comes to flush()
		 *   the changes, all the insert/delete/resize operations are done in a
		 *   single pass.  However providing this class is the sole method of
		 *   accessing the archive file, this is of no concern.
		 *
		 * @param id
		 *   ID of the item to resize, as returned by getItems().
		 *
		 * @param newSize
		 *   Item's new size.  If this is smaller than the current size the excess
		 *   data is lost, if it is larger than the current size the new data is
		 *   undefined/random (whatever was there from before.)
		 *
		 * @post Existing EntryPtrs remain valid.
		 */
		virtual void resize(EntryPtr& id, stream::len newSize) = 0;

		/// Write out any cached changes to the underlying stream.
		/**
		 * Some functions write their changes to the tileset file immediately,
		 * while others cache their changes for performance reasons.  Any cached
		 * changes are NOT automatically written out when the class is destroyed
		 * (as there would be no way to handle any write failures), so this
		 * function must be called before the class is destroyed or the tileset
		 * file will become corrupted.
		 *
		 * This function can also be called at any time to write all pending
		 * changes without destroying the class.  However some changes can involve
		 * shuffling around a lot of of data, so don't call this
		 * function unless you have good reason to!
		 *
		 * @note When opening subtilesets, you must call flush() on each tileset
		 *   in the chain, from the most-sub-sub-tileset first, to the original
		 *   root tileset last.
		 */
		virtual void flush() = 0;

		/// Get the dimensions of all images in this tileset.
		/**
		 * @param width
		 *   Image width in pixels, or zero if the tileset format doesn't support
		 *   a set of dimensions that apply to all tiles contained within.
		 *
		 * @param height
		 *   Image height in pixels, or zero if the tileset format doesn't support
		 *   this.
		 *
		 * @return Image dimensions are stored in the two parameters.
		 *
		 * @note Default implementation returns 0 x 0.
		 */
		virtual void getTilesetDimensions(unsigned int *width,
			unsigned int *height) = 0;

		/// Set the size of all images in this tileset.
		/**
		 * Change the size of all the tiles in this tileset.  This will corrupt
		 * all the image data, so if the images are to remain after the resize it
		 * is the responsibility of the caller to save, convert and then restore
		 * all the image data.
		 *
		 * @pre getCaps() return value includes ChangeTilesetDimensions.
		 *
		 * @post The image data is not adjusted to reflect the change in size, so
		 *   after this function is called all the images in the tileset (if any)
		 *   will be undefined (i.e. random data).
		 *
		 * @param width
		 *   New width
		 *
		 * @param height
		 *   New height
		 *
		 * @note Default implementation triggers assertion failure.
		 */
		virtual void setTilesetDimensions(unsigned int width,
			unsigned int height) = 0;

		/// Get the preferred layout size for this tileset.
		/**
		 * Often small tiles (8x8) can be arranged in a grid to produce a larger
		 * image, which makes editing the tiles easier (as the purpose of each
		 * tile is much clearer.)  This function is used to obtain the dimensions
		 * of such a layout.  The width is returned, and the height can then be
		 * calculated from this and the number of tiles.
		 *
		 * @return Layout width as number of tiles, or zero for don't know.
		 */
		virtual unsigned int getLayoutWidth() = 0;

		/// Get the indexed colour map from the image.
		/**
		 * @pre getCaps() return value includes HasPalette.
		 */
		virtual PaletteTablePtr getPalette() = 0;

		/// Change the indexed colour map used by the image.
		/**
		 * This doesn't modify any pixels, only the colours those pixels map to.
		 *
		 * @pre getCaps() return value includes ChangePalette.
		 *
		 * @param newPalette
		 *   New palette data
		 */
		virtual void setPalette(PaletteTablePtr newPalette) = 0;

};

/// Information about the location of a tile within an image.
struct TilePos {
	unsigned int xOffset;
	unsigned int yOffset;
	unsigned int width;
	unsigned int height;
};
typedef std::vector<TilePos> TileList;

/// Create a tileset out of different rectangles in a single image
/**
 * @param tileList
 *   List of coordinates in the image to turn into tiles.
 *
 * @param img
 *   Image to use for the underlying picture data.
 *
 * @param layoutWidth
 *   The ideal width of the tileset, as a number of tiles, when all the tiles
 *   are displayed together in a block.
 */
TilesetPtr DLL_EXPORT createTilesetFromList(const TileList& tileList,
	ImagePtr img, unsigned int layoutWidth);
// Defined in tilesetFromList.cpp


/// Element in list of images composing a tileset
struct TilesetFromImages_Item {
	std::string name;   ///< Name of this tile
	bool isImage;       ///< true=image, false=tileset
	ImagePtr image;     ///< valid if isImage is true
	TilesetPtr tileset; ///< valid if isImage is false
};

/// List of elements used to construct a tileset
typedef std::vector<TilesetFromImages_Item> TilesetFromImages_List;

/// Create a tileset out of a list of images
/**
 * @param content
 *   Vector of images to include in the tileset.  The first element becomes the
 *   first entry in the tileset.  It can be an image or a sub-tileset.
 *
 * @param layoutWidth
 *   The ideal width of the tileset, as a number of tiles, when all the tiles
 *   are displayed together in a block.
 */
TilesetPtr DLL_EXPORT createTilesetFromImages(TilesetFromImages_List& content,
	unsigned int layoutWidth);
// Defined in tilesetFromImages.cpp

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_TILESET_HPP_
