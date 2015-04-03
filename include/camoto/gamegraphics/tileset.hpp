/**
 * @file  camoto/gamegraphics/tileset.hpp
 * @brief Declaration of top-level Tileset class, for accessing files
 *        storing game images.
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

#include <vector>
#include <map>

#include <camoto/error.hpp>
#include <camoto/gamearchive/archive.hpp>
#include <camoto/gamegraphics/palette.hpp>
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
class Tileset: virtual public gamearchive::Archive
{
	public:
		/// Format capabilities.
		enum class Caps {
			/// No special capabilities.
			Default           = 0,

			/// Set if all tiles are the same size and this can be changed.
			ChangeDimensions  = 1 << 0,

			/// Set if palette() returns valid data.
			HasPalette        = 1 << 1,

			/// Set if palette() can be used.  Must be used with HasPalette.
			SetPalette        = 1 << 2,

			/// Tiles have names
			HasNames          = 1 << 3,
		};

		Tileset();
		virtual ~Tileset();

		/// Get the capabilities of this tileset format.
		/**
		 * @return One or more of the \ref Caps enum values (OR'd together.)
		 */
		virtual Caps caps() const = 0;

		/// Get the number of colours that can be stored in this tileset's images.
		/**
		 * @return One \ref ColourDepth value.
		 */
		virtual ColourDepth colourDepth() const = 0;

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
		virtual Point dimensions() const = 0;

		/// Set the size of all images in this tileset.
		/**
		 * Change the size of all the tiles in this tileset.  This will corrupt
		 * all the image data, so if the images are to remain after the resize it
		 * is the responsibility of the caller to save, convert and then restore
		 * all the image data.
		 *
		 * @pre caps() return value includes ChangeTilesetDimensions.
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
		virtual void dimensions(const Point& newDimensions);

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
		virtual unsigned int layoutWidth() const = 0;

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
		virtual std::unique_ptr<Image> openImage(FileHandle& id) = 0;

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
		virtual std::shared_ptr<Tileset> openTileset(FileHandle& id) = 0;

		/// Add a tile.
		/**
		 * Dimensions are standard for the tileset or 0x0, and can possibly be
		 * changed after the tile is opened.
		 */
		virtual FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);

		/// Get the indexed colour map from the image.
		/**
		 * @pre caps() return value includes HasPalette.
		 */
		virtual std::shared_ptr<const Palette> palette() const;

		/// Change the indexed colour map used by the image.
		/**
		 * This doesn't modify any pixels, only the colours those pixels map to.
		 *
		 * @pre caps() return value includes ChangePalette.
		 *
		 * @param newPalette
		 *   New palette data
		 */
		virtual void palette(std::shared_ptr<const Palette> newPalette);

	private:
		std::shared_ptr<const Palette> pal; ///< Palette storage, may be null
};

inline Tileset::Caps operator| (Tileset::Caps a, Tileset::Caps b) {
	return static_cast<Tileset::Caps>(
		static_cast<unsigned int>(a) | static_cast<unsigned int>(b)
	);
}

inline bool operator& (Tileset::Caps a, Tileset::Caps b) {
	return
		static_cast<unsigned int>(a) & static_cast<unsigned int>(b)
	;
}

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
std::unique_ptr<Tileset> DLL_EXPORT createTilesetFromList(
	const TileList& tileList, std::shared_ptr<Image> img,
	unsigned int layoutWidth);
// Defined in tilesetFromList.cpp

/// Element in list of images composing a tileset
struct TilesetFromImages_Item {
	std::string name;                 ///< Name of this tile
	bool isImage;                     ///< true=image, false=tileset
	std::shared_ptr<Image> image;     ///< valid if isImage is true
	std::shared_ptr<Tileset> tileset; ///< valid if isImage is false
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
std::unique_ptr<Tileset> DLL_EXPORT createTilesetFromImages(
	TilesetFromImages_List& content, unsigned int layoutWidth);
// Defined in tilesetFromImages.cpp

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_TILESET_HPP_
