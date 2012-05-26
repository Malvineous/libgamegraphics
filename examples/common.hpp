/**
 * @file   common.hpp
 * @brief  Functions shared between gamegfx and gameimg
 *
 * Copyright (C) 2010-2012 Adam Nielsen <malvineous@shikadi.net>
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
void imageToPng(gg::ImagePtr img, const std::string& destFile)
{
	unsigned int width, height;
	img->getDimensions(&width, &height);

	gg::StdImageDataPtr data = img->toStandard();
	gg::StdImageDataPtr mask = img->toStandardMask();

	png::image<png::index_pixel> png(width, height);

	bool useMask;

	gg::PaletteTablePtr srcPal;

	if (img->getCaps() & gg::Image::HasPalette) {
		srcPal = img->getPalette();
	} else {
		// Need to use the default palette
		switch (img->getCaps() & gg::Image::ColourDepthMask) {
			case gg::Image::ColourDepthVGA:
				srcPal = gg::createPalette_DefaultVGA();
				break;
			case gg::Image::ColourDepthEGA:
				srcPal = gg::createPalette_DefaultEGA();
				break;
			case gg::Image::ColourDepthCGA:
				srcPal = gg::createPalette_CGA(gg::CGAPal_CyanMagenta);
				break;
			case gg::Image::ColourDepthMono:
				srcPal = gg::createPalette_DefaultMono();
				break;
		}
	}

	unsigned int palSize = srcPal->size();
	int j = 0;

	// Figure out whether there's enough room in the palette to use the image
	// mask for transparency, or whether we have to fall back to palette index
	// transparency only.
	png::tRNS transparency;
	if (palSize < 256) {
		transparency.push_back(j);
		j++;
		palSize++;
		useMask = true;
	} else {
		// Not enough room in the palette for masked transparent entry
		useMask = false;
	}
	png::palette pal(palSize);

	// Set a colour for the transparent palette entry, for apps which can't
	// display transparent pixels (we couldn't set this above.)
	if (useMask) pal[0] = png::color(0xFF, 0x00, 0xFF);

	for (gg::PaletteTable::iterator
		i = srcPal->begin(); i != srcPal->end(); i++, j++
	) {
		pal[j] = png::color(i->red, i->green, i->blue);
		if (i->alpha == 0x00) transparency.push_back(j);
	}
	png.set_palette(pal);
	if (transparency.size()) png.set_tRNS(transparency);

	// Put the pixel data into the .png structure
	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
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
	throw (stream::error)
{
	unsigned int width, height;
	img->getDimensions(&width, &height);

	png::image<png::index_pixel> png(srcFile);

	if ((png.get_width() != width) || (png.get_height() != height)) {
		throw stream::error(createString("image does not match target size ("
			<< png.get_width() << "x" << png.get_height() << " != " << width << "x"
			<< height << ")"));
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

	int imgSizeBytes = width * height;
	uint8_t *imgData = new uint8_t[imgSizeBytes];
	uint8_t *maskData = new uint8_t[imgSizeBytes];
	gg::StdImageDataPtr stdimg(imgData);
	gg::StdImageDataPtr stdmask(maskData);

	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			uint8_t pixel = png[y][x];
			if (hasTransparency && (pixel == 0)) { // Palette #0 must be transparent
				maskData[y * width + x] = 0x01; // transparent
				imgData[y * width + x] = 0;
			} else {
				maskData[y * width + x] = 0x00; // opaque
				imgData[y * width + x] = pixel + pixelOffset;
			}
		}
	}

	if (img->getCaps() & gg::Image::HasPalette) {
		// This format supports custom palettes, so update it from the
		// PNG image.
		const png::palette& pngPal = png.get_palette();
		gg::PaletteTablePtr newPal(new gg::PaletteTable());
		newPal->reserve(pngPal.size());
		for (png::palette::const_iterator i = pngPal.begin(); i != pngPal.end(); i++) {
			gg::PaletteEntry p(i->red, i->green, i->blue, 255);
			newPal->push_back(p);
		}
		img->setPalette(newPal);
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
	for (unsigned int y = 0; y < height; y++) {
		if (!bFirst) std::cout << "\n"; else bFirst = false;
		for (unsigned int x = 0; x < width; x++, pos++) {
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
