/**
 * @file  basetileset.hpp
 * @brief Base class for all Tileset subclasses.
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

#ifndef _CAMOTO_GAMEGRAPHICS_BASETILESET_HPP_
#define _CAMOTO_GAMEGRAPHICS_BASETILESET_HPP_

#include <camoto/gamegraphics/tileset.hpp>

namespace camoto {
namespace gamegraphics {

/// Base class for tilesets.
/**
 * This class implements common functions that would otherwise have to be
 * implemented in each subclass.
 */
class Tileset_Base: virtual public Tileset
{
	public:

		class Tileset_BaseEntry: virtual public Entry {
			public:
				virtual std::string getName() const;
				virtual bool isValid() const;
				virtual int getAttr() const;

				/// Name of this tile
				std::string name;

				/// Is this entry valid? (set to false upon delete)
				bool valid;

				/// One or more %Attributes flags
				int attr;
		};

		Tileset_Base();
		virtual ~Tileset_Base();

		/// Default function throwing invalid ID error.
		virtual TilesetPtr openTileset(const EntryPtr& id);

		/// Default function throwing invalid ID error.
		virtual ImagePtr openImage(const EntryPtr& id);

		/// Default function returning 0x0.
		virtual void getTilesetDimensions(unsigned int *width, unsigned int *height);

		/// Default function to throw exception.
		/**
		 * Always throws stream::error, complaining the caller should have checked
		 * getCaps() for the presence of ChangeDimensions.
		 *
		 * @throw stream::error on every call.
		 */
		virtual void setTilesetDimensions(unsigned int width, unsigned int height);

		/// Default implementation that returns 0.
		virtual unsigned int getLayoutWidth();

		/// Default function triggers an assert() for formats with no palette.
		virtual PaletteTablePtr getPalette();

		/// Default function to throw exception.
		/**
		 * Always throws stream::error, complaining the caller should have checked
		 * getCaps() for the presence of HasPalette.
		 *
		 * @throw stream::error on every call.
		 */
		virtual void setPalette(PaletteTablePtr newPalette);
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_BASETILESET_HPP_
