/**
 * @file   img-ccaves.hpp
 * @brief  Crystal Caves graphics format handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Crystal_Caves_Tileset_Format
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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

#include <iostream>
#include <exception>
#include <cstring>
#include <boost/shared_array.hpp>
#include <boost/iostreams/positioning.hpp>  // stream_offset

#include <camoto/debug.hpp>
#include <camoto/iostream_helpers.hpp>
#include <camoto/byteorder.hpp>
#include <camoto/substream.hpp>
#include "conv-byteplanar.hpp"
#include "img-ccaves.hpp"

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

/// Number of planes in each image
#define NUMPLANES_SPRITE  5
#define NUMPLANES_TILE    4

/// Maximum number of tilesets expected to be in a single graphics file
#define SAFETY_MAX_TILESETCOUNT 1024

//
// CCavesGraphicsType
//

refcount_declclass(CCavesGraphicsType);

CCavesGraphicsType::CCavesGraphicsType()
	throw ()
{
	refcount_qenterclass(CCavesGraphicsType);
}

CCavesGraphicsType::~CCavesGraphicsType()
	throw ()
{
	refcount_qexitclass(CCavesGraphicsType);
}

std::string CCavesGraphicsType::getCode() const
	throw ()
{
	return "img-ccaves";
}

std::string CCavesGraphicsType::getFriendlyName() const
	throw ()
{
	return "Crystal Caves Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> CCavesGraphicsType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("gfx");
	return vcExtensions;
}

std::vector<std::string> CCavesGraphicsType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Crystal Caves");
	//vcGames.push_back("Secret Agent");
	return vcGames;
}

E_CERTAINTY CCavesGraphicsType::isInstance(iostream_sptr psGraphics) const
	throw (std::ios::failure)
{
	psGraphics->seekg(0, std::ios::end);
	io::stream_offset len = psGraphics->tellg();

	// TESTED BY: TODO //fmt_grp_duke3d_isinstance_c02
	if (len < 3) return EC_DEFINITELY_NO; // too short

	psGraphics->seekg(0, std::ios::beg);
	io::stream_offset pos = 0;
	while (pos < len) {
		uint8_t numTiles, width, height;
		psGraphics
			>> u8(numTiles)
			>> u8(width)
			>> u8(height)
		;
		int delta = width*height*NUMPLANES_SPRITE*numTiles;
/*
		// Make sure we don't get stuck
		if (delta == 0) {
			if ((numTiles == 0) && (len == 3)) {
				// This matches a valid empty file
				return EC_POSSIBLY_YES;
			}
			return EC_DEFINITELY_NO;
		}
*/
		// If this pushes us past EOF it's not a valid file
		pos += delta + 3;
		if (pos > len) return EC_DEFINITELY_NO;

		psGraphics->seekg(delta, std::ios::cur);
	}

	// TESTED BY: TODO //fmt_grp_duke3d_isinstance_c01
	return EC_POSSIBLY_YES;
}

GraphicsPtr CCavesGraphicsType::create(iostream_sptr psGraphics, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	throw std::ios::failure("not implemented yet");
	/*psGraphics->seekp(0, std::ios::beg);
	psGraphics->write("KenSilverman\0\0\0\0", 16);
	return GraphicsPtr(new CCavesGraphics(psGraphics));*/
}

// Preconditions: isInstance() has returned > EC_DEFINITELY_NO
GraphicsPtr CCavesGraphicsType::open(iostream_sptr psGraphics, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	return GraphicsPtr(new CCavesGraphics(psGraphics, NUMPLANES_SPRITE));
}

MP_SUPPLIST CCavesGraphicsType::getRequiredSupps(const std::string& filenameGraphics) const
	throw ()
{
	// No supplemental types/empty list
	return MP_SUPPLIST();
}


//
// CCavesGraphics
//

refcount_declclass(CCavesGraphics);

CCavesGraphics::CCavesGraphics(iostream_sptr psGraphics, unsigned int numPlanes)
	throw (std::ios::failure) :
		data(new segmented_stream(psGraphics)),
		numPlanes(numPlanes)
{
	this->data->seekg(0, std::ios::end);
	io::stream_offset len = this->data->tellg();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < 3) throw std::ios::failure("file too short");

	this->data->seekg(0, std::ios::beg);

	io::stream_offset pos = 0;
	while (pos < len) {
		uint8_t numTiles, width, height;
		this->data
			>> u8(numTiles)
			>> u8(width)
			>> u8(height)
		;
		Tileset next;
		next.offset = pos;
		next.len = width*height*this->numPlanes*numTiles+3;
		// Make sure this tileset won't go past EOF or is zero data
		if (pos + next.len > len) break;
		this->tilesets.push_back(next);
		this->data->seekg(next.len-3, std::ios::cur);
		pos += next.len;
		if (this->tilesets.size() >= SAFETY_MAX_TILESETCOUNT) {
			throw std::ios::failure("too many tilesets or corrupted graphics file");
		}
	}

	refcount_qenterclass(CCavesGraphics);
}

CCavesGraphics::~CCavesGraphics()
	throw ()
{
	refcount_qexitclass(CCavesGraphics);
}

int CCavesGraphics::getCaps()
	throw ()
{
	return 0;
}

int CCavesGraphics::getTilesetCount()
	throw ()
{
	return this->tilesets.size();
}

GraphicsPtr CCavesGraphics::getTileset(int index)
	throw ()
{
	assert(index < this->tilesets.size());

	iostream_sptr tilesetStream = iostream_sptr(new substream(
		this->data,
		this->tilesets[index].offset,
		this->tilesets[index].len
	));
	FN_TRUNCATE fnTruncate = NULL;//TEMP
	return GraphicsPtr(
		new CCavesTileset(tilesetStream, fnTruncate, this->numPlanes)
	);
}

void CCavesGraphics::getTileSize(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = 0;
	*height = 0;
	return;
}

unsigned int CCavesGraphics::getLayoutWidth()
	throw ()
{
	return 0;
}

void CCavesGraphics::setTileSize(unsigned int x, unsigned int y)
	throw (std::ios::failure)
{
	// This function should never be called (as per getCaps() response.)
	assert(false);
}

int CCavesGraphics::getImageCount()
	throw ()
{
	return 0;
}

void CCavesGraphics::setImageCount(int count)
	throw (std::ios::failure)
{
	// This function should never be called (as per getCaps() response.)
	assert(false);
}

ImageConverterPtr CCavesGraphics::openImage(int index)
	throw ()
{
	// This function should never be called (as getImageCount() returns 0.)
	assert(false);
	return ImageConverterPtr();
}


//
// CCavesTileset
//

CCavesTileset::CCavesTileset(iostream_sptr data, FN_TRUNCATE fnTruncate, uint8_t numPlanes)
	throw (std::ios::failure) :
		data(data),
		fnTruncate(fnTruncate),
		numPlanes(numPlanes)
{
	this->data->seekg(0, std::ios::end);
	io::stream_offset len = this->data->tellg();

	// We still have to perform sanity checks in case the user forced an
	// open even though it failed the signature check.
	if (len < 3) throw std::ios::failure("file too short");

	this->data->seekg(0, std::ios::beg);
	this->data
		>> u8(this->numImages)
		>> u8(this->width)
		>> u8(this->height)
	;

	refcount_qenterclass(CCavesTileset);
}

CCavesTileset::~CCavesTileset()
	throw ()
{
	refcount_qexitclass(CCavesTileset);
}

int CCavesTileset::getCaps()
	throw ()
{
	return ChangeTileCount | ChangeTileSize;
}

void CCavesTileset::getTileSize(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->width * 8;
	*height = this->height;
	return;
}

unsigned int CCavesTileset::getLayoutWidth()
	throw ()
{
	return 10;
}

void CCavesTileset::setTileSize(unsigned int x, unsigned int y)
	throw (std::ios::failure)
{
	this->width = x;
	this->height = y;

	// Resize the file
	this->fnTruncate(this->width * this->height * this->numPlanes * this->numImages);

	// Update the header
	this->data->seekp(1);
	this->data
		<< u8(this->width)
		<< u8(this->height)
	;
	return;
}

int CCavesTileset::getImageCount()
	throw ()
{
	return this->numImages;
}

void CCavesTileset::setImageCount(int count)
	throw (std::ios::failure)
{
	this->numImages = count;

	// Resize the file
	this->fnTruncate(this->width * this->height * this->numPlanes * this->numImages);

	// Update the header
	this->data->seekp(0);
	this->data
		<< u8(this->numImages)
	;
	return;
}

ImageConverterPtr CCavesTileset::openImage(int index)
	throw ()
{
	assert(index < this->numImages);
	int tileSize = this->width * this->height * this->numPlanes;

	PLANE_LAYOUT planes;
	planes[PLANE_BLUE] = 2;
	planes[PLANE_GREEN] = 3;
	planes[PLANE_RED] = 4;
	planes[PLANE_INTENSITY] = 5;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 1;

	iostream_sptr img(new substream(this->data, tileSize * index + 3, tileSize));
	ImageConverterPtr conv(new EGABytePlanarConverter(img, this->width * 8, this->height, planes));

	return conv;
}


} // namespace gamegraphics
} // namespace camoto
