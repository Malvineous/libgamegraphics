/**
 * @file  common.hpp
 * @brief Functions shared between gametls and gameimg
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

#include <camoto/gamegraphics.hpp>
#include <png++/png.hpp>
#include "pngutil.hpp"

namespace stream = camoto::stream;
namespace gg = camoto::gamegraphics;

/// Export an image to a .png file.
/**
 * Convert the given image into a PNG file on disk.
 *
 * @param img
 *   Image file to export
 *
 * @param destFile
 *   Filename of destination (including ".png")
 */
void imageToPng(const gg::Image& img, const std::string& destFile,
	std::shared_ptr<const gg::Palette> pal)
{
	// Must have a palette here, caller must supply a default one if needed.
	assert(pal);

	auto dims = img.dimensions();
	auto data = img.convert();
	auto mask = img.convert_mask();

	png::image<png::index_pixel> png(dims.x, dims.y);

	// Scan the image and see if there is an unused colour
	int forceXP = -1;
	std::vector<bool> clrUsed(256, false);
	auto *pixMask = mask.data();
	for (auto pix : data) {
		if (!(*pixMask & (int)gg::Image::Mask::Transparent)) {
			// Only count non-transparent pixels as using a colour
			clrUsed[pix] = true;
		}
		pixMask++;
	}
	// Prefer colour 255 if possible, then 0, otherwise find any unused colour
	if (!clrUsed[255]) {
		forceXP = 255;
	} else if (!clrUsed[0]) {
		forceXP = 0;
	} else {
		for (int i = 1; i < 255; i++) {
			if (!clrUsed[i]) {
				forceXP = i;
				break;
			}
		}
		if (forceXP < 0) {
			// Every single colour in the image is used!
			std::cerr << "Warning: This image uses all 256 colours plus "
				"transparency, so there is no room for palette-based transparency!  "
				"There will be no transparency in the output image.\n";
		}
	}
	png::palette pngPal;
	png::tRNS pngTNS;
	int palOffset;
	auto transparentIndex = preparePalette(img.colourDepth(), pal.get(),
		&pngPal, &pngTNS, &palOffset, forceXP);
	png.set_palette(pngPal);
	if (pngTNS.size() > 0) png.set_tRNS(pngTNS);

	// Put the pixel data into the .png structure
	auto pixelData = data.data();
	auto pixelMask = mask.data();
	for (unsigned int y = 0; y < dims.y; y++) {
		auto row = &png[y][0];
		for (unsigned int x = 0; x < dims.x; x++) {
			if (
				(transparentIndex >= 0)
				&& (*pixelMask & (int)gg::Image::Mask::Transparent)
			) {
				*row++ = png::index_pixel(transparentIndex);
			} else {
				*row++ = png::index_pixel(*pixelData + palOffset);
			}
			pixelData++;
			pixelMask++;
		}
	}

	png.write(destFile);
	return;
}

/// Replace an image with the contents of a .png file
/**
 * Load the given PNG file and use it to replace the given image.
 *
 * @param  img  Image file to overwrite
 *
 * @param  srcFile  Filename of source (including ".png")
 */
void pngToImage(gg::Image* img, const std::string& srcFile)
{
	auto dims = img->dimensions();

	png::image<png::index_pixel> png(srcFile);

	unsigned int pngWidth = png.get_width();
	unsigned int pngHeight = png.get_height();
	if ((pngWidth != dims.x) || (pngHeight != dims.y)) {
		if (img->caps() & gg::Image::Caps::SetDimensions) {
			dims.x = pngWidth;
			dims.y = pngHeight;
			img->dimensions(dims);
		} else {
			throw stream::error(createString("png image is " << pngWidth << "x"
				<< pngHeight << " but target image is fixed as " << dims.x << "x"
				<< dims.y));
		}
	}

	bool hasTransparency = false;
	int pixelOffset = 0;
	png::tRNS transparency = png.get_tRNS();
	if (transparency.size() > 0) {
		if (transparency[0] != 0) {
			throw stream::error("palette entry #0 must be assigned as transparent");
		}
		hasTransparency = true;
		pixelOffset = -1; // to account for palette #0 being inserted for use as transparency
	}

	unsigned int imgSizeBytes = dims.x * dims.y;
	gg::Pixels pix, mask;
	pix.resize(imgSizeBytes, 0x00);
	mask.resize(imgSizeBytes, 0x00);

	for (unsigned int y = 0; y < dims.y; y++) {
		for (unsigned int x = 0; x < dims.x; x++) {
			uint8_t pixel = png[y][x];
			if (hasTransparency && (pixel == 0)) { // Palette #0 must be transparent
				mask[y * dims.x + x] = 0x01; // transparent
				pix[y * dims.x + x] = 0;
			} else {
				mask[y * dims.x + x] = 0x00; // opaque
				pix[y * dims.x + x] = pixel + pixelOffset;
			}
		}
	}

	if (img->caps() & gg::Image::Caps::SetPalette) {
		// This format supports custom palettes, so update it from the
		// PNG image.
		const png::palette& pngPal = png.get_palette();
		auto newPal = std::make_shared<gg::Palette>();
		newPal->reserve(pngPal.size());
		for (auto& i : pngPal) {
			gg::PaletteEntry p;
			p.red = i.red;
			p.green = i.green;
			p.blue = i.blue;
			p.alpha = 255;
			newPal->push_back(p);
		}
		img->palette(newPal);
	}
	img->convert(pix, mask);
	return;
}

/// Export an image to ANSI coloured text
/**
 * Convert the given image into ANSI text and print it on stdout.
 *
 * @param  img  The image to display.
 */
void imageToANSI(const gg::Image& img)
{
	auto dims = img.dimensions();
	auto data = img.convert();
	auto mask = img.convert_mask();

	int pos = 0;
	bool bright = false, xp = false;
	std::cout << "\x1B[0;7m";
	bool bFirst = true;
	for (unsigned int y = 0; y < dims.y; y++) {
		if (!bFirst) std::cout << "\n"; else bFirst = false;
		for (unsigned int x = 0; x < dims.x; x++, pos++) {
			uint8_t pixel = data[pos], maskpixel = mask[pos];
			std::cout << "\x1B[";
			if (pixel & 0x08) {
				if (!bright) {
					std::cout << "1;"; // intensity bit on
					bright ^= 1;
				}
			} else {
				if (bright) {
					std::cout << "22;"; // intensity bit off
					bright ^= 1;
				}
			}
			if (maskpixel & 0x01) {
				if (!xp) {
					std::cout << "27;"; // reverse off (xp on)
					xp ^= 1;
				}
			} else {
				if (xp) {
					std::cout << "7;"; // reverse on (xp off)
					xp ^= 1;
				}
			}
			int ansiClr = 30 + (
				((pixel & 0x01) << 2) |
				(pixel & 0x02) |
				((pixel & 0x04) >> 2)
			);
			std::cout << ansiClr << "m  ";
		}
	}
	std::cout << "\x1B[0m" << std::endl;
	return;
}
