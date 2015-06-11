/**
 * @file  tileset-from_image_list.cpp
 * @brief Create a tileset from a collection of other images.
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

#include <cassert>
#include <camoto/util.hpp> // make_unique
#include <camoto/gamegraphics/tileset-from_image_list.hpp>
#include "image-sub.hpp"

namespace camoto {
namespace gamegraphics {

Tileset_FromImageList::Tileset_FromImageList(std::vector<Item> imageList,
	unsigned int layoutWidth)
	:	imageList(imageList),
		prefLayoutWidth(layoutWidth)
{
	this->pal = pal;

	unsigned int j = 0, count = 0;
	for (auto& i : this->imageList) {
		i.imageChanged = false;
		switch (i.att) {
			case Item::AttachmentType::Append:
				switch (i.split) {
					case Item::SplitType::SingleTile: {
						auto fat = std::make_unique<ImageListEntry>();
						fat->bValid = true;
						fat->fAttr = File::Attribute::Default;
						fat->type = "tileset/from-image-list+single";
						fat->index = count++;
						fat->srcindex = j;
						fat->subindex = 0;
						this->vcFAT.push_back(std::move(fat));
						break;
					}
					case Item::SplitType::UniformTiles: {
						assert(i.tileSize.x != 0);
						assert(i.tileSize.y != 0);
						unsigned int k = 0;
						for (int y = 0; y < i.tileArea.height; y += i.tileSize.y) {
							for (int x = 0; x < i.tileArea.width; x += i.tileSize.x) {
								auto fat = std::make_unique<ImageListEntry>();
								fat->bValid = true;
								fat->fAttr = File::Attribute::Default;
								fat->type = "tileset/from-image-list+uniform-entry";
								fat->index = count++;
								fat->srcindex = j;
								fat->subindex = k++;
								fat->dims = {x, y, i.tileSize.x, i.tileSize.y};
								this->vcFAT.push_back(std::move(fat));
							}
						}
						break;
					}
					case Item::SplitType::List: {
						unsigned int k = 0;
						for (auto& l : i.coords) {
							auto fat = std::make_unique<ImageListEntry>();
							fat->bValid = true;
							fat->fAttr = File::Attribute::Default;
							fat->type = "tileset/from-image-list+list-entry";
							fat->index = count++;
							fat->srcindex = j;
							fat->subindex = k++;
							fat->dims = l;
							this->vcFAT.push_back(std::move(fat));
						}
						break;
					}
				}
				break;
			case Item::AttachmentType::Child: {
				auto fat = std::make_unique<ImageListEntry>();
				fat->bValid = true;
				fat->fAttr = File::Attribute::Folder;
				fat->type = "tileset/from-image-list+sub";
				fat->index = count++;
				fat->srcindex = j;
				fat->subindex = 0;
				this->vcFAT.push_back(std::move(fat));
				break;
			}
		}
		j++;
	}
}

Tileset_FromImageList::~Tileset_FromImageList()
{
}

const Tileset::FileVector& Tileset_FromImageList::files() const
{
	return this->vcFAT;
}

const Tileset::FileHandle Tileset_FromImageList::find(const std::string& strFilename) const
{
	// no filenames means no matches, ever
	return {};
}

bool Tileset_FromImageList::isValid(const FileHandle& id) const
{
	return id && id->bValid;
}

std::unique_ptr<stream::inout> Tileset_FromImageList::open(const FileHandle& id, bool useFilter)
{
	throw stream::error("tiles in this tileset are virtual and have no "
		"underlying stream");
}

std::shared_ptr<camoto::gamearchive::Archive> Tileset_FromImageList::openFolder(
	const FileHandle& id)
{
	// TODO: This could call openTileset() and return that, but only if
	// openTileset() isn't going to call us for some reason!
	throw stream::error("tiles in this tileset are virtual and have no "
		"underlying storage");
}

const Tileset::FileHandle Tileset_FromImageList::insert(const FileHandle& idBeforeThis,
	const std::string& strFilename, stream::len storedSize, std::string type,
	File::Attribute attr)
{
	throw stream::error("tiles in this tileset cannot be rearranged");
}

void Tileset_FromImageList::remove(const FileHandle& id)
{
	throw stream::error("tiles in this tileset cannot be rearranged (yet?)");
}

void Tileset_FromImageList::rename(const FileHandle& id, const std::string& strNewName)
{
	throw stream::error("tiles in this tileset cannot be renamed");
}

void Tileset_FromImageList::move(const FileHandle& idBeforeThis, const FileHandle& id)
{
	throw stream::error("tiles in this tileset cannot be rearranged");
}

void Tileset_FromImageList::resize(const FileHandle& id, stream::len newStoredSize,
	stream::len newRealSize)
{
	// Should never get any resize requests from child objects (which are either
	// same as us or Image_Sub instances)
	throw stream::error("tiles in this tileset are a fixed size");
}

void Tileset_FromImageList::flush()
{
	for (auto& i : this->imageList) {
		if (i.imageChanged) {
			i.img->convert(*i.stdImg, *i.stdMask);
			i.imageChanged = false;
		}
	}
	return;
}

Tileset::Caps Tileset_FromImageList::caps() const
{
	// Palettes belong to individual images, not this virtual tileset.
	return Caps::Default;
}

ColourDepth Tileset_FromImageList::colourDepth() const
{
	// Just return the colour depth of the first image, as they are all supposed
	// to be the same.
	return this->imageList[0].img->colourDepth();
}

Point Tileset_FromImageList::dimensions() const
{
	// See if all tilesets are uniform appended ones and if so, return the uniform
	// size.  Otherwise return {0, 0} to signal that the size varies.
	Point dims{0, 0};
	for (auto& i : this->imageList) {
		if (
			(i.att == Item::AttachmentType::Append)
			&& (i.split == Item::SplitType::UniformTiles)
		) {
			if ((dims.x == 0) && (dims.y == 0)) dims = i.tileSize;
			else if (dims != i.tileSize) {
				// Two or more uniform tilesets, with different sizes
				return {0, 0};
			}
		} else {
			// Non append/uniform item
			return {0, 0};
		}
	}
	return dims;
}

unsigned int Tileset_FromImageList::layoutWidth() const
{
	return this->prefLayoutWidth;
}

std::unique_ptr<Image> Tileset_FromImageList::openImage(const FileHandle& id)
{
	auto fat = ImageListEntry::cast(id);
	auto& item = this->imageList[fat->srcindex];
	switch (item.att) {
		case Item::AttachmentType::Append:
			switch (item.split) {
				case Item::SplitType::SingleTile:
					// TODO need to create a unique_ptr copy
#warning Not implemented yet!
throw stream::error("TODO: Not implemented yet!");
//					return item.img;

				case Item::SplitType::UniformTiles:
				case Item::SplitType::List:
					if (!item.stdImg) {
						item.stdImg = std::make_shared<Pixels>(item.img->convert());
						item.stdMask = std::make_shared<Pixels>(item.img->convert_mask());
					}
					return std::make_unique<Image_Sub>(item.stdImg, item.stdMask,
						item.img->dimensions(), fat->dims, item.img->colourDepth(),
						item.img->palette(),
						[&item](){
							item.imageChanged = true;
						}
					);
			}
			break;
		case Item::AttachmentType::Child:
			throw stream::error("Tried to open sub-tileset as an image.");
	}
	throw stream::error("Invalid image list item attachment/split type.");
}

std::shared_ptr<Tileset> Tileset_FromImageList::openTileset(const FileHandle& id)
{
	auto fat = ImageListEntry::cast(id);
	auto& item = this->imageList[fat->srcindex];
	if (item.att != Item::AttachmentType::Child) {
		throw stream::error("Tried to open tile as a sub-tileset.");
	}

	// If a tileset was given, just use that.
	if (item.tileset) return item.tileset;

	// Copy the item but make it append not child
	Item n = item;
	n.att = Item::AttachmentType::Append;
	return std::make_shared<Tileset_FromImageList>(
		std::vector<Item>{n}, this->layoutWidth()
	);
}

} // namespace gamegraphics
} // namespace camoto
