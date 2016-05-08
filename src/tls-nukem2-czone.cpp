/**
 * @file  tls-nukem2-czone.cpp
 * @brief FixedArchive implementation for Duke Nukem II czone*.mni files.
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

#include <camoto/util.hpp> // make_unique
#include <camoto/gamearchive/fixedarchive.hpp>
#include "tls-nukem2-czone.hpp"
#include "tls-ega-apogee.hpp"
#include "pal-vga-raw.hpp"

using namespace camoto::gamearchive;

namespace camoto {
namespace gamegraphics {

class Tileset_Nukem2_CZone: virtual public FixedArchive,
	public Tileset
{
	public:
		Tileset_Nukem2_CZone(std::unique_ptr<stream::inout> content,
			const std::vector<FixedArchiveFile>& vcFiles,
			std::shared_ptr<const Palette> pal)
			:	FixedArchive(std::move(content), vcFiles)
		{
			this->pal = pal;

			// Mark the first file as vacant even though it's not, so that nothing
			// tries to open it.
			{
				auto f = FixedEntry::cast(this->vcFixedEntries[0]);
				f->fAttr = File::Attribute::Vacant;
			}

			// Change the default attributes assigned to the second and third files,
			// so that they are marked as folders.  This is how we indicate they are
			// sub-tilesets.
			{
				auto f = FixedEntry::cast(this->vcFixedEntries[1]);
				f->fAttr = File::Attribute::Folder;
			}
			{
				auto f = FixedEntry::cast(this->vcFixedEntries[2]);
				f->fAttr = File::Attribute::Folder;
			}
		}

		virtual std::shared_ptr<Archive> openFolder(const FileHandle& id)
		{
			return this->openTileset(id);
		}

		virtual Caps caps() const
		{
			// Although we don't have any images to display, we'll report having
			// a palette so any map files using this tileset can retrieve the
			// palette without having to walk through every single sub-tileset
			// just in case a sub-tileset has the main palette to use.
			return Caps::HasPalette;
		}

		virtual ColourDepth colourDepth() const
		{
			return ColourDepth::EGA;
		}

		virtual unsigned int layoutWidth() const
		{
			return 1; // no tiles anyway
		}

		virtual std::unique_ptr<Image> openImage(const FileHandle& id)
		{
			throw camoto::error("Tried to open a non-image tileset entry.");
		}

		virtual std::shared_ptr<Tileset> openTileset(const FileHandle& id)
		{
			auto f = FixedEntry::cast(id);
			return std::make_shared<Tileset_EGAApogee>(
				this->open(id, true),
				Point{8, 8},
				(f->index == 2) ? PlaneCount::Masked : PlaneCount::Solid,
				40,
				this->pal
			);
		}
};

TilesetType_Nukem2_CZone::TilesetType_Nukem2_CZone()
{
}

TilesetType_Nukem2_CZone::~TilesetType_Nukem2_CZone()
{
}

std::string TilesetType_Nukem2_CZone::code() const
{
	return "tls-nukem2-czone";
}

std::string TilesetType_Nukem2_CZone::friendlyName() const
{
	return "Duke Nukem II CZone";
}

std::vector<std::string> TilesetType_Nukem2_CZone::fileExtensions() const
{
	return {
		"mni",
	};
}

std::vector<std::string> TilesetType_Nukem2_CZone::games() const
{
	return {
		"Duke Nukem II",
	};
}

TilesetType::Certainty TilesetType_Nukem2_CZone::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// Standard tileset
	// TESTED BY: TODO
	if (len == 42000) return Certainty::PossiblyYes;

	// TESTED BY: TODO
	return Certainty::DefinitelyNo;
}

std::shared_ptr<Tileset> TilesetType_Nukem2_CZone::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	// This isn't a true archive so we can't create new versions of it.
	throw stream::error("Can't create a new archive in this format.");
}

std::shared_ptr<Tileset> TilesetType_Nukem2_CZone::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	std::shared_ptr<const Palette> pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		auto palFile = std::make_unique<Palette_VGA>(
			std::move(suppData[SuppItem::Palette]),
			6
		);
		pal = palFile->palette();
	}
	return std::make_shared<Tileset_Nukem2_CZone>(std::move(content),
		std::vector<FixedArchiveFile>{
			{0,             3600, "attrdata.mni", {}, nullptr},
			{3600,         32000, "solid.mni",    {}, nullptr},
			{3600 + 32000,  6400, "masked.mni",   {}, nullptr},
		},
		pal
	);
}

SuppFilenames TilesetType_Nukem2_CZone::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	return {
		std::make_pair(SuppItem::Palette, "gamepal.pal"),
	};
}


} // namespace gamegraphics
} // namespace camoto
