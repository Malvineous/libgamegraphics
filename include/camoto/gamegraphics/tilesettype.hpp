/**
 * @file  camoto/gamegraphics/tilesettype.hpp
 * @brief TilesetType class, used to identify and open an instance of a
 *        particular tileset format.
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

#ifndef _CAMOTO_GAMEGRAPHICS_TILESETTYPE_HPP_
#define _CAMOTO_GAMEGRAPHICS_TILESETTYPE_HPP_

#include <memory>
#include <string>
#include <vector>
#include <camoto/stream.hpp>
#include <camoto/suppitem.hpp>
#include <camoto/gamegraphics/tileset.hpp>

/// Main namespace
namespace camoto {
/// Namespace for this library
namespace gamegraphics {

/// Interface to a particular tileset format.
class TilesetType
{
	public:
		/// Type of object this class creates
		typedef Tileset obj_t;

		/// Type name as a string
		static constexpr const char* const obj_t_name = "Tileset";

		/// Confidence level when guessing a file format.
		enum Certainty {
			DefinitelyNo,  ///< Definitely not in this format
			Unsure,        ///< The checks were inconclusive, it could go either way
			PossiblyYes,   ///< Everything checked out OK, but there's no signature
			DefinitelyYes, ///< This format has a signature and it matched
		};

		/// Get a short code to identify this file format, e.g. "tls-xargon"
		/**
		 * This can be useful for command-line arguments.
		 *
		 * @return The tileset short name/ID.
		 */
		virtual std::string code() const = 0;

		/// Get the tileset name, e.g. "Xargon tileset file"
		/**
		 * @return The tileset name.
		 */
		virtual std::string friendlyName() const = 0;

		/// Get a list of the known file extensions for this format.
		/**
		 * @return A vector of file extensions, e.g. "xr1", "xr2".  Can be an
		 *   empty vector (if the tileset are normally stored in an archive file,
		 *   and the archive file doesn't support filenames.)  The file extensions
		 *   may also be generic, other data may be found in files with these
		 *   extensions too!
		 */
		virtual std::vector<std::string> fileExtensions() const = 0;

		/// Get a list of games using this format.
		/**
		 * @return A vector of game names, such as "Crystal Caves",
		 *   "Secret Agent"
		 */
		virtual std::vector<std::string> games() const = 0;

		/// Check a stream to see if it's in this tileset format.
		/**
		 * @param content
		 *   A stream of the file to test.
		 *
		 * @return A single confidence value from \ref Certainty.
		 *
		 * @note Many tileset formats lack a file header, so Unsure will be a
		 *   common return value, especially with small files.
		 */
		virtual Certainty isInstance(stream::input& content) const = 0;

		/// Create a blank tileset file in this format.
		/**
		 * This function writes out the necessary signatures and headers to create
		 * a valid blank tileset file in this format.
		 *
		 * Note to format implementors: This function only needs to be overridden
		 * if there are headers to write, otherwise an empty stream is passed to
		 * open() which is expected to succeed.
		 *
		 * @param content
		 *   A blank stream to store the new tileset in.
		 *
		 * @param suppData
		 *   Any supplemental data required by this format (see getRequiredSupps())
		 *
		 * @return A pointer to an instance of the Tileset class, just as if a
		 *   valid empty file had been opened by open().
		 */
		virtual std::shared_ptr<Tileset> create(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const = 0;

		/// Open a tileset file.
		/**
		 * @pre Recommended that isInstance() has returned > DefinitelyNo.
		 *
		 * @param content
		 *   The tileset file.
		 *
		 * @param suppData
		 *   Any supplemental data required by this format (see getRequiredSupps())
		 *
		 * @return A pointer to an instance of the Tileset class.  Will throw an
		 *   exception if the data is invalid (i.e. if isInstance() returned
		 *   DefinitelyNo) however it will try its best to read the data anyway, to
		 *   make it possible to "force" a file to be opened by a particular format
		 *   handler.
		 */
		virtual std::shared_ptr<Tileset> open(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const = 0;

		/// Get a list of any required supplemental files.
		/**
		 * For some tileset formats, data is stored externally to the tileset file
		 * itself (for example the palette may be stored in a different file.)
		 * This function obtains a list of these supplementary files, so the caller
		 * can open them and pass them along to the tileset manipulation classes.
		 *
		 * @param filenameTileset
		 *   The filename of the tileset.  This is for supplemental files which
		 *   share the same base name as the tileset, but a different filename
		 *   extension.  The filename should not have a path.
		 *
		 * @return A (possibly empty) map associating required supplemental file
		 *   types with their filenames.  For each returned value the file should
		 *   be opened and placed in a SuppItem instance.  The SuppItem is then
		 *   added to an \ref SuppData map where it can be passed to
		 *   create() or open().
		 */
		virtual SuppFilenames getRequiredSupps(const std::string& filenameTileset)
			const = 0;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_TILESETTYPE_HPP_
