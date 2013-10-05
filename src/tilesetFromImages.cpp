/**
 * @file   tilesetFromImages.cpp
 * @brief  Tiles loaded from a list of images.
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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

#include <boost/bind.hpp>
#include "basetileset.hpp"

namespace camoto {
namespace gamegraphics {

class TilesetFromImages: virtual public BaseTileset
{
	public:
		struct ImageEntry: virtual public BaseTileset::BaseTilesetEntry {
			TilesetFromImages_Item item;
		};

		TilesetFromImages(TilesetFromImages_List& content, unsigned int layoutWidth);
		virtual ~TilesetFromImages();

		virtual int getCaps();
		virtual const VC_ENTRYPTR& getItems() const;
		virtual ImagePtr openImage(const EntryPtr& id);
		virtual TilesetPtr openTileset(const EntryPtr& id);
		virtual EntryPtr insert(const EntryPtr& idBeforeThis, int attr);
		virtual void remove(EntryPtr& id);
		virtual void resize(EntryPtr& id, stream::len newSize);
		virtual void flush();
		virtual unsigned int getLayoutWidth();

	protected:
		unsigned int layoutWidth;       ///< Return value for getLayoutWidth()
		VC_ENTRYPTR items;              ///< List of tiles
		int caps;                       ///< Return value for getCaps()
};

// Exported in tileset.hpp
TilesetPtr createTilesetFromImages(TilesetFromImages_List& content,
	unsigned int layoutWidth)
{
	return TilesetPtr(new TilesetFromImages(content, layoutWidth));
}


TilesetFromImages::TilesetFromImages(TilesetFromImages_List& content,
	unsigned int layoutWidth)
	:	layoutWidth(layoutWidth),
		caps(0)
{
	for (TilesetFromImages_List::iterator
		i = content.begin(); i != content.end(); i++
	) {
		ImageEntry *fat = new ImageEntry;
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = i->isImage ? Tileset::Default : Tileset::SubTileset;
		if (!i->name.empty()) this->caps |= Tileset::HasNames;
		fat->name = i->name;
		fat->item = *i;
		this->items.push_back(ep);
	}
}

TilesetFromImages::~TilesetFromImages()
{
}

int TilesetFromImages::getCaps()
{
	return this->caps;
}

const Tileset::VC_ENTRYPTR& TilesetFromImages::getItems() const
{
	return this->items;
}

ImagePtr TilesetFromImages::openImage(const EntryPtr& id)
{
	ImageEntry *fat = dynamic_cast<ImageEntry *>(id.get());
	assert(fat);
	return fat->item.image;
}

TilesetPtr TilesetFromImages::openTileset(const EntryPtr& id)
{
	ImageEntry *fat = dynamic_cast<ImageEntry *>(id.get());
	assert(fat);
	return fat->item.tileset;
}

Tileset::EntryPtr TilesetFromImages::insert(const EntryPtr& idBeforeThis, int attr)
{
	throw stream::error("tiles in this tileset cannot be rearranged");
}

void TilesetFromImages::remove(EntryPtr& id)
{
	throw stream::error("tiles in this tileset cannot be rearranged");
}

void TilesetFromImages::flush()
{
	return;
}

void TilesetFromImages::resize(EntryPtr& id, stream::len newSize)
{
	// This isn't a real tileset, so sure, we can resize ;-)
	return;
}

unsigned int TilesetFromImages::getLayoutWidth()
{
	return this->layoutWidth;
}

} // namespace gamegraphics
} // namespace camoto
