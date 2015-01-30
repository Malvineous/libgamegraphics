/**
 * @file  camoto/gamegraphics/manager.hpp
 * @brief Manager class, used for accessing the graphics format handlers.
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

#ifndef _CAMOTO_GAMEGRAPHICS_MANAGER_HPP_
#define _CAMOTO_GAMEGRAPHICS_MANAGER_HPP_

#include <boost/shared_ptr.hpp>
#include <camoto/gamegraphics/tilesettype.hpp>
#include <camoto/gamegraphics/imagetype.hpp>

#ifndef DLL_EXPORT
#define DLL_EXPORT
#endif

namespace camoto {
namespace gamegraphics {

/// Top-level class to manage graphics types.
/**
 * This class provides access to the different graphics file formats supported
 * by the library.
 *
 * In order to open a file, this class must be used to access an instance of the
 * tileset or image type.  This TilesetType or ImageType instance is then used
 * to create a Tileset or Image instance around a particular file.  It is this
 * instance that is then used to manipulate the graphics file itself.
 *
 * @note Use the free function getManager() to obtain a pointer to an instance
 *   of an object implementing the Manager interface.
 */
class Manager
{
	public:
		/// Get an TilesetType instance for a supported file format.
		/**
		 * This can be used to enumerate all available file formats.
		 *
		 * @param iIndex
		 *   Index of the format, starting from 0.
		 *
		 * @return A shared pointer to a TilesetType instance for the given index,
		 *   or an pointer once iIndex goes out of range.
		 */
		virtual const TilesetTypePtr getTilesetType(unsigned int iIndex)
			const = 0;

		/// Get an TilesetType instance by its code.
		/**
		 * @param strCode
		 *   %Tileset code (e.g. "grp-duke3d")
		 *
		 * @return A shared pointer to a TilesetType instance for the given code, or
		 *   an empty pointer for an invalid code.
		 */
		virtual const TilesetTypePtr getTilesetTypeByCode(
			const std::string& strCode) const = 0;

		/// Get an ImageType instance for a supported file format.
		/**
		 * This can be used to enumerate all available file formats.
		 *
		 * @param iIndex
		 *  Index of the format, starting from 0.
		 *
		 * @return A shared pointer to an ImageType instance for the given index, or
		 *   an empty pointer once iIndex goes out of range.
		 */
		virtual const ImageTypePtr getImageType(unsigned int iIndex)
			const = 0;

		/// Get an ImageType instance by its code.
		/**
		 * @param strCode
		 *   %Image code (e.g. "grp-duke3d")
		 *
		 * @return A shared pointer to an ImageType instance for the given code, or
		 *   an empty pointer for an invalid code.
		 */
		virtual const ImageTypePtr getImageTypeByCode(const std::string& strCode)
			const = 0;
};

/// Shared pointer to a Manager.
typedef boost::shared_ptr<Manager> ManagerPtr;

/// Library entry point.
/**
 * All further functionality is provided by calling functions in the Manager
 * class.
 *
 * @return A shared pointer to a Manager instance.
 */
const ManagerPtr DLL_EXPORT getManager(void);

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_MANAGER_HPP_
