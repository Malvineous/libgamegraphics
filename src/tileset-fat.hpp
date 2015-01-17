/**
 * @file   tileset-fat.hpp
 * @brief  Generic FAT-based tileset handler.
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

#ifndef _CAMOTO_TILESET_FAT_HPP_
#define _CAMOTO_TILESET_FAT_HPP_

#include <map>
#include <boost/iostreams/stream.hpp>
#include <boost/weak_ptr.hpp>
#include "basetileset.hpp"
#include <camoto/stream_sub.hpp>
#include <camoto/stream_seg.hpp>
#include <camoto/stream_sub.hpp>

namespace camoto {
namespace gamegraphics {

/// Tileset implementation for tilesets with an associated size/offset table.
class FATTileset: virtual public BaseTileset
{
	public:
		/// FAT-related fields to add to EntryPtr.
		/**
		 * This shouldn't really be public, but sometimes it is handy to access the
		 * FAT fields (especially from within the unit tests.)
		 */
		struct FATEntry: virtual public BaseTilesetEntry {
			/// Index of item in the tileset.
			/**
			 * We can't use the index into the vector as entries are passed around
			 * outside the vector.
			 */
			unsigned int index;

			stream::pos offset;    ///< Offset of embedded FAT (if any) followed by content
			stream::pos size;      ///< Size of content, not including embedded FAT
			stream::pos lenHeader; ///< Size of embedded FAT
		};

		/// Shared pointer of FAT-specific file entry.
		typedef boost::shared_ptr<FATEntry> FATEntryPtr;

	protected:
		mutable stream::seg_sptr data;

		/// Offset of the first tile in an empty archive.
		uint8_t offFirstTile;

		/// Vector of all items in the tileset.
		/**
		 * Although we have a specific FAT type for each entry we can't use a
		 * vector of them here because getFileList() must return a vector of the
		 * base type.  So instead each FAT entry type inherits from the base type
		 * so that the specific FAT entry types can still be added to this vector.
		 *
		 * The entries in this vector can be in any order (not necessarily the
		 * order on-disk.  Use the iIndex member for that.)
		 */
		VC_ENTRYPTR items;

		/// Vector of stream::sub references.
		/**
		 * These are weak pointers so that we don't hold a file open simply because
		 * we're keeping track of it.  We need to keep track of it so that open
		 * files can be moved around as other files are inserted, resized, etc.
		 */
		typedef std::multimap< FATEntryPtr, boost::weak_ptr<stream::sub> > OPEN_ITEMS;

		/// Helper type when inserting elements into openFiles.
		typedef std::pair< FATEntryPtr, boost::weak_ptr<stream::sub> > OPEN_ITEM;

		/// List of stream::subs currently open.
		OPEN_ITEMS openItems;

	public:

		FATTileset(stream::inout_sptr data,
			stream::pos offFirstTile);

		virtual ~FATTileset();

		virtual const VC_ENTRYPTR& getItems(void) const;

		virtual TilesetPtr openTileset(const EntryPtr& id);

		virtual ImagePtr openImage(const EntryPtr& id);

		virtual EntryPtr insert(const EntryPtr& idBeforeThis, int attr);

		virtual void remove(EntryPtr& id);

		virtual void resize(EntryPtr& id, stream::len newSize);

		virtual void flush();

		/// Shift any files *starting* at or after offStart by delta bytes.
		/**
		 * This updates the internal offsets and index numbers.  The FAT is updated
		 * by calling updateFileOffset().  If offStart is in the middle of a file
		 * (which should never happen) that file won't be affected, only those
		 * following it.  This function must notify any open files that their offset
		 * has moved.
		 */
		virtual void shiftFiles(const FATEntry *fatSkip, stream::pos offStart,
			stream::delta deltaOffset, int deltaIndex);

		// Methods to be filled out by descendent classes

		/// Create a Tileset instance for the given item.
		/**
		 * @param id
		 *   Item to open.
		 *
		 * @param content
		 *   Substream containing the item's data.
		 */
		virtual TilesetPtr createTilesetInstance(const EntryPtr& id,
			stream::inout_sptr content);

		/// Create an Image instance for the given item.
		/**
		 * @param id
		 *   Item to open.
		 *
		 * @param content
		 *   Substream containing the item's data.
		 */
		virtual ImagePtr createImageInstance(const EntryPtr& id,
			stream::inout_sptr content);

		/// Adjust the offset of the given entry in the on-disk FAT.
		/**
		 * @param pid
		 *   The entry to update.  pid->offset is already set to the new offset.
		 *
		 * @param offDelta
		 *   Amount the offset has changed, in case this value is needed.
		 *
		 * @note pid->offset is already set to the new offset, do not add offDelta
		 *   to this or you will get the wrong offset!
		 */
		virtual void updateFileOffset(const FATEntry *pid, stream::len offDelta);

		/// Adjust the size of the given entry in the on-disk FAT.
		/**
		 * @param pid
		 *   The entry to update.  pid->size is already set to the new size.
		 *
		 * @param sizeDelta
		 *   Amount the size has changed, in case this value is needed.
		 *
		 * @note pid->size is already set to the new size, do not add sizeDelta
		 *   to this or you will get the wrong size!
		 */
		virtual void updateFileSize(const FATEntry *pid, stream::len sizeDelta);

		/// Insert a new entry in the on-disk FAT.
		/**
		 * It should be inserted before idBeforeThis, or at the end of the archive
		 * if idBeforeThis is not valid.  All the FAT entries will be updated with
		 * new offsets after this function returns (so this function *must* add a
		 * new entry into the on-disk FAT for this file) however the offsets will
		 * not take into account any changes resulting from the FAT changing size,
		 * which must be handled by this function.  The FAT vector does not contain
		 * the new entry, so pNewEntry->iIndex may be the same as an existing file
		 * (but the existing file will have its index moved after this function
		 * returns.)  All this function has to do is make room in the FAT and write
		 * out the new entry.  It also needs to set the lenHeader field in
		 * pNewEntry.  The returned pointer is the one that is used.  Normally
		 * pNewEntry will be returned, but if a FATEntry has been extended for a
		 * particular format, this is where the custom class should be created,
		 * have pNewEntry copied into it, then be returned.
		 * If pNewEntry->lenHeader is set to a non-zero value, this function is
		 * responsible for inserting that many bytes and writing the correct value
		 * to them.
		 *
		 * @note Invalidates existing EntryPtrs. TODO - does it?
		 *
		 * @param idBeforeThis
		 *   The new file is to be inserted before this.  If it is invalid the new
		 *   file should be appended to the end of the archive.
		 *
		 * @param pNewEntry
		 *   Initial details about the new entry.
		 *
		 * @return Real entry to use.  Will usually (but not always) be the same as
		 *   pNewEntry.
		 */
		virtual FATEntry *preInsertFile(const FATEntry *idBeforeThis,
			FATEntry *pNewEntry);

		/// Called after the file data has been inserted.
		/**
		 * Only needs to be overridden if there are tasks to perform after the file
		 * has been set.  pNewEntry can be changed if need be, but this is not
		 * required.
		 *
		 * @note preInsertFile() and all subsequent FAT updates and file shifting
		 * is done without the new file, then the new file data is insert last, and
		 * postInsertFile() immediately called.
		 */
		virtual void postInsertFile(FATEntry *pNewEntry);

		/// Remove the entry from the FAT.
		/**
		 * The file data has already been removed from the archive, but the offsets
		 * have not yet been updated.  On return, pid will be removed from the FAT
		 * vector and the on-disk offsets of files following this one will be
		 * updated (via calls to updateFileOffset()) - so they don't need changing
		 * here.  However the offsets will not take into account any changes
		 * resulting from the FAT changing size, which must be handled by this
		 * function.
		 *
		 * Invalidates existing EntryPtrs.
		 */
		virtual void preRemoveFile(const FATEntry *pid);

		/// Called after the file data has been removed and the FAT has been
		/// updated.
		/**
		 * Only override if needed.  Note that pid->bValid will be false (because
		 * the file has been removed) but for this function only, the other
		 * parameters are still correct, although no longer used (e.g. the offset
		 * it was at, its size, etc.)
		 */
		virtual void postRemoveFile(const FATEntry *pid);

		/// Allocate a new, empty FAT entry.
		/**
		 * This function creates a new FATEntry instance.  A default implementation
		 * is provided which creates a new FATEntry instance.  If you are
		 * implementing a new archive format and you need to extend FATEntry to hold
		 * additional information, you will need to replace this function with one
		 * that allocates your extended class instead, otherwise the EntryPtrs
		 * passed to the other functions will be a mixture of FATEntry and whatever
		 * your extended class is.  See fmt-epf-lionking.cpp for an example.
		 */
		virtual FATEntry *createNewFATEntry();

	private:

		/// Create a stream::sub containing the item's data.
		stream::inout_sptr openStream(const EntryPtr& id);

		/// Remove any stream::subs from the cached list if they have closed.
		void cleanOpenSubstreams();

		/// Should the given entry be moved during an insert/resize operation?
		bool entryInRange(const FATEntry *fat, stream::pos offStart,
			const FATEntry *fatSkip) const;

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TILESET_FAT_HPP_
