/**
 * @file  tls-actrinfo.cpp
 * @brief Cosmo actor tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Cosmo_Tileinfo_Format
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

#include <algorithm>
#include <cassert>
#include <camoto/iostream_helpers.hpp>
#include <camoto/stream_filtered.hpp>
#include <camoto/util.hpp> // make_unique
#include <camoto/gamegraphics/palette.hpp>
#include <camoto/gamearchive/stream_archfile.hpp>
#include "tls-actrinfo.hpp"
#include "tileset-fat.hpp"
#include "filter-block-pad.hpp"
#include "image-from_tileset.hpp"
#include "tls-ega-apogee.hpp"

/// Number of planes in each image
#define ACTR_NUMPLANES  (PlaneCount::Masked)

/// Offset of first tileset in a tileset collection.
#define ACTR_FIRST_TILE_OFFSET 0

/// Offset of first tile/frame in a single actor tileset.
#define ACTR_SINGLE_FIRST_TILE_OFFSET 0

/// Size of each tile within a single actor frame, in pixels.
#define ACTR_TILE_WIDTH 8
#define ACTR_TILE_HEIGHT 8

/// Size of a single tile, in bytes
#define ACTR_TILE_SIZE_BYTES (ACTR_TILE_WIDTH * ACTR_TILE_HEIGHT * (int)ACTR_NUMPLANES / 8)

/// Length of each entry in the FAT.
#define ACTRINFO_FAT_ENTRY_LEN 2

/// Extract offset of FAT entry for given FATEntry object.
#define ACTRINFO_FATENTRY_OFFSET(x) ((x)->iIndex * ACTRINFO_FAT_ENTRY_LEN)

/// Size of a single actor frame, in bytes.
#define ACTOR_FRAME_SIZE 8

/// Byte offset to insert a pad byte when reading, and remove when writing.
#define ACTRINFO_PAD_BLOCK 65535

namespace camoto {
namespace gamegraphics {

/// Tileset for the full list of actors, one sub-tileset for each actor.
class Tileset_ActrInfo_Frames: virtual public Tileset_FAT
{
	public:
		Tileset_ActrInfo_Frames(std::unique_ptr<stream::inout> contentTileset,
			std::unique_ptr<stream::inout> contentFAT,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_ActrInfo_Frames();

		// Archive
		std::unique_ptr<stream::inout> open(const FileHandle& id, bool useFilter);
		virtual void flush();

		// Archive_FAT
		virtual void updateFileOffset(const FATEntry *pid, stream::delta offDelta);
		virtual void updateFileSize(const FATEntry *pid, stream::delta sizeDelta);
		virtual void preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);
		virtual void preRemoveFile(const FATEntry *pid);

		// Tileset
		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;

		// Tileset_FAT
		virtual std::shared_ptr<Tileset> openTileset(const FileHandle& id);
		virtual std::unique_ptr<Image> openImage(const FileHandle& id);

		const FileVector& filesActors() const;
		stream::len getFirstFrameOffset() const;
		const FileHandle insertActor(const FileHandle& idBeforeThis,
			const std::string& strFilename, stream::len storedSize, std::string type,
			File::Attribute attr);
		const FileHandle insertFrame(const FATEntry* fatActor,
			const FileHandle& idBeforeThis, const std::string& strFilename,
			stream::len storedSize, std::string type, File::Attribute attr);
		void removeActor(const FileHandle& id);

	protected:
		/// Actual ACTRINFO.MNI data
		std::shared_ptr<stream::seg> contentFAT;

		/// List of actors (sub-tilesets; this->vcFAT is frames across all actors)
		FileVector vcActors;

		/// Actor currently being modified by insert()
		long currentActor;

		/// Dimensions (in tiles) to use when inserting new images as raw files
		Point currentDims;

		friend class Tileset_SingleActor;
};

class Tileset_ActrInfo: virtual public Tileset
{
	public:
		Tileset_ActrInfo(std::unique_ptr<stream::inout> contentTileset,
			std::unique_ptr<stream::inout> contentFAT,
			std::shared_ptr<const Palette> pal);
		virtual ~Tileset_ActrInfo();

		virtual const FileVector& files() const;
		virtual const FileHandle find(const std::string& strFilename) const;
		virtual bool isValid(const FileHandle& id) const;
		virtual std::unique_ptr<stream::inout> open(const FileHandle& id, bool useFilter);
		virtual std::shared_ptr<Archive> openFolder(const FileHandle& id);
		virtual const FileHandle insert(const FileHandle& idBeforeThis,
			const std::string& strFilename, stream::len storedSize, std::string type,
			File::Attribute attr);
		virtual void remove(const FileHandle& id);
		virtual void rename(const FileHandle& id, const std::string& strNewName);
		virtual void move(const FileHandle& idBeforeThis, const FileHandle& id);
		virtual void resize(const FileHandle& id, stream::len newStoredSize,
			stream::len newRealSize);
		virtual void flush();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;
		virtual std::unique_ptr<Image> openImage(const FileHandle& id);
		virtual std::shared_ptr<Tileset> openTileset(const FileHandle& id);
		virtual const FileHandle insert(const FileHandle& idBeforeThis,
			File::Attribute attr);

	protected:
		std::shared_ptr<Tileset_ActrInfo_Frames> tsFrames;
};


/// Tileset containing all the frames for a single actor.
class Tileset_SingleActor: virtual public Tileset
{
	public:
		Tileset_SingleActor(std::shared_ptr<Tileset_ActrInfo_Frames> actorInfo,
			std::shared_ptr<const Tileset_FAT::FATEntry> fatActor);
		virtual ~Tileset_SingleActor();

		virtual const FileVector& files() const;
		virtual const FileHandle find(const std::string& strFilename) const;
		virtual bool isValid(const FileHandle& id) const;
		virtual std::unique_ptr<stream::inout> open(const FileHandle& id, bool useFilter);
		virtual std::shared_ptr<Archive> openFolder(const FileHandle& id);
		virtual const FileHandle insert(const FileHandle& idBeforeThis,
			const std::string& strFilename, stream::len storedSize, std::string type,
			File::Attribute attr);
		virtual void remove(const FileHandle& id);
		virtual void rename(const FileHandle& id, const std::string& strNewName);
		virtual void move(const FileHandle& idBeforeThis, const FileHandle& id);
		virtual void resize(const FileHandle& id, stream::len newStoredSize,
			stream::len newRealSize);
		virtual void flush();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual unsigned int layoutWidth() const;

		// Tileset
		virtual std::unique_ptr<Image> openImage(const FileHandle& id);
		virtual std::shared_ptr<Tileset> openTileset(const FileHandle& id);

	protected:
		mutable FileVector vcFrames;

		std::shared_ptr<Tileset_ActrInfo_Frames> actorInfo;
		std::shared_ptr<const Tileset_FAT::FATEntry> fatActor;
};

//
// TilesetType_Actrinfo
//

TilesetType_Actrinfo::TilesetType_Actrinfo()
{
}

TilesetType_Actrinfo::~TilesetType_Actrinfo()
{
}

std::string TilesetType_Actrinfo::code() const
{
	return "tls-actrinfo";
}

std::string TilesetType_Actrinfo::friendlyName() const
{
	return "Cosmo Actor Tileset";
}

std::vector<std::string> TilesetType_Actrinfo::fileExtensions() const
{
	return {"mni"};
}

std::vector<std::string> TilesetType_Actrinfo::games() const
{
	return {
		"Cosmo's Cosmic Adventures",
	};
}

TilesetType_Actrinfo::Certainty TilesetType_Actrinfo::isInstance(
	stream::input& content) const
{
	auto lenContent = content.size();

	// Could be a blank file
	// TESTED BY: tls_actrinfo_isinstance_c01
	if (lenContent == 0) return Unsure;

	// First offset truncated
	// TESTED BY: tls_actrinfo_isinstance_c02
	if (lenContent == 1) return DefinitelyNo;

	content.seekg(0, stream::start);
	stream::pos nextOffset;
	content >> u16le(nextOffset);
	unsigned int numImages = nextOffset;
	nextOffset *= 2;

	// No actor data
	// TESTED BY: tls_actrinfo_isinstance_c03
	if (nextOffset > lenContent) return DefinitelyNo;

	// First FAT offset points to FAT itself
	// TESTED BY: tls_actrinfo_isinstance_c04
	if (nextOffset < 1) return DefinitelyNo;

	stream::pos fatPos = 2;
	for (unsigned int i = 0; i < numImages; i++) {

		// Have a look at the actor structure
		if (nextOffset + 8 < lenContent) {
			// This isn't an empty actor at the end of the file
			content.seekg(nextOffset, stream::start);

			uint16_t width, height;
			uint32_t offset;
			content
				>> u16le(height)
				>> u16le(width)
				>> u32le(offset)
			;

			// Actor frame too big
			// Again this is probably allowed but it won't happen normally
			// TESTED BY: tls_actrinfo_isinstance_c05
			if (
				(width > (320 / ACTR_TILE_WIDTH))
				|| (height > (200 / ACTR_TILE_HEIGHT))
			) return DefinitelyNo;

			// Offset too big
			// We'll assume tileset files are never going to be larger than 4MB
			// TESTED BY: tls_actrinfo_isinstance_c06
			if (offset > 1048576 * 4) return DefinitelyNo;

			// Go back to get the next offset
			content.seekg(fatPos, stream::start);
		}
		if (i == numImages - 1) break;

		auto lastOffset = nextOffset;
		content >> u16le(nextOffset);
		fatPos += 2;
		nextOffset *= 2;

		// Offsets decrementing
		// This is probably allowed but it will help avoid false positives
		// TESTED BY: tls_actrinfo_isinstance_c07
		if (nextOffset < lastOffset) return DefinitelyNo;

		// Truncated actor data
		// TESTED BY: tls_actrinfo_isinstance_c08
		if (nextOffset >= lenContent) return DefinitelyNo;
	}

	// TESTED BY: tls_actrinfo_isinstance_c00
	return DefinitelyYes;
}

std::shared_ptr<Tileset> TilesetType_Actrinfo::create(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	content->seekp(0, stream::start);
	// Zero tiles, 0x0
	return this->open(std::move(content), suppData);
}

std::shared_ptr<Tileset> TilesetType_Actrinfo::open(
	std::unique_ptr<stream::inout> content, SuppData& suppData) const
{
	if (suppData.find(SuppItem::Extra1) == suppData.end()) {
		throw stream::error("no actor tileset specified (missing supplementary item Extra1)");
	}

	// The file data is loaded in lots of 65535 bytes, into memory blocks of
	// 65536 bytes.  This means after every 65535 bytes, a padding byte should be
	// inserted in order for the offsets to add up correctly.  Likewise when
	// saving data, every 65536th byte should be dropped.
	auto content_filtered = std::make_unique<stream::filtered>(
		std::move(content),
		std::make_shared<filter_block_pad>(std::string(1, '\x00'), ACTRINFO_PAD_BLOCK),
		std::make_shared<filter_block_unpad>(1, ACTRINFO_PAD_BLOCK),
		nullptr
	);
	auto extra1_filtered = std::make_unique<stream::filtered>(
		std::move(suppData[SuppItem::Extra1]),
		std::make_shared<filter_block_pad>(std::string(1, '\x00'), ACTRINFO_PAD_BLOCK),
		std::make_shared<filter_block_unpad>(1, ACTRINFO_PAD_BLOCK),
		nullptr
	);

	return std::make_shared<Tileset_ActrInfo>(
		std::move(extra1_filtered),
		std::move(content_filtered),
		nullptr
	);
}

SuppFilenames TilesetType_Actrinfo::getRequiredSupps(stream::input& content,
	const std::string& filename) const
{
	SuppFilenames supps;
	if (filename.compare("plyrinfo.mni") == 0) {
		supps[SuppItem::Extra1] = "players.mni";
	} else if (filename.compare("cartinfo.mni") == 0) {
		supps[SuppItem::Extra1] = "cartoon.mni";
	} else {
		supps[SuppItem::Extra1] = "actors.mni";
	}
	return supps;
}

Tileset_ActrInfo::Tileset_ActrInfo(std::unique_ptr<stream::inout> contentTileset,
	std::unique_ptr<stream::inout> contentFAT, std::shared_ptr<const Palette> pal)
	:	tsFrames(
			std::make_shared<Tileset_ActrInfo_Frames>(
				std::move(contentTileset), std::move(contentFAT), pal
			)
		)
{
}

Tileset_ActrInfo::~Tileset_ActrInfo()
{
}

const Tileset_ActrInfo::FileVector& Tileset_ActrInfo::files() const
{
	return this->tsFrames->filesActors();
}

const Tileset_ActrInfo::FileHandle Tileset_ActrInfo::find(const std::string& strFilename) const
{
	return this->tsFrames->find(strFilename);
}

bool Tileset_ActrInfo::isValid(const FileHandle& id) const
{
	return this->tsFrames->isValid(id);
}

std::unique_ptr<stream::inout> Tileset_ActrInfo::open(const FileHandle& id, bool useFilter)
{
	return this->tsFrames->open(id, useFilter);
}

std::shared_ptr<gamearchive::Archive> Tileset_ActrInfo::openFolder(const FileHandle& id)
{
	return this->tsFrames->openFolder(id);
}

const Tileset_ActrInfo::FileHandle Tileset_ActrInfo::insert(
	const FileHandle& idBeforeThis, const std::string& strFilename,
	stream::len storedSize, std::string type, File::Attribute attr)
{
	return this->tsFrames->insertActor(idBeforeThis, strFilename, storedSize, type, attr);
}

void Tileset_ActrInfo::remove(const FileHandle& id)
{
	return this->tsFrames->removeActor(id);
}

void Tileset_ActrInfo::rename(const FileHandle& id,
	const std::string& strNewName)
{
	return this->tsFrames->rename(id, strNewName);
}

void Tileset_ActrInfo::move(const FileHandle& idBeforeThis,
	const FileHandle& id)
{
	auto fat = Tileset_FAT::FATEntry::cast(id);
	auto targetActor = this->tsFrames->insertActor(idBeforeThis,
		fat->strName, 0, fat->type, fat->fAttr);
	auto targetActorFAT = Tileset_FAT::FATEntry::cast(targetActor);

	auto sourceActor = this->tsFrames->openTileset(id);
	for (auto& i : sourceActor->files()) {
		auto fatI = Tileset_FAT::FATEntry::cast(i);
		auto newFrameId = this->tsFrames->insertFrame(targetActorFAT, nullptr,
			fatI->strName, fatI->storedSize, fatI->type, fatI->fAttr);
		auto newFrameContent = this->tsFrames->open(newFrameId, true);
		auto oldFrameContent = this->tsFrames->open(i, true);
		stream::copy(*newFrameContent, *oldFrameContent);
		newFrameContent->flush();
	}
	sourceActor = nullptr;

	this->tsFrames->removeActor(id);
	return;
}

void Tileset_ActrInfo::resize(const FileHandle& id,
	stream::len newStoredSize, stream::len newRealSize)
{
	return this->tsFrames->resize(id, newStoredSize, newRealSize);
}

void Tileset_ActrInfo::flush()
{
	return this->tsFrames->flush();
}


Tileset_ActrInfo::Caps Tileset_ActrInfo::caps() const
{
	return this->tsFrames->caps();
}

ColourDepth Tileset_ActrInfo::colourDepth() const
{
	return this->tsFrames->colourDepth();
}

unsigned int Tileset_ActrInfo::layoutWidth() const
{
	return this->tsFrames->layoutWidth();
}

std::unique_ptr<Image> Tileset_ActrInfo::openImage(const FileHandle& id)
{
	// TODO: Throw exception
	return this->tsFrames->openImage(id);
}

std::shared_ptr<Tileset> Tileset_ActrInfo::openTileset(const FileHandle& id)
{
	return this->tsFrames->openTileset(id);
}

const Tileset_ActrInfo::FileHandle Tileset_ActrInfo::insert(
	const FileHandle& idBeforeThis, File::Attribute attr)
{
	return this->tsFrames->Tileset::insert(idBeforeThis, attr);
}

//
// Tileset_ActrInfo_Frames
//

Tileset_ActrInfo_Frames::Tileset_ActrInfo_Frames(std::unique_ptr<stream::inout> contentTileset,
	std::unique_ptr<stream::inout> contentFAT, std::shared_ptr<const Palette> pal)
	:	Tileset_FAT(std::move(contentTileset), ACTR_FIRST_TILE_OFFSET, ARCH_NO_FILENAMES),
		contentFAT(std::make_shared<stream::seg>(std::move(contentFAT))),
		currentDims{2, 1} // This should match the dimensions in test-tls-actrinfo
{
	this->pal = pal;

	auto lenContent = this->contentFAT->size();
	if (lenContent > 2) {
		this->contentFAT->seekg(0, stream::start);
		stream::pos nextOffset;
		*this->contentFAT >> u16le(nextOffset);

		unsigned int numActors = nextOffset;

		nextOffset *= 2;

		if (lenContent < nextOffset) {
			throw camoto::error(createString(
				"Actor info FAT truncated (first actor frame at offset " << nextOffset
				<< " is past EOF at " << lenContent << ")"
			));
		}

		stream::pos firstFrameOffset = nextOffset;
		if (firstFrameOffset < 2) {
			throw camoto::error("Actor info FAT is corrupted (first actor frame is "
				"within FAT itself)");
		}

		// Load all the actors
		this->vcActors.reserve(numActors);
		unsigned long totalFrames = 0;
		for (unsigned int i = 0; i < numActors; i++) {
			auto prevOffset = nextOffset;
			if (i == numActors - 1) {
				nextOffset = lenContent;
			} else {
				*this->contentFAT >> u16le(nextOffset);
				nextOffset *= 2;
			}

			auto fat = std::make_unique<FATEntry>();
			fat->bValid = true;
			fat->fAttr = File::Attribute::Folder;
			fat->iIndex = i;
			fat->lenHeader = 0;
			fat->iOffset = prevOffset;
			fat->storedSize = fat->realSize = nextOffset - prevOffset;
			this->vcActors.push_back(std::move(fat));
		}

		assert(this->contentFAT->tellg() == firstFrameOffset);

		// Load all the frames
		this->vcFAT.reserve(totalFrames);
		auto lenRemaining = lenContent - firstFrameOffset;
		unsigned int i = 0;
		while (lenRemaining >= ACTOR_FRAME_SIZE) {
			auto fat = std::make_unique<FATEntry>();
			fat->bValid = true;
			fat->fAttr = File::Attribute::Default;
			fat->iIndex = i++;
			fat->lenHeader = 0;
			Point dims;
			*this->contentFAT
				>> u16le(dims.y)
				>> u16le(dims.x)
				>> u32le(fat->iOffset)
			;
			lenRemaining -= ACTOR_FRAME_SIZE;
			fat->storedSize = fat->realSize = dims.x * dims.y * ACTR_TILE_SIZE_BYTES;
			this->vcFAT.push_back(std::move(fat));
		}
	}
}

Tileset_ActrInfo_Frames::~Tileset_ActrInfo_Frames()
{
}

std::unique_ptr<stream::inout> Tileset_ActrInfo_Frames::open(const FileHandle& id, bool useFilter)
{
	auto fat = FATEntry::cast(id);
	if (fat->fAttr & File::Attribute::Folder) {
		// This is an actor, so open the actor struct.  This is primarily just for
		// the test code so it can check what happens when adding/removing whole
		// actors.  Technically this function should fail if it's called for a
		// folder/tileset!
		auto raw = std::make_unique<gamearchive::archfile>(
			this->shared_from_this(),
			id,
			this->contentFAT
		);

		if (useFilter && !id->filter.empty()) {
			return applyFilter(
				std::move(raw),
				id->filter
			);
		}

		return std::move(raw);
	}
	// else this is a frame
	return this->Tileset_FAT::open(id, useFilter);
}

void Tileset_ActrInfo_Frames::flush()
{
	this->contentFAT->flush();
	this->Tileset_FAT::flush();
	return;
}

void Tileset_ActrInfo_Frames::updateFileOffset(const FATEntry *pid, stream::delta offDelta)
{
	unsigned int offset = pid->iOffset;
	this->contentFAT->seekp(
		this->getFirstFrameOffset()
		+ pid->iIndex * ACTOR_FRAME_SIZE
		+ 4,
		stream::start);
	*this->contentFAT << u32le(offset);
	return;
}

void Tileset_ActrInfo_Frames::updateFileSize(const FATEntry *pid, stream::delta sizeDelta)
{
	// Read the current size, assume these values have been updated *before* this
	// function has been called.
	this->contentFAT->seekp(
		this->getFirstFrameOffset()
		+ pid->iIndex * ACTOR_FRAME_SIZE,
		stream::start);
	Point dims;
	*this->contentFAT
		>> u16le(dims.y)
		>> u16le(dims.x)
	;

	if (pid->storedSize != (unsigned long)(dims.x * dims.y * ACTR_TILE_SIZE_BYTES)) {
		throw stream::error("Can only resize an actrinfo frame in multiples of whole tiles.");
	}
	// Don't need to write anything, the caller has done that already.
	/*
	*this->contentFAT
		<< u16le(dims.y)
		<< u16le(dims.x)
	;
	*/
	return;
}

void Tileset_ActrInfo_Frames::preInsertFile(const FATEntry *idBeforeThis,
	FATEntry *pNewEntry)
{
	// Make sure the caller has told us which actor we're working on
	assert(this->currentActor >= 0);

	// Make sure the new file size matches the dimensions we've been told to write
	assert(pNewEntry->storedSize == (unsigned long)(
		this->currentDims.x * this->currentDims.y * ACTR_TILE_SIZE_BYTES));

	stream::pos offNew = this->getFirstFrameOffset();
	if (idBeforeThis) {
		offNew += idBeforeThis->iIndex * ACTOR_FRAME_SIZE;
	} else {
		offNew += this->vcFAT.size() * ACTOR_FRAME_SIZE;
	}

	this->contentFAT->seekp(offNew, stream::start);
	this->contentFAT->insert(ACTOR_FRAME_SIZE);

	unsigned int offset = pNewEntry->iOffset;
	*this->contentFAT
		<< u16le(this->currentDims.y)
		<< u16le(this->currentDims.x)
		<< u32le(offset)
	;

	// Find the actor we are editing
	auto fatActor = FATEntry::cast(this->vcActors[this->currentActor]);
	assert(fatActor);
	fatActor->storedSize += ACTOR_FRAME_SIZE;
	fatActor->realSize = fatActor->storedSize;

	// Update actor structures now this one is one frame larger
	this->contentFAT->seekp(ACTR_FIRST_TILE_OFFSET, stream::start);
	for (auto& i : this->vcActors) {
		auto fatI = FATEntry::cast(i);
		// Don't change offset of actor we are editing
		if (fatI->iIndex == fatActor->iIndex) continue;

		// Increase offsets of all actors following the one we are editing
		if (fatI->iOffset >= fatActor->iOffset) {
			fatI->iOffset += ACTOR_FRAME_SIZE;

			auto offNew = fatI->iOffset;
			offNew /= 2;
			this->contentFAT->seekp(ACTRINFO_FATENTRY_OFFSET(fatI), stream::start);
			*this->contentFAT << u16le(offNew);
		}
	}
	return;
}

void Tileset_ActrInfo_Frames::preRemoveFile(const FATEntry *pid)
{
	// pid is the FATEntry of the frame to be removed
	assert(this->currentActor >= 0);
	assert(this->vcActors.size() > (unsigned long)this->currentActor);
	auto fatActor = FATEntry::cast(this->vcActors[this->currentActor]);

	// Update the actor structs
	auto targetOffsetEnd = fatActor->iOffset + fatActor->storedSize;
	for (auto& i : this->vcActors) {
		auto fat = FATEntry::cast(i);
		if (fat->iOffset >= targetOffsetEnd) {
			assert(fat->iOffset >= ACTOR_FRAME_SIZE);
			fat->iOffset -= ACTOR_FRAME_SIZE;
			// write new value
			this->contentFAT->seekp(ACTRINFO_FATENTRY_OFFSET(fat), stream::start);
			auto offNew = fat->iOffset;
			offNew /= 2;
			*this->contentFAT << u16le(offNew);
		}
	}

	assert(fatActor->storedSize >= ACTOR_FRAME_SIZE);
	this->contentFAT->seekp(
		this->getFirstFrameOffset() + pid->iIndex * ACTOR_FRAME_SIZE,
		stream::start);
	this->contentFAT->remove(ACTOR_FRAME_SIZE);

	fatActor->storedSize -= ACTOR_FRAME_SIZE;
	fatActor->realSize = fatActor->storedSize;
	return;
}

Tileset::Caps Tileset_ActrInfo_Frames::caps() const
{
	return (this->pal ? Caps::HasPalette : Caps::Default);
}

ColourDepth Tileset_ActrInfo_Frames::colourDepth() const
{
	return ColourDepth::EGA;
}

unsigned int Tileset_ActrInfo_Frames::layoutWidth() const
{
	return 1;
}

std::shared_ptr<Tileset> Tileset_ActrInfo_Frames::openTileset(const FileHandle& id)
{
	auto shared_this = std::dynamic_pointer_cast<Tileset_ActrInfo_Frames>(
		this->shared_from_this()
	);
	assert(shared_this);
	auto fatActor = std::dynamic_pointer_cast<const FATEntry>(id);
	return std::make_shared<Tileset_SingleActor>(shared_this, fatActor);
}

std::unique_ptr<Image> Tileset_ActrInfo_Frames::openImage(const FileHandle& id)
{
	auto fat = FATEntry::cast(id);
	assert(fat);

	this->contentFAT->seekg(
		this->getFirstFrameOffset() + fat->iIndex * ACTOR_FRAME_SIZE,
		stream::start
	);
	Point dims;
	*this->contentFAT
		>> u16le(dims.y)
		>> u16le(dims.x)
	;
	return std::make_unique<Image_FromTileset>(
		std::make_shared<Tileset_EGAApogee>( // must be shared_ptr so shared_from_this works
			this->open(id, true),
			Point{ACTR_TILE_WIDTH, ACTR_TILE_HEIGHT},
			ACTR_NUMPLANES, 1, nullptr
		),
		0, dims.x, dims
	);
}

const Tileset_ActrInfo_Frames::FileVector& Tileset_ActrInfo_Frames::filesActors() const
{
	return this->vcActors;
}

stream::len Tileset_ActrInfo_Frames::getFirstFrameOffset() const
{
	return this->vcActors.size() * ACTRINFO_FAT_ENTRY_LEN;
}

const Tileset_ActrInfo::FileHandle Tileset_ActrInfo_Frames::insertActor(
	const FileHandle& idBeforeThis, const std::string& strFilename,
	stream::len storedSize, std::string type, File::Attribute attr)
{
	if (storedSize) {
		throw camoto::error("Cannot insert an actor while supplying data.");
	}
	if (!(attr & File::Attribute::Folder)) {
		throw camoto::error("Can only create folders/tilesets in top-level tls-actrinfo.");
	}

	auto fat = std::make_shared<FATEntry>();
	fat->bValid = true;
	fat->type = type;
	fat->fAttr = attr;
	fat->lenHeader = 0;
	fat->storedSize = storedSize;
	if (idBeforeThis) {
		auto fatBefore = FATEntry::cast(idBeforeThis);
		assert(fatBefore->fAttr & File::Attribute::Folder);

		fat->iOffset = fatBefore->iOffset;
		fat->iIndex = fatBefore->iIndex;

		for (auto& i : this->vcActors) {
			auto fatI = FATEntry::cast(i);
			// Increment the index of idBefore this and all following actors
			if (fatI->iIndex >= fat->iIndex) {
				fatI->iIndex++;
			}
			// Increment the offset of idBefore this and all following actors to make
			// room for the new actor's data, if any.
			if (fatI->iOffset >= fat->iOffset) {
				fatI->iOffset += fat->storedSize;
			}
		}

		this->vcActors.insert(this->vcActors.begin() + fat->iIndex, fat);
	} else {
		if (this->vcActors.empty()) {
			// empty archive
			fat->iOffset = ACTR_FIRST_TILE_OFFSET;
			fat->iIndex = 0;
		} else {
			auto idLast = this->vcActors.back();
			auto fatLast = FATEntry::cast(idLast);
			assert(fatLast);
			fat->iOffset = fatLast->iOffset + fatLast->storedSize;
			fat->iIndex = fatLast->iIndex + 1;
		}
		this->vcActors.push_back(fat);
	}

	// Insert the new actor entry into the file now it's in vcActors
	this->contentFAT->seekp(ACTRINFO_FATENTRY_OFFSET(fat), stream::start);
	this->contentFAT->insert(ACTRINFO_FAT_ENTRY_LEN);

	// Increment each actor's first frame offset by the new actor entry, and write
	// the offset to the file.  This will write the offset for the new entry into
	// the space we just inserted, too.
	this->contentFAT->seekp(ACTR_FIRST_TILE_OFFSET, stream::start);
	for (auto& i : this->vcActors) {
		auto fatI = FATEntry::cast(i);
		fatI->iOffset += ACTRINFO_FAT_ENTRY_LEN;

		auto offNew = fatI->iOffset;
		offNew /= 2;
		*this->contentFAT << u16le(offNew);
	}
	return fat;
}

const Tileset_ActrInfo_Frames::FileHandle Tileset_ActrInfo_Frames::insertFrame(
	const FATEntry* fatActor, const FileHandle& idBeforeThis,
	const std::string& strFilename, stream::len storedSize, std::string type,
	File::Attribute attr)
{
	if (attr & File::Attribute::Folder) {
		throw stream::error("Cannot create folders/tilesets in tls-actrinfo subtilesets.");
	}

	FileHandle frameBefore = nullptr;
	if (idBeforeThis) {
		frameBefore = idBeforeThis;
	} else {
		// Find the frame after this actor's last frame
		unsigned long firstActorFrame =
			(fatActor->iOffset - this->getFirstFrameOffset()) / ACTOR_FRAME_SIZE;
		unsigned long lastActorFrame =
			firstActorFrame + (fatActor->storedSize / ACTOR_FRAME_SIZE);
		for (auto& i : this->vcFAT) {
			auto fat = FATEntry::cast(i);
			if (fat->iIndex == lastActorFrame) {
				frameBefore = i;
				break;
			}
		}
		// If there are no frames after this actor's last frame, frameBefore will be
		// null, so the frame will be appended to the end, which is what we want.
	}

	// This isn't thread-safe, but then I guess the whole class isn't, really...
	this->currentActor = fatActor->iIndex;

	return this->Archive_FAT::insert(frameBefore, strFilename, storedSize, type,
		attr);
}

void Tileset_ActrInfo_Frames::removeActor(const FileHandle& id)
{
	auto fat = FATEntry::cast(id);

	// Remove all the actor's frames
	unsigned long firstFrame = (fat->iOffset - this->getFirstFrameOffset())
		/ ACTOR_FRAME_SIZE;
	unsigned long numFrames = fat->realSize / ACTOR_FRAME_SIZE;
	this->currentActor = fat->iIndex;
	while (numFrames--) {
		this->remove(this->vcFAT[firstFrame]);
	}
	this->currentActor = -1;

	// Remove the actor's FAT entry
	this->contentFAT->seekp(ACTRINFO_FATENTRY_OFFSET(fat), stream::start);
	this->contentFAT->remove(ACTRINFO_FAT_ENTRY_LEN);

	// Mark it as invalid in case some other code is still holding on to it.
	fat->bValid = false;

	auto removedIndex = fat->iIndex;

	// Remove the actor itself
	//@todo - call some updateOffsets function just for the actors
	auto itErase = std::find(this->vcActors.begin(), this->vcActors.end(), id);
	assert(itErase != this->vcActors.end());
	this->vcActors.erase(itErase);
	// Can't use this pointer any more, now the vector erase() has deleted the
	// underlying data.
	fat = nullptr;

	// Decrement each actor's first frame offset, and write the offset to the
	// file.
	this->contentFAT->seekp(ACTR_FIRST_TILE_OFFSET, stream::start);
	for (auto& i : this->vcActors) {
		auto fatI = FATEntry::cast(i);
		fatI->iOffset -= ACTRINFO_FAT_ENTRY_LEN;

		// Update the index while we're here
		if (fatI->iIndex >= removedIndex) fatI->iIndex--;

		auto offNew = fatI->iOffset;
		offNew /= 2;
		*this->contentFAT << u16le(offNew);
	}

	return;
}

//
// Tileset_SingleActor
//

Tileset_SingleActor::Tileset_SingleActor(
	std::shared_ptr<Tileset_ActrInfo_Frames> actorInfo,
	std::shared_ptr<const Tileset_FAT::FATEntry> fatActor)
	:	actorInfo(actorInfo),
		fatActor(fatActor)
{
}

Tileset_SingleActor::~Tileset_SingleActor()
{
}

const Tileset_SingleActor::FileVector& Tileset_SingleActor::files() const
{
	// Update the vector and return the new one
	this->vcFrames.clear();

	unsigned long firstFrame = (this->fatActor->iOffset - this->actorInfo->getFirstFrameOffset())
		/ ACTOR_FRAME_SIZE;
	unsigned long numFrames = this->fatActor->realSize / ACTOR_FRAME_SIZE;
	auto allFrames = this->actorInfo->files();
	if (firstFrame < allFrames.size()) {
		this->vcFrames.insert(
			this->vcFrames.end(),
			allFrames.begin() + firstFrame,
			// std::min limits to last frame if actor count wants to go past
			allFrames.begin() + std::min(firstFrame + numFrames, (unsigned long)allFrames.size())
		);
	} // else first tile is beyond last frame, return blank actor

	return this->vcFrames;
}

const Tileset_SingleActor::FileHandle Tileset_SingleActor::find(
	const std::string& strFilename) const
{
	// No filenames
	return {};
}

bool Tileset_SingleActor::isValid(const FileHandle& id) const
{
	return this->actorInfo->isValid(id);
}

std::unique_ptr<stream::inout> Tileset_SingleActor::open(const FileHandle& id,
	bool useFilter)
{
	return this->actorInfo->open(id, useFilter);
}

std::shared_ptr<Tileset_SingleActor::Archive> Tileset_SingleActor::openFolder(
	const FileHandle& id)
{
	assert(false);
	throw stream::error("Tried to open an actor frame as a tileset, ignoring "
		"FileHandle attributes!");
}

const Tileset_SingleActor::FileHandle Tileset_SingleActor::insert(
	const FileHandle& idBeforeThis, const std::string& strFilename,
	stream::len storedSize, std::string type, File::Attribute attr)
{
	return this->actorInfo->insertFrame(this->fatActor.get(), idBeforeThis,
		strFilename, storedSize, type, attr);
}

void Tileset_SingleActor::remove(const FileHandle& id)
{
	this->actorInfo->currentActor = this->fatActor->iIndex;
	this->actorInfo->remove(id);
	this->actorInfo->currentActor = -1;
	return;
}

void Tileset_SingleActor::rename(const FileHandle& id, const std::string& strNewName)
{
	throw stream::error("Unable to rename - Cosmo actor frames have no filenames!");
}

void Tileset_SingleActor::move(const FileHandle& idBeforeThis, const FileHandle& id)
{
	this->actorInfo->currentActor = this->fatActor->iIndex;
	this->actorInfo->move(idBeforeThis, id);
	this->actorInfo->currentActor = -1;
	return;
}

void Tileset_SingleActor::resize(const FileHandle& id, stream::len newStoredSize,
	stream::len newRealSize)
{
	return;
}

void Tileset_SingleActor::flush()
{
	this->actorInfo->flush();
	return;
}

Tileset::Caps Tileset_SingleActor::caps() const
{
	return Caps::Default
		| (this->pal ? Caps::HasPalette : Caps::Default);
}

ColourDepth Tileset_SingleActor::colourDepth() const
{
	return ColourDepth::EGA;
}

unsigned int Tileset_SingleActor::layoutWidth() const
{
	return 1;
}

std::unique_ptr<Image> Tileset_SingleActor::openImage(const FileHandle& id)
{
	return this->actorInfo->openImage(id);
}

std::shared_ptr<Tileset> Tileset_SingleActor::openTileset(const FileHandle& id)
{
	throw stream::error("This format does not contain any tilesets!");
}

} // namespace gamegraphics
} // namespace camoto
