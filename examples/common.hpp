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
	std::shared_ptr<const gg::Palette> palImg)
{
	auto dims = img.dimensions();
	auto data = img.convert();
	auto mask = img.convert_mask();

	png::image<png::index_pixel> png(dims.x, dims.y);

	bool useMask;

	std::shared_ptr<const gg::Palette> srcPal;

	if (img.caps() & gg::Image::Caps::HasPalette) {
		srcPal = palImg;
	}
	if (!srcPal) {
		// Need to use the default palette
		switch (img.colourDepth()) {
			case gg::ColourDepth::VGA:
				srcPal = gg::createPalette_DefaultVGA();
				break;
			case gg::ColourDepth::EGA:
				srcPal = gg::createPalette_DefaultEGA();
				break;
			case gg::ColourDepth::CGA:
				srcPal = gg::createPalette_CGA(gg::CGAPaletteType::CyanMagenta);
				break;
			case gg::ColourDepth::Mono:
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

	for (auto& i : *srcPal) {
		pal[j] = png::color(i.red, i.green, i.blue);
		if (i.alpha == 0x00) transparency.push_back(j); // @todo < 0x80 perhaps?
		j++;
	}
	png.set_palette(pal);
	if (transparency.size()) png.set_tRNS(transparency);

	// Put the pixel data into the .png structure
	for (unsigned int y = 0; y < dims.y; y++) {
		auto row = &png[y][0];
		for (unsigned int x = 0; x < dims.x; x++) {
			if (useMask) {
				if (mask[y*dims.x+x] & 0x01) {
					*row++ = png::index_pixel(0);
				} else {
					*row++ = png::index_pixel(data[y*dims.x+x] + 1);
				}
			} else {
				*row++ = png::index_pixel(data[y*dims.x+x]);
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
