/**
 * @file   graphics.hpp
 * @brief  Declaration of top-level Graphics class, for accessing files
 *         storing game images.
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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

#ifndef _CAMOTO_GAMEGRAPHICS_GRAPHICS_HPP_
#define _CAMOTO_GAMEGRAPHICS_GRAPHICS_HPP_

#include <boost/shared_ptr.hpp>
#include <exception>
#include <iostream>
#include <sstream>
#include <vector>

#include <camoto/types.hpp>
#include <camoto/gamegraphics/imageconverter.hpp>

namespace camoto {
namespace gamegraphics {

/// Metadata item types.
enum E_METADATA {
	EM_FILENAME_PALETTE, ///< Palette filename
};

/// Vector of metadata item types.
typedef std::vector<E_METADATA> VC_METADATA_ITEMS;

/// Generic "file not found" exception.
class ENotFound: public std::exception {
};

/// Generic "invalid graphics format" exception.
class EInvalidFormat: public std::exception {
};

class Graphics;

/// Shared pointer to a Graphics file.
typedef boost::shared_ptr<Graphics> GraphicsPtr;

/// Vector of Graphics shared pointers.
typedef std::vector<GraphicsPtr> VC_GRAPHICS;

/// Primary interface to a graphics file.
/**
 * This class represents a graphics file.  Its functions are used to manipulate
 * the contents of the graphics.
 *
 * A graphics file (also referred to as a tileset) is made up of zero or more
 * images (tiles), alongside zero or more sub-graphics files (tilesets within
 * tilesets.)  This can be thought of as files and folders, where the files are
 * the images/tiles and the folders are the tilesets (or "groups of images.")
 *
 * This structure allows the representation of many types of tileset formats:
 *
 *  * Single image (main tileset with one image and no sub-tilesets)
 *  * Animated image (main tileset with timing properties on it, and a number
 *    of images, again no sub-tilesets)
 *  * Collection of map tilesets (main tileset with no images, but multiple
 *    sub-tilesets.  Each sub-tileset contains a number of images but no
 *    further sub-tilesets.)
 *  * Collection of enemy animations (main tileset with no images and multiple
 *    sub-tilesets - one for each enemy.  Each sub-tileset contains timing
 *    parameters to make it an animation, as well as the images for the
 *    animation.)
 *
 * While perhaps a little verbose for simple situations, this structure allows
 * a number of different tileset variations to be stored uniformly.
 *
 * @note Multithreading: Only call one function in this class at a time.  Many
 *       of the functions seek around the underlying stream and thus will break
 *       if two or more functions are executing at the same time.
 */
class Graphics {

	public:

		enum Caps {
			ChangeTileCount = 0x01, ///< Can the tile count be changed?
			ChangeTileSize  = 0x02, ///< Can the tile/image size be changed?
			ModifyTilesets  = 0x04, ///< Can tilesets be added or removed?
		};

		/// Truncate callback
		/**
		 * This function is called with a single unsigned long parameter when
		 * the underlying graphics file needs to be shrunk or enlarged to the given
		 * size.  It must be set to a valid function before flush() is called.
		 *
		 * The function signature is:
		 * @code
		 * void fnTruncate(unsigned long newLength);
		 * @endcode
		 *
		 * This example uses boost::bind to package up a call to the Linux
		 * truncate() function (which requires both a filename and size) such that
		 * the filename is supplied in advance and not required when flush() makes
		 * the call.
		 *
		 * @code
		 * Graphics *pGraphics = ...
		 * pGraphics->fnTruncate = boost::bind<void>(truncate, "graphics.dat", _1);
		 * pGraphics->flush();  // calls truncate("graphics.dat", 123)
		 * @endcode
		 *
		 * Unfortunately since there is no cross-platform method for changing a
		 * file's size from an open file handle, this is a necessary evil to avoid
		 * passing the graphics filename around all over the place.
		 */
		FN_TRUNCATE fnTruncate;

		Graphics()
			throw ();

		virtual ~Graphics()
			throw ();

		/// Get the capabilities of this tileset format.
		/**
		 * @return One or more of the Caps enum values (OR'd together.)
		 */
		virtual int getCaps()
			throw () = 0;

		/// Get the number of sub-tilesets within this tileset.
		/**
		 * @note   To implementors: This function defaults to returning zero, so
		 *         only needs to be overridden when there are sub-tilesets
		 *         available.
		 *
		 * @return Number of sub-tilesets within this tileset.
		 */
		virtual int getTilesetCount()
			throw ();

		/// Open the sub-tileset at the given offset.
		/**
		 * @note   To implementors: This function defaults to triggering an
		 *         assertion failure, so like getTilesetCount(), only needs to be
		 *         overridden if there are sub-tilesets available.
		 *
		 * @param  index The zero-based index of the tileset to open.  Must be
		 *         less than the value returned by getTilesetCount().
		 *
		 * @return A shared pointer to another Graphics class instance.
		 */
		virtual GraphicsPtr getTileset(int index)
			throw ();

		/// Insert sub-tileset at the given offset.
		/**
		 * @note   To implementors: This function defaults to triggering an
		 *         assertion failure, so only needs to be overridden if there
		 *         are sub-tilesets available.
		 *
		 * @precondition  getCaps() return value includes ModifyTilesets.
		 *
		 * @param  insertBefore  The zero-based index of a tileset which will
		 *         follow the newly inserted tileset.  A value of zero means
		 *         append to the end of the file.  Must be less than the value
		 *         returned by getTilesetCount().
		 *
		 * @return A shared pointer to the new (empty) Graphics class instance,
		 *         with 0x0 tiles if getCaps() includes ChangeTileSize, or the
		 *         only valid (unchangeable) tile size otherwise.
		 */
		//virtual GraphicsPtr insertTileset(int insertBefore)
		//	throw ();

		/// Get the dimensions of all images in this tileset.
		/**
		 * @param  width Image width in pixels, or zero if there are no tiles.
		 *
		 * @param  height Image height in pixels, or zero if there are no tiles.
		 *
		 * @return Image dimensions are stored in the two parameters.
		 */
		virtual void getTileSize(unsigned int *width, unsigned int *height)
			throw () = 0;

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
		virtual unsigned int getLayoutWidth()
			throw () = 0;

		/// Set the size of all images in this tileset.
		/**
		 * Change the size of all the tiles in this tileset.  This will corrupt
		 * all the image data, so if the images are to remain after the resize it
		 * is the responsibility of the caller to save, convert and then restore
		 * all the image data.
		 *
		 * @param  width New width
		 * @param  height New height
		 * @precondition  getCaps() return value includes ChangeTileSize.
		 * @postcondition The image data is not updated to reflect the change in
		 *         size, so after this function is called all the images in the
		 *         tileset (if any) will be undefined (i.e. random data).
		 */
		virtual void setTileSize(unsigned int width, unsigned int height)
			throw (std::ios::failure) = 0;

		/// Get the number of images in this tileset.
		/**
		 * @return Number of images in the tileset.
		 */
		virtual int getImageCount()
			throw () = 0;

		/// Set the number of images in this tileset.
		/**
		 * Change the number of images in the tileset to the given value.  If
		 * this value is larger than the current tile count, the content of the
		 * new tiles is undefined (i.e. random data.)  If the value is smaller,
		 * the tileset will be cropped and images with the largest index will be
		 * lost.
		 *
		 * @precondition  getCaps() return value includes ChangeTileCount.
		 *
		 * @param  count The new number of images.
		 */
		virtual void setImageCount(int count)
			throw (std::ios::failure) = 0;

		/// Export the given tile's data
		/**
		 * @param  index The zero-based index of the tile to access.  Must be
		 *         less than the value returned by getTileCount().
		 * @return A shared pointer to an instance of the Image class.
		 */
		virtual ImageConverterPtr openImage(int index)
			throw () = 0;

		/// Write out any cached changes to the underlying stream.
		/**
		 * Some functions write their changes to the graphics file immediately,
		 * while others cache their changes for performance reasons.  Any cached
		 * changes are NOT automatically written out when the class is destroyed
		 * (as there would be no way to handle any write failures), so this
		 * function must be called before the class is destroyed or the graphics
		 * file will become corrupted.
		 *
		 * This function can also be called at any time to write all pending
		 * changes without destroying the class.  However some changes can involve
		 * shuffling around many hundreds of megabytes of data, so don't call this
		 * function unless you have good reason to!
		 *
		 * @pre    The \ref fnTruncate member must be valid before this call.
		 */
		//virtual void flush()
		//	throw (std::ios::failure) = 0;

		// The metadata functions all have no-op defaults, they only need to be
		// overridden for graphics formats that have metadata.

		/// Get a list of supported metadata elements that can be set.
		/**
		 * Some graphics formats have room for additional data, such as a palette
		 * filename.  This function is used to obtain a list of the
		 * metadata elements supported by the current graphics.  Not every graphics
		 * supports all the metadata types, and any optional elements will be
		 * included in this list (but getMetadata() may return an empty string for
		 * those.)
		 *
		 * Note to graphics format implementors: There is a default implementation
		 * of this function which returns an empty vector.  Thus this only needs
		 * to be overridden if the graphics format does actually support metadata.
		 *
		 * @return std::vector of \ref E_METADATA items.
		 */
		virtual VC_METADATA_ITEMS getMetadataList() const
			throw ();

		/// Get the value of a metadata element.
		/**
		 * Returns the value of a metadata item reported to exist by
		 * getMetadataList().
		 *
		 * Note to graphics format implementors: There is a default implementation
		 * of this function which always throws an exception.  Thus this only needs
		 * to be overridden if the graphics format does actually support metadata.
		 *
		 * @param  item Item to retrieve.  Must have been included in the list
		 *         returned by getMetadataList().
		 * @return A string containing the metadata (may be empty.)
		 */
		virtual std::string getMetadata(E_METADATA item) const
			throw (std::ios::failure);

		/// Change the value of a metadata element.
		/**
		 * Only elements returned by getMetadataList() can be changed.
		 *
		 * Note to graphics format implementors: There is a default implementation
		 * of this function which always throws an exception.  Thus this only needs
		 * to be overridden if the graphics format does actually support metadata.
		 *
		 * @param  item Item to set.  Must have been included in the list returned
		 *         by getMetadataList().
		 * @param  value The value to set.  Passing an empty string will remove
		 *         the metadata element if possible, otherwise it will be set to
		 *         a blank.
		 */
		virtual void setMetadata(E_METADATA item, const std::string& value)
			throw (std::ios::failure);

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_GRAPHICS_HPP_
