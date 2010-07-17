/**
 * @file   graphicstype.hpp
 * @brief  GraphicsType class, used to identify and open an instance of a
 *         particular graphics format.
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

#ifndef _CAMOTO_GAMEGRAPHICS_GRAPHICSTYPE_HPP_
#define _CAMOTO_GAMEGRAPHICS_GRAPHICSTYPE_HPP_

#include <vector>
#include <map>

#include <camoto/types.hpp>
#include <camoto/gamegraphics/graphics.hpp>

/// Main namespace
namespace camoto {
/// Namespace for this library
namespace gamegraphics {

/// Confidence level when guessing an graphics format.
enum E_CERTAINTY {
	/// Certain this file is not of the graphics format.
	EC_DEFINITELY_NO,
	/// The checks were inconclusive, it could go either way.
	EC_UNSURE,
	/// Everything checked out OK, but this format has no signature or other
	/// test to be completely certain.
	EC_POSSIBLY_YES,
	/// This format has a signature and it matched.
	EC_DEFINITELY_YES,
};

/// Type of supplemental file.
enum E_SUPPTYPE {
	/// FAT is stored externally
	EST_FAT,
	/// Compression dictionary is external
	EST_DICT,
};

/// Supplementary item for an graphics.
/**
 * This class contains data about a supplementary item required to open an
 * graphics file.
 *
 * @see GraphicsType::getRequiredSupps()
 */
struct SuppItem {
	/// The stream containing the supplemental data.
	iostream_sptr stream;
	/// The truncate callback (required)
	FN_TRUNCATE fnTruncate;
};

/// A list of required supplemental files and their filenames.
typedef std::map<E_SUPPTYPE, std::string> MP_SUPPLIST;

/// A list of the supplemental file types mapped to open file streams.
typedef std::map<E_SUPPTYPE, SuppItem> MP_SUPPDATA;

/// Interface to a particular graphics format.
class GraphicsType {

	public:

		/// Get a short code to identify this file format, e.g. "img-xargon"
		/**
		 * This can be useful for command-line arguments.
		 *
		 * @return The graphics short name/ID.
		 */
		virtual std::string getCode() const
			throw () = 0;

		/// Get the graphics name, e.g. "Xargon graphics file"
		/**
		 * @return The graphics name.
		 */
		virtual std::string getFriendlyName() const
			throw () = 0;

		/// Get a list of the known file extensions for this format.
		/**
		 * @return A vector of file extensions, e.g. "xr1", "xr2".  Can be an
		 *         empty vector (if the graphics are normally stored in an archive
		 *         file, and the archive file doesn't support filenames.)  The
		 *         file extensions may also be generic, other data may be found in
		 *         files with these extensions too!
		 */
		virtual std::vector<std::string> getFileExtensions() const
			throw () = 0;

		/// Get a list of games using this format.
		/**
		 * @return A vector of game names, such as "Crystal Caves",
		 *         "Secret Agent"
		 */
		virtual std::vector<std::string> getGameList() const
			throw () = 0;

		/// Check a stream to see if it's in this graphics format.
		/**
		 * @param  psGraphics A C++ iostream of the file to test.
		 * @return A single confidence value from \ref E_CERTAINTY.
		 * @note   Many graphics formats lack a file header, so EC_UNSURE will be
		 *         a common return value, especially with small files.
		 */
		virtual E_CERTAINTY isInstance(iostream_sptr psGraphics) const
			throw (std::ios::failure) = 0;

		/// Create a blank graphics file in this format.
		/**
		 * This function writes out the necessary signatures and headers to create
		 * a valid blank graphics file in this format.
		 *
		 * Note to format implementors: This function only needs to be overridden
		 * if there are headers to write, otherwise an empty stream is passed to
		 * open() which is expected to succeed.
		 *
		 * @param  psGraphics A blank stream to store the new graphics in.
		 * @param  suppData Any supplemental data required by this format (see
		 *         getRequiredSupps()).
		 * @return A pointer to an instance of the Graphics class, just as if a
		 *         valid empty file had been opened by open().
		 */
		virtual GraphicsPtr create(iostream_sptr psGraphics, MP_SUPPDATA& suppData) const
			throw (std::ios::failure) = 0;

		/// Open a graphics file.
		/**
		 * @pre    Recommended that isInstance() has returned > EC_DEFINITELY_NO.
		 * @param  psGraphics The graphics file.
		 * @param  suppData Any supplemental data required by this format (see
		 *         getRequiredSupps()).
		 * @return A pointer to an instance of the Graphics class.  Will throw an
		 *         exception if the data is invalid (i.e. if isInstance() returned
		 *         EC_DEFINITELY_NO) however it will try its best to read the data
		 *         anyway, to make it possible to "force" a file to be opened by a
		 *         particular format handler.
		 */
		virtual GraphicsPtr open(iostream_sptr psGraphics, MP_SUPPDATA& suppData) const
			throw (std::ios::failure) = 0;

		/// Get a list of any required supplemental files.
		/**
		 * For some graphics formats, data is stored externally to the graphics file
		 * itself (for example the palette may be stored in a different file.)
		 * This function obtains a list of these supplementary files, so the caller
		 * can open them and pass them along to the graphics manipulation classes.
		 *
		 * @param  filenameGraphics The filename of the graphics (no path.)  This is
		 *         for supplemental files which share the same base name as the
		 *         graphics, but a different filename extension.
		 * @return A (possibly empty) map associating required supplemental file
		 *         types with their filenames.  For each returned value the file
		 *         should be opened and placed in a SuppItem instance.  The
		 *         SuppItem is then added to an \ref MP_SUPPDATA map where it can
		 *         be passed to newGraphics() or open().
		 */
		virtual MP_SUPPLIST getRequiredSupps(const std::string& filenameGraphics) const
			throw () = 0;

};

/// Shared pointer to an GraphicsType.
typedef boost::shared_ptr<GraphicsType> GraphicsTypePtr;

/// Vector of GraphicsType shared pointers.
typedef std::vector<GraphicsTypePtr> VC_GRAPHICSTYPE;

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_GRAPHICSTYPE_HPP_
