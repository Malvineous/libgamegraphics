/**
 * @file  tls-bash.cpp
 * @brief Monster Bash sprite handler
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Monster_Bash_Sprite_Format
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

#include <camoto/iostream_helpers.hpp>
#include <camoto/util.hpp> // make_unique
#include "img-bash-sprite.hpp"
#include "tls-bash-sprite.hpp"
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

/// Offset of the first image within a sprite
#define MB_FIRST_TILE_OFFSET  1

/// Number of bytes in embedded FAT [sizeof(uint16_t)]
#define MB_SPR_EFAT_ENTRY_LEN 2

/// Minimum length (in bytes) of a blank image
#define MB_MIN_IMAGE_LEN     13

/// Minimum length (in bytes) of a sprite file
#define MB_MIN_FILE_LEN       1

#define FILETYPE_BASH_SPRITE "tile/bash-sprite"

class Tileset_MonsterBashSprite: virtual public Tileset_FAT
{
	public:
		Tileset_MonsterBashSprite(std::unique_ptr<stream::inout> content);
		virtual ~Tileset_MonsterBashSprite();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::unique_ptr<Image> openImage(FileHandle& id);
		virtual FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);
		using Archive::insert;
		virtual void preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void postInsertFile(Tileset_FAT::FATEntry *pNewEntry);
		virtual void updateFileSize(const Tileset_FAT::FATEntry *pid,
			stream::delta sizeDelta);
};

TilesetType_MonsterBashSprite::TilesetType_MonsterBashSprite()
{
}

TilesetType_MonsterBashSprite::~TilesetType_MonsterBashSprite()
{
}

std::string TilesetType_MonsterBashSprite::code() const
{
	return "tls-bash-sprite";
}

std::string TilesetType_MonsterBashSprite::friendlyName() const
{
	return "Monster Bash Sprite";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> TilesetType_MonsterBashSprite::fileExtensions() const
{
	return {"spr"};
}

std::vector<std::string> TilesetType_MonsterBashSprite::games() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Monster Bash");
	return vcGames;
}

TilesetType::Certainty TilesetType_MonsterBashSprite::isInstance(
	stream::input& content) const
{
	stream::len lenRemaining = content.size();
	content.seekg(0, stream::start);

	// TESTED BY: tls_bash_sprite_isinstance_c02
	if (lenRemaining < MB_MIN_FILE_LEN) return DefinitelyNo;

	uint8_t sig;
	content >> u8(sig);
	lenRemaining--;

	// TESTED BY: tls_bash_sprite_isinstance_c03
	if (sig != 0xFF) return DefinitelyNo;

	while (lenRemaining >= MB_SPR_EFAT_ENTRY_LEN) {
		uint16_t lenBlock;
		content >> u16le(lenBlock);
		lenRemaining -= 2;

		// TESTED BY: tls_bash_sprite_isinstance_c04
		if (lenBlock < MB_MIN_IMAGE_LEN) return DefinitelyNo;

		// TESTED BY: tls_bash_sprite_isinstance_c05
		if (lenBlock > lenRemaining) return DefinitelyNo;

		content.seekg(lenBlock - 1, stream::cur);
		uint8_t lastByte;
		content >> u8(lastByte);
		// TESTED BY: tls_bash_sprite_isinstance_c06
		if (lastByte != 0x00) return DefinitelyNo;

		lenRemaining -= lenBlock;
	}

	// TESTED BY: tls_bash_sprite_isinstance_c00
	// TESTED BY: tls_bash_sprite_isinstance_c01
	return DefinitelyYes;
}

std::shared_ptr<Tileset> TilesetType_MonsterBashSprite::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->truncate(1);
	content->seekp(0, stream::start);
	*content << u8(0xFF);
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_MonsterBashSprite::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	return std::make_shared<Tileset_MonsterBashSprite>(std::move(content));
}

SuppFilenames TilesetType_MonsterBashSprite::getRequiredSupps(
	const std::string& filenameGraphics) const
{
	return {};
}


Tileset_MonsterBashSprite::Tileset_MonsterBashSprite(
	std::unique_ptr<stream::inout> content)
	:	Tileset_FAT(std::move(content), MB_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES)
{
	// Skip signature byte
	this->content->seekg(1, stream::start);
	stream::pos offset = 1;
	stream::len lenRemaining = this->content->size() - 1;

	int i = 0;
	while (lenRemaining >= MB_SPR_EFAT_ENTRY_LEN) {
		uint16_t lenBlock;
		*this->content >> u16le(lenBlock);
		lenRemaining -= 2;

		auto fat = std::make_unique<FATEntry>();
		fat->bValid = true;
		fat->fAttr = File::Attribute::Default;
		fat->iIndex = i;
		fat->iOffset = offset;
		fat->storedSize = lenBlock;
		fat->lenHeader = MB_SPR_EFAT_ENTRY_LEN;
		fat->type = FILETYPE_BASH_SPRITE;
		this->vcFAT.push_back(std::move(fat));
		if (lenBlock > lenRemaining) {
			std::cerr << "Warning: Ignoring incomplete image in Monster Bash sprite "
				"(image is " << lenBlock << " bytes, but file only contains another "
				<< lenRemaining << " bytes)" << std::endl;
			break;
		}
		this->content->seekg(lenBlock, stream::cur);
		lenRemaining -= lenBlock;
		offset += lenBlock + MB_SPR_EFAT_ENTRY_LEN; // MB_SPR_EFAT_ENTRY_LEN == sizeof(uint16le)
		i++;
	}

}

Tileset_MonsterBashSprite::~Tileset_MonsterBashSprite()
{
}

Tileset::Caps Tileset_MonsterBashSprite::caps() const
{
	return Caps::Default;
}

ColourDepth Tileset_MonsterBashSprite::colourDepth() const
{
	return ColourDepth::EGA;
}

unsigned int Tileset_MonsterBashSprite::layoutWidth() const
{
	return 1;
}

std::unique_ptr<Image> Tileset_MonsterBashSprite::openImage(FileHandle& id)
{
	return std::make_unique<Image_BashSprite>(this->open(id, true));
}

Tileset::FileHandle Tileset_MonsterBashSprite::insert(
	const FileHandle& idBeforeThis, File::Attribute attr)
{
	return this->insert(idBeforeThis, "", MB_MIN_IMAGE_LEN, FILETYPE_BASH_SPRITE,
		attr);
}

void Tileset_MonsterBashSprite::preInsertFile(const FATEntry *idBeforeThis,
	FATEntry *pNewEntry)
{
	// Set tile defaults
	//pNewEntry->storedSize = MB_MIN_IMAGE_LEN + 1;

	// Insert embedded FAT
	pNewEntry->lenHeader = MB_SPR_EFAT_ENTRY_LEN;
	this->content->seekp(pNewEntry->iOffset, stream::start);
	this->content->insert(MB_SPR_EFAT_ENTRY_LEN);

	// Since we've inserted some data for the embedded header, we need to update
	// the other file offsets accordingly.  This call updates the offset of the
	// files, then calls updateFileOffset() on them, using the *new* offset, so
	// we need to do this after the insert() call above to make sure the extra
	// data has been inserted.  Then when updateFileOffset() writes data out it
	// will go into the correct spot.
	this->shiftFiles(NULL, pNewEntry->iOffset, pNewEntry->lenHeader, 0);

	return;
}

void Tileset_MonsterBashSprite::postInsertFile(Tileset_FAT::FATEntry *pNewEntry)
{
	this->content->seekp(pNewEntry->iOffset, stream::start);
	*this->content << u16le(pNewEntry->storedSize);
	return;
}

void Tileset_MonsterBashSprite::updateFileSize(const Tileset_FAT::FATEntry *pid,
	stream::delta sizeDelta)
{
	this->content->seekp(pid->iOffset, stream::start);
	*this->content << u16le(pid->storedSize);
	return;
}

} // namespace gamegraphics
} // namespace camoto
