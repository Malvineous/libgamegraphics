/**
 * @file   img-ega-planar.cpp
 * @brief  Image implementation adding support for the EGA byte-planar format.
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

#include <cstring>  // memset
#include <cassert>
#include <camoto/iostream_helpers.hpp>
#include "img-ega-planar.hpp"

namespace camoto {
namespace gamegraphics {

EGAPlanarImage::EGAPlanarImage()
{
}

EGAPlanarImage::~EGAPlanarImage()
{
}

void EGAPlanarImage::setParams(stream::inout_sptr data,
	stream::pos offset, int width, int height, const PLANE_LAYOUT& planes
)
{
	this->data = data;
	this->offset = offset;
	this->width = width;
	this->height = height;
	memcpy(this->planes, planes, sizeof(PLANE_LAYOUT));
	return;
}

int EGAPlanarImage::getCaps()
{
	return ColourDepthEGA;
}

void EGAPlanarImage::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->width;
	*height = this->height;
	return;
}

void EGAPlanarImage::setDimensions(unsigned int width, unsigned int height)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	this->width = width;
	this->height = height;

	int numPlanes = 0;
	for (int p = 0; p < PLANE_MAX; p++) {
		// Count the plane if its order is nonzero, otherwise ignore it
		if (this->planes[p]) numPlanes++;
	}

	// TODO: Confirm this is correct
	this->data->truncate((this->width + 7) / 8 * this->height * numPlanes);
	return;
}

StdImageDataPtr EGAPlanarImage::toStandard()
{
	return this->doConversion(false);
}

StdImageDataPtr EGAPlanarImage::toStandardMask()
{
	if ((this->planes[PLANE_OPACITY] == 0) && (this->planes[PLANE_HITMAP] == 0)) {
		// Mask is unused, skip the conversion and return an opaque mask
		unsigned int width, height;
		this->getDimensions(&width, &height);
		assert((width != 0) && (height != 0));
		int dataSize = width * height;

		// Return an entirely opaque mask
		uint8_t *imgData = new uint8_t[dataSize];
		StdImageDataPtr ret(imgData);
		memset(imgData, 0, dataSize);

		return ret;
	}

	// Otherwise decode the mask
	return this->doConversion(true);
}

void EGAPlanarImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
{
	// Sort out all the values we need to output for each plane
	int numPlanes = 0;
	int planeValue[PLANE_MAX];
	bool planeMask[PLANE_MAX]; // true == use newMask, false == use newContent
	bool planeSwap[PLANE_MAX]; // true == invert bits, false == leave alone
	memset(planeValue, 0, PLANE_MAX);
	for (int p = 0; p < PLANE_MAX; p++) {
		// Count the plane if its order is nonzero, otherwise ignore it
		if (this->planes[p]) numPlanes++; else continue;

		// Handle negative values
		int order;
		bool swap;
		if (this->planes[p] < 0) {
			swap = true;
			order = -this->planes[p];
		} else {
			swap = false;
			order = this->planes[p];
		}
		order--;

		// Sanity check
		assert(order < PLANE_MAX);

		// Figure out which bit this plane should set in the 8bpp output data
		int value;
		bool mask;
		switch (p) {
			case PLANE_BLUE:      mask = false; value = 0x01; break;
			case PLANE_GREEN:     mask = false; value = 0x02; break;
			case PLANE_RED:       mask = false; value = 0x04; break;
			case PLANE_INTENSITY: mask = false; value = 0x08; break;

			case PLANE_OPACITY:   mask = true;  value = 0x01; break;
			case PLANE_HITMAP:    mask = true;  value = 0x02; break;

			default:              value = 0x00; break;
		}
		planeMask[order] = mask;
		planeSwap[order] = swap;
		planeValue[order] = value;
	}

	this->data->seekp(0, stream::start);
	uint8_t *rowData;

	// For each pixel, set the appropriate bits in the cell
	for (int p = 0; p < numPlanes; p++) {
		uint8_t *imgData = (uint8_t *)newContent.get();
		uint8_t *maskData = (uint8_t *)newMask.get();

		for (int y = 0; y < this->height; y++) {

			// Run through each lot of eight pixels (a "cell")
			for (int x = 0; x < (this->width + 7) / 8; x++) {

				uint8_t c = 0;

				// Work out if this plane will read from the input image or mask data.
				if (planeMask[p]) rowData = maskData;
				else rowData = imgData;

				// See how many bits we should run through.  This is only used
				// when the image is not an even multiple of 8.
				int bits = 8;
				if (x * 8 + 8 > this->width) bits = this->width % 8;

				// Run through each pixel in the group
				for (int b = 0; b < bits; b++) {
					if (rowData[x * 8 + b] & planeValue[p]) {
						// The pixel is on in this plane
						c |= 0x80 >> b;
					} // else the pixel is off in this plane
				}
				// If the plane index was negative, the bits need to be inverted.  If
				// we invert the whole number (c = ~c, or c ^= 0xff) then we could
				// invert too many bits (e.g. all eight bits will be inverted when an
				// image might only be four pixels wide.)  By XORing the value below,
				// only the bits used by the image will be flipped, with the unused
				// bits remaining as zero.
				if (planeSwap[p]) c ^= ~((1 << (8-bits)) - 1);

				this->data << u8(c);
			}
			imgData += this->width;
			maskData += this->width;
		}
	}
	this->data->truncate_here();
	return;
}

StdImageDataPtr EGAPlanarImage::doConversion(bool mask)
{
	// Sort out all the values we need to output for each plane
	int numPlanes = 0;
	int planeValue[PLANE_MAX], notPlaneValue[PLANE_MAX];
	memset(planeValue, 0, sizeof(planeValue));
	memset(notPlaneValue, 0, sizeof(notPlaneValue));
	for (int p = 0; p < PLANE_MAX; p++) {
		// Count the plane if its order is nonzero, otherwise ignore it
		if (!this->planes[p]) continue;
		if (this->planes[p] > numPlanes) numPlanes = this->planes[p];

		// Handle negative values
		int order;
		bool swap;
		if (this->planes[p] < 0) {
			swap = true;
			order = -this->planes[p];
		} else {
			swap = false;
			order = this->planes[p];
		}
		order--;

		// Sanity check
		assert(order < PLANE_MAX);

		// Figure out which bit this plane should set in the 8bpp output data
		int value;
		if (mask) {
			switch (p) {
				case PLANE_OPACITY:   value = 0x01; break;
				case PLANE_HITMAP:    value = 0x02; break;
				default:              value = 0x00; break;
			}
		} else {
			switch (p) {
				case PLANE_BLUE:      value = 0x01; break;
				case PLANE_GREEN:     value = 0x02; break;
				case PLANE_RED:       value = 0x04; break;
				case PLANE_INTENSITY: value = 0x08; break;
				default:              value = 0x00; break;
			}
		}
		if (swap) {
			planeValue[order] = 0;
			notPlaneValue[order] = value;
		} else {
			planeValue[order] = value;
			notPlaneValue[order] = 0;
		}
	}

	int imgSizeBytes = (this->width * 8) * this->height;
	uint8_t *rowData = new uint8_t[imgSizeBytes];
	StdImageDataPtr ret(rowData);
	memset(rowData, 0, imgSizeBytes);
	this->data->seekg(0, stream::start);

	for (int p = 0; p < numPlanes; p++) {
		rowData = ret.get();

		// Don't waste time processing a plane we're ignoring
		if (!(planeValue[p] || notPlaneValue[p])) {
			this->data->seekg(this->height * ((this->width + 7) / 8), stream::cur);
			continue;
		}

		for (int y = 0; y < this->height; y++) {
			// Run through each lot of eight pixels (a "cell")
			// Adding 7 means a width that's not an even multiple of eight will
			// effectively be rounded up to the next byte - so an eight pixel wide
			// image will use one byte (8 + 7 = 15, 15 / 8 == 1) but a nine pixel
			// wide image will use two bytes (9 + 7 = 16, 16 / 8 == 2).
			for (int x = 0; x < (this->width + 7) / 8; x++) {

				uint8_t nextByte;
				try {
					this->data >> u8(nextByte);
				} catch (const stream::incomplete_read) {
					std::cerr << "ERROR: Incomplete read converting image to standard "
						"format.  Returning partial conversion." << std::endl;
					return ret;
				}

				// See how many bits we should run through.  This is only used
				// when the image is not an even multiple of 8.
				int bits = 8;
				if (x * 8 + 8 > this->width) bits = this->width % 8;

				// Run through all the (valid) bits in this byte
				for (int b = 0; b < bits; b++) {
					rowData[x * 8 + b] |=
						(nextByte & (0x80 >> b)) ? planeValue[p] : notPlaneValue[p];
				}
			}
			rowData += this->width;
		}
	}
	return ret;
}


//
// EGARawPlanarBGRIImageType
//

EGARawPlanarBGRIImageType::EGARawPlanarBGRIImageType()
{
}

EGARawPlanarBGRIImageType::~EGARawPlanarBGRIImageType()
{
}

std::string EGARawPlanarBGRIImageType::getCode() const
{
	return "img-ega-raw-planar-bgri-fullscreen";
}

std::string EGARawPlanarBGRIImageType::getFriendlyName() const
{
	return "Raw Planar EGA fullscreen image (BGRI)";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> EGARawPlanarBGRIImageType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	return vcExtensions;
}

std::vector<std::string> EGARawPlanarBGRIImageType::getGameList() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty EGARawPlanarBGRIImageType::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();

	// TESTED BY: TODO
	if (len == 32000) return PossiblyYes;

	// TESTED BY: TODO
	return DefinitelyNo;
}

ImagePtr EGARawPlanarBGRIImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	psImage->truncate(32000);
	psImage->seekp(0, stream::start);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 500; i++) psImage->write(buf, 64);

	SuppData dummy;
	return this->open(psImage, dummy);
}

ImagePtr EGARawPlanarBGRIImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	EGAPlanarImage *ega = new EGAPlanarImage();
	ImagePtr img(ega);

	PLANE_LAYOUT planes;
	planes[PLANE_BLUE] = 1;
	planes[PLANE_GREEN] = 2;
	planes[PLANE_RED] = 3;
	planes[PLANE_INTENSITY] = 4;
	planes[PLANE_HITMAP] = 0;
	planes[PLANE_OPACITY] = 0;
	ega->setParams(psImage, 0, 320, 200, planes);

	return img;
}

SuppFilenames EGARawPlanarBGRIImageType::getRequiredSupps(const std::string& filenameImage) const
{
	return SuppFilenames();
}


} // namespace gamegraphics
} // namespace camoto
