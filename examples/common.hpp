/**
 * @file   common.hpp
 * @brief  Functions shared between gamegfx and gameimg
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

#include <camoto/gamegraphics.hpp>
#include "png++/png.hpp"

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
void imageToPng(gg::ImagePtr img, const std::string& destFile)
{
	unsigned int width, height;
	img->getDimensions(&width, &height);

	gg::StdImageDataPtr data = img->toStandard();
	gg::StdImageDataPtr mask = img->toStandardMask();

	png::image<png::index_pixel> png(width, height);

	bool useMask;

	if (img->getCaps() & gg::Image::HasPalette) {
		gg::PaletteTablePtr srcPal = img->getPalette();
		png::palette pal(srcPal->size());
		int j = 0;
		//pal[ 0] = png::color(0xFF, 0x00, 0xFF); // transparent
		for (gg::PaletteTable::iterator i = srcPal->begin();
			i != srcPal->end();
			i++, j++
		) {
			pal[j] = png::color(i->red, i->green, i->blue);
		}
		png.set_palette(pal);
		useMask = false; // not enough room in the palette for transparent entry
	} else {
		// Standard EGA palette
		png::palette pal(17);
		pal[ 0] = png::color(0xFF, 0x00, 0xFF); // transparent
		pal[ 1] = png::color(0x00, 0x00, 0x00);
		pal[ 2] = png::color(0x00, 0x00, 0xAA);
		pal[ 3] = png::color(0x00, 0xAA, 0x00);
		pal[ 4] = png::color(0x00, 0xAA, 0xAA);
		pal[ 5] = png::color(0xAA, 0x00, 0x00);
		pal[ 6] = png::color(0xAA, 0x00, 0xAA);
		pal[ 7] = png::color(0xAA, 0x55, 0x00);
		pal[ 8] = png::color(0xAA, 0xAA, 0xAA);
		pal[ 9] = png::color(0x55, 0x55, 0x55);
		pal[10] = png::color(0x55, 0x55, 0xFF);
		pal[11] = png::color(0x55, 0xFF, 0x55);
		pal[12] = png::color(0x55, 0xFF, 0xFF);
		pal[13] = png::color(0xFF, 0x55, 0x55);
		pal[14] = png::color(0xFF, 0x55, 0xFF);
		pal[15] = png::color(0xFF, 0xFF, 0x55);
		pal[16] = png::color(0xFF, 0xFF, 0xFF);
		png.set_palette(pal);

		useMask = true; // first palette entry is transparency

		// Make first colour transparent
		png::tRNS transparency;
		transparency.push_back(0);
		png.set_tRNS(transparency);
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (useMask) {
				if (mask[y*width+x] & 0x01) {
					png[y][x] = png::index_pixel(0);
				} else {
					png[y][x] = png::index_pixel(data[y*width+x] + 1);
				}
			} else {
				png[y][x] = png::index_pixel(data[y*width+x]);
			}
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
void pngToImage(gg::ImagePtr img, const std::string& srcFile)
	throw (std::ios::failure)
{
	unsigned int width, height;
	img->getDimensions(&width, &height);

	png::image<png::index_pixel> png(
		srcFile, png::require_color_space<png::index_pixel>()
	);

	if ((png.get_width() != width) || (png.get_height() != height)) {
		throw std::ios::failure("image does not match tileset size");
	}

	png::tRNS transparency = png.get_tRNS();
	if ((transparency.size() < 1) || (transparency[0] != 0)) {
		throw std::ios::failure("palette entry #0 must be assigned as transparent");
	}

	int imgSizeBytes = width * height;
	uint8_t *imgData = new uint8_t[imgSizeBytes];
	uint8_t *maskData = new uint8_t[imgSizeBytes];
	gg::StdImageDataPtr stdimg(imgData);
	gg::StdImageDataPtr stdmask(maskData);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			uint8_t pixel = png[y][x];
			if (pixel == 0) { // Palette #0 must be transparent
				maskData[y * width + x] = 0x01; // transparent
			} else {
				maskData[y * width + x] = 0x00; // opaque
				imgData[y * width + x] = pixel - 1; // -1 to account for palette #0
			}
		}
	}

	if (img->getCaps() & gg::Image::HasPalette) {
		// TODO: This format supports custom palettes, so update it from the
		// PNG image.
		//img->setPalette(...);
	}
	img->fromStandard(stdimg, stdmask);

	return;
}

/// Export an image to ANSI coloured text
/**
 * Convert the given image into ANSI text and print it on stdout.
 *
 * @param  img  The image to display.
 */
void imageToANSI(gg::ImagePtr img)
{
	unsigned int width, height;
	img->getDimensions(&width, &height);

	gg::StdImageDataPtr data = img->toStandard();
	gg::StdImageDataPtr mask = img->toStandardMask();
	int pos = 0;
	bool bright = false, xp = false;
	std::cout << "\x1B[0;7m";
	bool bFirst = true;
	for (int y = 0; y < height; y++) {
		if (!bFirst) std::cout << "\n"; else bFirst = false;
		for (int x = 0; x < width; x++, pos++) {
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
