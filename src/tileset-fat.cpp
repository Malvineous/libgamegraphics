/**
 * @file   tileset-fat.cpp
 * @brief  Generic FAT-based tileset handler.
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

#include <boost/bind.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

FATTileset::FATEntry::FATEntry()
{
}

FATTileset::FATEntry::~FATEntry()
{
}

FATTileset::FATTileset(stream::inout_sptr data,
	stream::pos offFirstTile)
	throw (stream::error) :
		data(new stream::seg()),
		offFirstTile(offFirstTile)
{
	this->data->open(data);
}

FATTileset::~FATTileset()
	throw ()
{
}

const FATTileset::VC_ENTRYPTR& FATTileset::getItems(void) const
	throw ()
{
	return this->items;
}

TilesetPtr FATTileset::openTileset(const EntryPtr& id)
	throw (stream::error)
{
	stream::inout_sptr sub = this->openStream(id);
	return this->createTilesetInstance(id, sub);
}

ImagePtr FATTileset::openImage(const EntryPtr& id)
	throw (stream::error)
{
	stream::inout_sptr sub = this->openStream(id);
	/// @todo What happens if the entry gets deleted, then the image resized?
	///   Will id still point to a valid entry?
	return this->createImageInstance(id, sub);
}

FATTileset::EntryPtr FATTileset::insert(const EntryPtr& idBeforeThis, int attr)
	throw (stream::error)
{
	FATEntry *pNewFile = this->createNewFATEntry();
	EntryPtr ep(pNewFile);

	pNewFile->isValid = true;
	pNewFile->attr = attr;
	pNewFile->size = 0;
	pNewFile->lenHeader = 0;

	// Figure out where the new file is going to go
	const FATEntry *pFATBeforeThis = NULL;
	if (idBeforeThis && idBeforeThis->isValid) {
		// Insert at idBeforeThis
		// TESTED BY: fmt_grp_duke3d_insert_mid
		pFATBeforeThis = dynamic_cast<const FATEntry *>(idBeforeThis.get());
		assert(pFATBeforeThis);
		pNewFile->offset = pFATBeforeThis->offset;
		pNewFile->index = pFATBeforeThis->index;
	} else {
		// Append to end of archive
		// TESTED BY: fmt_grp_duke3d_insert_end
		if (this->items.size()) {
			const FATEntry *pFATAfterThis =
				dynamic_cast<const FATEntry *>(this->items.back().get());
			assert(pFATAfterThis);
			pNewFile->offset = pFATAfterThis->offset
				+ pFATAfterThis->lenHeader + pFATAfterThis->size;
			pNewFile->index = pFATAfterThis->index + 1;
		} else {
			// There are no files in the archive
			pNewFile->offset = this->offFirstTile;
			pNewFile->index = 0;
		}
	}

	// Add the file's entry from the FAT.  May throw (e.g. filename too long),
	// archive should be left untouched in this case.
	FATEntry *returned = this->preInsertFile(pFATBeforeThis, pNewFile);
	if (returned != pNewFile) {
		ep.reset(returned);
		pNewFile = returned;
	}

	if (idBeforeThis && idBeforeThis->isValid) {
		// Update the offsets of any files located after this one (since they will
		// all have been shifted forward to make room for the insert.)
		this->shiftFiles(
			pNewFile,
			pNewFile->offset + pNewFile->lenHeader,
			pNewFile->size,
			1
		);

		// Add the new file to the vector now all the existing offsets have been
		// updated.
		// TESTED BY: fmt_grp_duke3d_insert_mid
		VC_ENTRYPTR::iterator itBeforeThis = std::find(this->items.begin(),
			this->items.end(), idBeforeThis);
		assert(itBeforeThis != this->items.end());
		this->items.insert(itBeforeThis, ep);
	} else {
		// TESTED BY: fmt_grp_duke3d_insert_end
		this->items.push_back(ep);
	}

	// Insert space for the file's data into the archive.  If there is a header
	// (e.g. embedded FAT) then preInsertFile() will have inserted space for
	// this and written the data, so our insert should start just after the
	// header.
	this->data->seekp(pNewFile->offset + pNewFile->lenHeader, stream::start);
	this->data->insert(pNewFile->size);

	this->postInsertFile(pNewFile);

	return ep;
}

void FATTileset::remove(EntryPtr& id)
	throw (stream::error)
{
	// Make sure the caller doesn't try to remove something that doesn't exist!
	assert(id->isValid);

	FATEntry *pFATDel = dynamic_cast<FATEntry *>(id.get());
	assert(pFATDel);

	// Remove the file's entry from the FAT
	this->preRemoveFile(pFATDel);

	// Remove the entry from the vector
	VC_ENTRYPTR::iterator itErase = std::find(this->items.begin(),
		this->items.end(), id);
	assert(itErase != this->items.end());
	this->items.erase(itErase);

	// Update the offsets of any files located after this one (since they will
	// all have been shifted back to fill the gap made by the removal.)
	this->shiftFiles(
		pFATDel,
		pFATDel->offset,
		-(pFATDel->size + pFATDel->lenHeader),
		-1
	);

	// Remove the file's data from the archive
	this->data->seekp(pFATDel->offset, stream::start);
	this->data->remove(pFATDel->size + pFATDel->lenHeader);

	// Mark it as invalid in case some other code is still holding on to it.
	pFATDel->isValid = false;

	this->postRemoveFile(pFATDel);

	return;
}

void FATTileset::resize(EntryPtr& id, stream::len newSize)
	throw (stream::error)
{
	FATEntry *pFAT = dynamic_cast<FATEntry *>(id.get());
	assert(pFAT);
	stream::delta delta = newSize - pFAT->size;

	// Add or remove the data in the underlying stream
	stream::pos start;
	if (delta > 0) { // inserting data
		// TESTED BY: tls_zone66_insert*
		start = pFAT->offset + pFAT->lenHeader + pFAT->size;
		this->data->seekp(start, stream::start);
		this->data->insert(delta);
	} else if (delta < 0) { // removing data
		// TESTED BY: TODO
		start = pFAT->offset + pFAT->lenHeader + newSize;
		this->data->seekp(start, stream::start);
		this->data->remove(-delta);
	} else {
		return; // no change
	}
	pFAT->size = newSize;

	// Update the FAT with the file's new size
	this->updateFileSize(pFAT, delta);

	// Adjust the in-memory offsets etc. of the rest of the files in the archive,
	// including any open streams.
	this->shiftFiles(pFAT, start, delta, 0);

	// Resize any open stream::subs
	bool clean = false;
	for (OPEN_ITEMS::iterator i = this->openItems.begin();
		i != this->openItems.end();
		i++
	) {
		if (i->first.get() == pFAT) {
			if (stream::sub_sptr sub = i->second.lock()) {
				sub->resize(newSize);
				// no break, could be multiple opens for same entry
			} else clean = true;
		}
	}

	// If one stream::sub has closed, clean up
	if (clean) this->cleanOpenSubstreams();

	return;
}

void FATTileset::flush()
	throw (stream::error)
{
	// Write out to the underlying stream
	this->data->flush();

	return;
}

void FATTileset::shiftFiles(const FATEntry *fatSkip, stream::pos offStart,
	stream::len deltaOffset, int deltaIndex)
	throw (stream::error)
{
	for (VC_ENTRYPTR::iterator i = this->items.begin(); i != this->items.end(); i++) {
		FATEntry *pFAT = dynamic_cast<FATEntry *>(i->get());
		if (this->entryInRange(pFAT, offStart, fatSkip)) {
			// This file is located after the one we're deleting, so tweak its offset
			pFAT->offset += deltaOffset;

			// We have to update the index first, as this is used when inserting
			// files, and it's called *after* the FAT has been updated on-disk.  So
			// the index needs to be adjusted before any further on-disk updates to
			// ensure the right place in the file gets changed.
			pFAT->index += deltaIndex;

			this->updateFileOffset(pFAT, deltaOffset);
		}
	}

	// Relocate any open stream::subs
	bool clean = false;
	for (OPEN_ITEMS::iterator i = this->openItems.begin();
		i != this->openItems.end();
		i++
	) {
		if (i->first->isValid) {
			if (this->entryInRange(i->first.get(), offStart, fatSkip)) {
				if (stream::sub_sptr sub = i->second.lock()) {
					sub->relocate(deltaOffset);
				}
			}
		} else clean = true;
	}

	// If one stream::sub has closed, clean up
	if (clean) this->cleanOpenSubstreams();

	return;
}

TilesetPtr FATTileset::createTilesetInstance(const EntryPtr& id,
	stream::inout_sptr content)
	throw (stream::error)
{
	// Caller didn't check id->attr
	assert(false);
	throw stream::error("this tileset format doesn't have any sub-tilesets"
		" (this is a bug - the caller should have checked the EntryPtr's"
		" attributes to detect this)");
}

ImagePtr FATTileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
	throw (stream::error)
{
	// Caller didn't check id->attr
	assert(false);
	throw stream::error("this tileset format doesn't have any sub-tilesets"
		" (this is a bug - the caller should have checked the EntryPtr's"
		" attributes to detect this)");
}

void FATTileset::updateFileOffset(const FATTileset::FATEntry *pid,
	stream::len offDelta)
	throw (stream::error)
{
	// Default implementation is a no-op
	return;
}

/// Adjust the size of the given file in the on-disk FAT.
void FATTileset::updateFileSize(const FATTileset::FATEntry *pid,
	stream::len sizeDelta)
	throw (stream::error)
{
	// Default implementation is a no-op
	return;
}

FATTileset::FATEntry *FATTileset::preInsertFile(
	const FATTileset::FATEntry *idBeforeThis, FATTileset::FATEntry *pNewEntry)
	throw (stream::error)
{
	// Default implementation is a no-op
	return pNewEntry;
}

void FATTileset::postInsertFile(FATTileset::FATEntry *pNewEntry)
	throw (stream::error)
{
	// Default implementation is a no-op
	return;
}

void FATTileset::preRemoveFile(const FATTileset::FATEntry *pid)
	throw (stream::error)
{
	// Default implementation is a no-op
	return;
}

void FATTileset::postRemoveFile(const FATTileset::FATEntry *pid)
	throw (stream::error)
{
	// Default implementation is a no-op
	return;
}

FATTileset::FATEntry *FATTileset::createNewFATEntry()
	throw ()
{
	return new FATEntry();
}

stream::inout_sptr FATTileset::openStream(const EntryPtr& id)
	throw ()
{
	assert(id->isValid);

	// We are casting away const here, but that's because we need to maintain
	// access to the EntryPtr, which we may need to "change" later (to update the
	// offset if another file gets inserted before it, i.e. any change would not
	// be visible externally.)
	FATEntryPtr pFAT = boost::dynamic_pointer_cast<FATEntry>(id);
	assert(pFAT);

	stream::fn_truncate fnTruncate = boost::bind<void>(&FATTileset::resize, this, id, _1);

	stream::sub_sptr sub(new stream::sub);
	sub->open(
		this->data,
		pFAT->offset + pFAT->lenHeader,
		pFAT->size,
		fnTruncate
	);

	// Add it to the list of open files, in case we need to shift the stream::sub
	// around later on as files are added/removed/resized.
	this->openItems.insert(OPEN_ITEM(
		pFAT,
		sub
	));
	return sub;
}

void FATTileset::cleanOpenSubstreams()
	throw ()
{
	bool clean;
	do {
		clean = true;
		// Run through the list looking for the first expired item
		for (OPEN_ITEMS::iterator i = this->openItems.begin();
			i != this->openItems.end();
			i++
		) {
			if (i->second.expired()) {
				// Found one so remove it and restart the search (since removing an
				// item invalidates the iterator.)
				this->openItems.erase(i);
				clean = false;
				break;
			};
		}
	} while (!clean);

	return;
}

bool FATTileset::entryInRange(const FATEntry *fat, stream::pos offStart,
	const FATEntry *fatSkip) const
	throw ()
{
	// Don't move any files earlier than the start of the shift block.
	if (fat->offset < offStart) return false;

	// If we have a valid item to skip (an invalid item is given during insert,
	// before the skip item has been fully inserted.)
	if ((fatSkip) && (fatSkip->isValid)) {

		// Don't move the item we're skipping.
		if (fat == fatSkip) return false;
		// Can't use index for comparison here as this function is called during
		// file insertion, and then we have two files with the exact same index and
		// offset.
		// if (fat->iIndex == fatSkip->iIndex) return false;

		if (
			// If it's a zero-length file...
			(fat->size == 0)
			// ...starting at the same location as the skip file...
			&& (fat->offset == fatSkip->offset)
			// ...but appearing before it in the index order...
			&& (fat->index < fatSkip->index)
		) {
			// ...then don't move it.
			return false;
		}
	}

	return true;
}

} // namespace gamearchive
} // namespace camoto
