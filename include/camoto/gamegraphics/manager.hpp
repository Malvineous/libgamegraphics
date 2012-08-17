/**
 * @file   manager.hpp
 * @brief  Manager class, used for accessing the various graphics readers.
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
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
#include <vector>

#include <camoto/stream.hpp>
#include <stdint.h>
#include <camoto/gamegraphics/tilesettype.hpp>
#include <camoto/gamegraphics/imagetype.hpp>

namespace camoto {
namespace gamegraphics {

class Manager;

/// Shared pointer to a Manager.
typedef boost::shared_ptr<Manager> ManagerPtr;

/// Library entry point.
/**
 * All further functionality is provided by calling functions in the Manager
 * class.
 *
 * @return A shared pointer to a Manager instance.
 */
ManagerPtr getManager(void);

/// Top-level class to manage graphics types.
/**
 * This class provides access to the different graphics file formats supported
 * by the library.
 *
 * In order to open an graphics, this class must be used to access an instance
 * of the graphics type.  This TilesetType instance is then used to create an
 * Tileset instance around a particular file.  It is this Tileset instance that
 * is then used to manipulate the graphics file itself.
 *
 * @note This class shouldn't be created manually, use the global function
 *       getManager() to obtain a pointer to it.
 */
class Manager {
	private:
		/// List of available graphics types.
		VC_TILESETTYPE vcTilesetTypes;
		VC_IMAGETYPE vcImageTypes;

		Manager();

		friend ManagerPtr getManager(void);

	public:

		~Manager();

		/// Get an TilesetType instance for a supported file format.
		/**
		 * This can be used to enumerate all available file formats.
		 *
		 * @param  iIndex Index of the format, starting from 0.
		 * @return A shared pointer to an TilesetType for the given index, or
		 *         an empty pointer once iIndex goes out of range.
		 * @todo Remove this and replace it with a function that just returns the vector.
		 */
		TilesetTypePtr getTilesetType(unsigned int iIndex);

		/// Get an TilesetType instance by its code.
		/**
		 * @param  strCode %Tileset code (e.g. "grp-duke3d")
		 * @return A shared pointer to an TilesetType for the given code, or
		 *         an empty pointer on an invalid code.
		 */
		TilesetTypePtr getTilesetTypeByCode(const std::string& strCode);

		/// Get an ImageType instance for a supported file format.
		/**
		 * This can be used to enumerate all available file formats.
		 *
		 * @param  iIndex Index of the format, starting from 0.
		 * @return A shared pointer to an ImageType for the given index, or
		 *         an empty pointer once iIndex goes out of range.
		 * @todo Remove this and replace it with a function that just returns the vector.
		 */
		ImageTypePtr getImageType(unsigned int iIndex);

		/// Get an ImageType instance by its code.
		/**
		 * @param  strCode %Image code (e.g. "grp-duke3d")
		 * @return A shared pointer to an ImageType for the given code, or
		 *         an empty pointer on an invalid code.
		 */
		ImageTypePtr getImageTypeByCode(const std::string& strCode);

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_MANAGER_HPP_
