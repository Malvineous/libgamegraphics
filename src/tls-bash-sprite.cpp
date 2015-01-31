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
#include "img-bash-sprite.hpp"
#include "tls-bash-sprite.hpp"
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

/// Number of planes in each image
#define MB_NUMPLANES_SPRITE   5

/// Offset of the first image within a sprite
#define MB_FIRST_TILE_OFFSET  1

/// Number of bytes in embedded FAT [sizeof(uint16_t)]
#define MB_SPR_EFAT_ENTRY_LEN 2

/// Minimum length (in bytes) of a blank image
#define MB_MIN_IMAGE_LEN     13

/// Minimum length (in bytes) of a sprite file
#define MB_MIN_FILE_LEN       1

class Tileset_MonsterBashSprite: virtual public Tileset_FAT
{
	public:
		Tileset_MonsterBashSprite(stream::inout_sptr data);
		virtual ~Tileset_MonsterBashSprite();

		virtual int getCaps();
		//void resize(EntryPtr& id, stream::len newSize);
		//virtual void getTilesetDimensions(unsigned int *width, unsigned int *height);
		virtual unsigned int getLayoutWidth();

		// Tileset_FAT
		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);
		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void postInsertFile(Tileset_FAT::FATEntry *pNewEntry);
		virtual void updateFileSize(const Tileset_FAT::FATEntry *pid,
			stream::len sizeDelta);

	protected:
		unsigned int lenTile;   ///< Length of each image, in bytes
};

TilesetType_MonsterBashSprite::TilesetType_MonsterBashSprite()
{
}

TilesetType_MonsterBashSprite::~TilesetType_MonsterBashSprite()
{
}

std::string TilesetType_MonsterBashSprite::getCode() const
{
	return "tls-bash-sprite";
}

std::string TilesetType_MonsterBashSprite::getFriendlyName() const
{
	return "Monster Bash Sprite";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> TilesetType_MonsterBashSprite::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("spr");
	return vcExtensions;
}

std::vector<std::string> TilesetType_MonsterBashSprite::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Monster Bash");
	return vcGames;
}

TilesetType_MonsterBashSprite::Certainty TilesetType_MonsterBashSprite::isInstance(
	stream::input_sptr psGraphics) const
{
	stream::len lenRemaining = psGraphics->size();
	psGraphics->seekg(0, stream::start);

	// TESTED BY: tls_bash_sprite_isinstance_c02
	if (lenRemaining < MB_MIN_FILE_LEN) return DefinitelyNo;

	uint8_t sig;
	psGraphics >> u8(sig);
	lenRemaining--;

	// TESTED BY: tls_bash_sprite_isinstance_c03
	if (sig != 0xFF) return DefinitelyNo;

	while (lenRemaining >= MB_SPR_EFAT_ENTRY_LEN) {
		uint16_t lenBlock;
		psGraphics >> u16le(lenBlock);
		lenRemaining -= 2;

		// TESTED BY: tls_bash_sprite_isinstance_c04
		if (lenBlock < MB_MIN_IMAGE_LEN) return DefinitelyNo;

		// TESTED BY: tls_bash_sprite_isinstance_c05
		if (lenBlock > lenRemaining) return DefinitelyNo;

		psGraphics->seekg(lenBlock - 1, stream::cur);
		uint8_t lastByte;
		psGraphics >> u8(lastByte);
		// TESTED BY: tls_bash_sprite_isinstance_c06
		if (lastByte != 0x00) return DefinitelyNo;

		lenRemaining -= lenBlock;
	}

	// TESTED BY: tls_bash_sprite_isinstance_c00
	// TESTED BY: tls_bash_sprite_isinstance_c01
	return DefinitelyYes;
}

TilesetPtr TilesetType_MonsterBashSprite::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	psGraphics << u8(0xFF);
	return TilesetPtr(new Tileset_MonsterBashSprite(psGraphics));
}

TilesetPtr TilesetType_MonsterBashSprite::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	return TilesetPtr(new Tileset_MonsterBashSprite(psGraphics));
}

SuppFilenames TilesetType_MonsterBashSprite::getRequiredSupps(
	const std::string& filenameGraphics) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


Tileset_MonsterBashSprite::Tileset_MonsterBashSprite(stream::inout_sptr data)
	:	Tileset_FAT(data, MB_FIRST_TILE_OFFSET)
{
	// Skip signature byte
	data->seekg(1, stream::start);
	stream::pos offset = 1;
	stream::len lenRemaining = this->data->size() - 1;

	int i = 0;
	while (lenRemaining >= MB_SPR_EFAT_ENTRY_LEN) {
		uint16_t lenBlock;
		data >> u16le(lenBlock);
		lenRemaining -= 2;

		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->valid = true;
		fat->attr = 0;
		fat->index = i;
		fat->offset = offset;
		fat->size = lenBlock;
		fat->lenHeader = MB_SPR_EFAT_ENTRY_LEN;
		this->items.push_back(ep);
		if (lenBlock > lenRemaining) {
			std::cerr << "Warning: Ignoring incomplete image in Monster Bash sprite "
				"(image is " << lenBlock << " bytes, but file only contains another "
				<< lenRemaining << " bytes)" << std::endl;
			break;
		}
		data->seekg(lenBlock, stream::cur);
		lenRemaining -= lenBlock;
		offset += lenBlock + MB_SPR_EFAT_ENTRY_LEN; // MB_SPR_EFAT_ENTRY_LEN == sizeof(uint16le)
		i++;
	}

}

Tileset_MonsterBashSprite::~Tileset_MonsterBashSprite()
{
}

int Tileset_MonsterBashSprite::getCaps()
{
	return Tileset::ColourDepthEGA;
}

unsigned int Tileset_MonsterBashSprite::getLayoutWidth()
{
	return 1;
}

ImagePtr Tileset_MonsterBashSprite::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
{
	return ImagePtr(new Image_BashSprite(content));
}

Tileset_MonsterBashSprite::FATEntry *Tileset_MonsterBashSprite::preInsertFile(
	const Tileset_MonsterBashSprite::FATEntry *idBeforeThis, Tileset_MonsterBashSprite::FATEntry *pNewEntry)
{
	// Set tile defaults
	pNewEntry->size = MB_MIN_IMAGE_LEN + 1;

	// Insert embedded FAT
	pNewEntry->lenHeader = MB_SPR_EFAT_ENTRY_LEN;
	this->data->seekp(pNewEntry->offset, stream::start);
	this->data->insert(MB_SPR_EFAT_ENTRY_LEN);

	// Since we've inserted some data for the embedded header, we need to update
	// the other file offsets accordingly.  This call updates the offset of the
	// files, then calls updateFileOffset() on them, using the *new* offset, so
	// we need to do this after the insert() call above to make sure the extra
	// data has been inserted.  Then when updateFileOffset() writes data out it
	// will go into the correct spot.
	this->shiftFiles(NULL, pNewEntry->offset, pNewEntry->lenHeader, 0);

	return pNewEntry;
}

void Tileset_MonsterBashSprite::postInsertFile(Tileset_FAT::FATEntry *pNewEntry)
{
	/// @todo Not necessary because of updateFileSize()?
	this->data->seekp(pNewEntry->offset, stream::start);
	this->data
		<< u16le(pNewEntry->size)
		<< u8(0)
		<< u8(0)
		<< u8(0)
		<< u8(0)
		<< s16le(0)
		<< s16le(0)
		<< u16le(0)
		<< u16le(0)
		<< u8(0) // EOF plane flag
	;
	return;
}

void Tileset_MonsterBashSprite::updateFileSize(const Tileset_FAT::FATEntry *pid,
	stream::len sizeDelta)
{
	this->data->seekp(pid->offset, stream::start);
	this->data << u16le(pid->size);
	return;
}

} // namespace gamegraphics
} // namespace camoto
