/**
 * @file  tls-vinyl.hpp
 * @brief Vinyl Goddess From Mars tileset handler.
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

#ifndef _CAMOTO_TLS_VINYL_HPP_
#define _CAMOTO_TLS_VINYL_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include <camoto/gamegraphics/palettetable.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class TilesetType_Vinyl: virtual public TilesetType
{
	public:
		TilesetType_Vinyl();
		virtual ~TilesetType_Vinyl();

		virtual std::string getCode() const;
		virtual std::string getFriendlyName() const;
		virtual std::vector<std::string> getFileExtensions() const;
		virtual std::vector<std::string> getGameList() const;
		virtual Certainty isInstance(stream::input_sptr fsGraphics) const;
		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const;
		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameGraphics) const;
};

class Tileset_Vinyl: virtual public Tileset_FAT
{
	public:
		Tileset_Vinyl(stream::inout_sptr data, PaletteTablePtr pal);
		virtual ~Tileset_Vinyl();

		virtual int getCaps();
		virtual void flush();
		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);
		virtual PaletteTablePtr getPalette();
		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void postInsertFile(FATEntry *pNewEntry);
		virtual void postRemoveFile(const FATEntry *pid);

		// Called by Image_VGFMTile
		virtual StdImageDataPtr toStandard(unsigned int index);
		virtual StdImageDataPtr toStandardMask(unsigned int index);
		virtual void fromStandard(unsigned int index, StdImageDataPtr newContent,
			StdImageDataPtr newMask);

	private:
		PaletteTablePtr pal;
		std::vector<uint8_t> pixels;         ///< Pixels for each pixel code
		bool pixelsChanged;                  ///< Does the pixel array need to be written back to the file?

		/// Update the number of tiles in the tileset
		void updateFileCount(uint32_t newCount);
};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_TLS_VINYL_HPP_
