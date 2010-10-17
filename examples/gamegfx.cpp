/**
 * @file   gamegfx.cpp
 * @brief  Command-line interface to libgamegraphics.
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

#include <boost/algorithm/string.hpp> // for case-insensitive string compare
#include <boost/program_options.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/bind.hpp>
#include <camoto/gamegraphics.hpp>
#include <iostream>
#include <fstream>
#include "png++/png.hpp"

namespace po = boost::program_options;
namespace gg = camoto::gamegraphics;

#define PROGNAME "gamegfx"

/*** Return values ***/
// All is good
#define RET_OK                 0
// Bad arguments (missing/invalid parameters)
#define RET_BADARGS            1
// Major error (couldn't open archive file, etc.)
#define RET_SHOWSTOPPER        2
// More info needed (-t auto didn't work, specify a type)
#define RET_BE_MORE_SPECIFIC   3
// One or more files failed, probably user error (file not found, etc.)
#define RET_NONCRITICAL_FAILURE 4
// Some files failed, but not in a common way (cut off write, disk full, etc.)
#define RET_UNCOMMON_FAILURE   5

// Split a string in two at a delimiter, e.g. "one=two" becomes "one" and "two"
// and true is returned.  If there is no delimiter both output strings will be
// the same as the input string and false will be returned.
//
// If delim == '=' then:
// in        -> ret, out1, out2
// "one=two" -> true, "one", "two"
// "one=two=three" -> true, "one=two", "three"
// "four" -> false, "four", "four"
// If delim == '@' then:
// "one@two" -> true, "one", "two"
// "@1=myfile@@4"
// "test.txt@here.txt"
// "@2=test.txt"
// "e1m1.mid=mysong.mid:@4"
// "e1m1.mid=mysong.mid:e1m2.mid"
bool split(const std::string& in, char delim, std::string *out1, std::string *out2)
{
	std::string::size_type iEqualPos = in.find_last_of(delim);
	*out1 = in.substr(0, iEqualPos);
	// Does the destination have a different filename?
	bool bAltDest = iEqualPos != std::string::npos;
	*out2 = bAltDest ? in.substr(iEqualPos + 1) : *out1;
	return bAltDest;
}

/// Dump a list of tilesets and images
/**
 * This is a recursive function to display a list of tilesets and images when
 * the -l option is used.
 *
 * @return true to continue, false on error (e.g. corrupt file suspected)
 */
bool printTilesetList(std::string prefix, gg::GraphicsPtr pGraphics,
	bool bScript
) {
	unsigned int tilesetCount = pGraphics->getTilesetCount();
	unsigned int imageCount = pGraphics->getImageCount();
	if ((tilesetCount > 4096) || (imageCount > 4096)) {
		std::cerr << "Too many images - aborting as this is probably an error." << std::endl;
		return false; // abort
	}

	unsigned int width, height;
	pGraphics->getTileSize(&width, &height);
	if (bScript) {
		std::cout << "id=" << prefix << ";type=tileset;width=" << width
			<< ";height=" << height << ";tilesets=" << tilesetCount
			<< ";images=" << imageCount << std::endl;
		for (int j = 0; j < imageCount; j++) {
			std::cout << "id=" << prefix << '+' << j
				<< ";type=image;width=" << width
				<< ";height=" << height << std::endl;
		}
	} else {
		std::cout << prefix << ": Tileset (" << width << 'x'
			<< height << "), ";
		if (imageCount) {
			std::cout << "images are " << prefix << "+0 to "
				<< prefix << '+' << (imageCount - 1) << std::endl;
		} else {
			std::cout << "no images" << std::endl;
		}
	}
	for (int j = 0; j < tilesetCount; j++) {
		std::ostringstream ss;
		ss << prefix << '.' << j;
		if (!printTilesetList(
			ss.str(), pGraphics->getTileset(j), bScript
		)) {
			return false;
		}
	}
	return true; // keep going
}


/// Explode an ID into a list of ints
/**
 * Take an ID like 0.3.1/15 and convert it into a vector like [0, 3, 1, -16].
 *
 * @note Because -0 is not possible, the (optional) final image value will
 *       start at -1.  This value will need to be adjusted before it can be
 *       passed to the openImage() function.
 *
 * @param  id  Pointer to a vector where each element should be appended.
 *
 * @param  name  ID to be examined.
 *
 * @return true on success, false on an invalid ID
 */
bool explodeId(std::vector<int> *id, const std::string& name)
{
	int next = 0;
	bool img = false; // is the next number an image? (if not, it's a tileset)
	for (int i = 0; i < name.length(); i++) {
		if ((name[i] == '.') || (name[i] == '+')) {
			// Separator
			id->push_back(next);
			next = 0;
			img = (name[i] == '+'); // is next num an image?
		} else if ((name[i] >= '0') || (name[i] <= '9')) {
			next *= 10;
			next += name[i] - '0';
		} else {
			// Invalid char
			return false;
		}
	}
	// If the last separator was a slash, this is an image number so make it neg.
	if (img) {
		next++; // can't have -0 so images start at -1
		next *= -1;
	}
	id->push_back(next);
	return true;
}

/// Export an image to a .png file
/**
 * Convert the given image into a PNG file on disk.
 *
 * @param  img  Image file to export
 *
 * @param  width  Image width
 *
 * @param  height  Image height
 *
 * @param  destFile  Filename of destination (including ".png")
 */
void imageToPng(gg::ImageConverterPtr img, unsigned int width,
	unsigned int height, const std::string& destFile)
{
	gg::StdImageDataPtr data = img->toStandard();
	gg::StdImageDataPtr mask = img->toStandardMask();

	png::image<png::index_pixel> png(width, height);

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

	// Make first colour transparent
	png::tRNS transparency;
	transparency.push_back(0);
	png.set_tRNS(transparency);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// TODO: optimise this loop (remove i)
			//if (!(data[i] & 0x20)) pixels[i] = 16; // transparent
			//else pixels[i] = data[i] & 0x0F;
			// Only write opaque pixels
			if (mask[y*height+x] & 0x01) png[y][x] = png::index_pixel(data[y*height+x] + 1);
			else png[y][x] = png::index_pixel(0);
		}
	}

	png.write(destFile);
	return;
}

/// Export a whole tileset to a single .png file
/**
 * Convert the given tileset into a PNG file on disk, by arranging each image
 * in the tileset as a grid of the given width.
 *
 * @param  tileset  The tileset to export.
 *
 * @param  widthTiles  The width of the exported .png image, in tiles.  0 means
 *         as wide as possible (all tiles on a single row.)
 *
 * @param  destFile  Filename of destination (including ".png")
 */
void tilesetToPng(gg::GraphicsPtr tileset, unsigned int widthTiles,
	const std::string& destFile)
{
	if (widthTiles == 0) {
		widthTiles = tileset->getLayoutWidth();
	}
	unsigned int numTiles = tileset->getImageCount();
	if (widthTiles > numTiles) widthTiles = numTiles;
	unsigned int heightTiles = (numTiles + widthTiles - 1) / widthTiles;

	unsigned int width, height;
	tileset->getTileSize(&width, &height);

	png::image<png::index_pixel> png(width * widthTiles, height * heightTiles);

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

	// Make first colour transparent
	png::tRNS transparency;
	transparency.push_back(0);
	png.set_tRNS(transparency);

	for (unsigned int t = 0; t < numTiles; t++) {
		gg::ImageConverterPtr img = tileset->openImage(t);
		gg::StdImageDataPtr data = img->toStandard();
		gg::StdImageDataPtr mask = img->toStandardMask();

		unsigned int offX = (t % widthTiles) * width;
		unsigned int offY = (t / widthTiles) * height;

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				// TODO: optimise this loop (remove i)
				//if (!(data[i] & 0x20)) pixels[i] = 16; // transparent
				//else pixels[i] = data[i] & 0x0F;
				// Only write opaque pixels
				if (mask[y*height+x] & 0x01) {
					png[offY+y][offX+x] =
						// +1 to the colour to skip over transparent (#0)
						png::index_pixel(data[y*height+x] + 1);
				} else {
					png[offY+y][offX+x] = png::index_pixel(0);
				}
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
 * @param  width  Image width
 *
 * @param  height  Image height
 *
 * @param  srcFile  Filename of source (including ".png")
 */
void pngToImage(gg::ImageConverterPtr img, unsigned int width,
	unsigned int height, const std::string& srcFile)
	throw (std::ios::failure)
{
	png::image<png::index_pixel> png(
		srcFile, png::require_color_space<png::index_pixel>()
	);

	if ((png.get_width() != width) || (png.get_height() != height)) {
		throw std::ios::failure("image does not match tileset size");
	}

	png::tRNS transparency = png.get_tRNS();
	if (transparency[0] != 0) {
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
				maskData[y * width + x] = 0x00; // transparent
			} else {
				maskData[y * width + x] = 0x01; // opaque
				imgData[y * width + x] = pixel - 1; // -1 to account for palette #0
			}
		}
	}

	gg::PalettePtr origPal = img->getPalette();
	if (origPal) {
		// TODO: This format supports custom palettes, so update it from the
		// PNG image.
	}
	img->fromStandard(stdimg, stdmask, origPal);

	return;
}

/// Replace a tileset with the contents of a .png file
/**
 * Load the given PNG file and use it to replace the given tileset.  The .png
 * image must be an even multiple of the tile width.
 *
 * @param  tileset  Tileset to overwrite
 *
 * @param  srcFile  Filename of source (including ".png")
 */
void pngToTileset(gg::GraphicsPtr tileset, const std::string& srcFile)
	throw (std::ios::failure)
{
	png::image<png::index_pixel> png(
		srcFile, png::require_color_space<png::index_pixel>()
	);

	unsigned int width, height;
	tileset->getTileSize(&width, &height);

	if ((png.get_width() % width) != 0) {
		throw std::ios::failure("image width must be a multiple of the tile width");
	}
	if ((png.get_height() % height) != 0) {
		throw std::ios::failure("image height must be a multiple of the tile height");
	}

	png::tRNS transparency = png.get_tRNS();
	if (transparency[0] != 0) {
		throw std::ios::failure("palette entry #0 must be assigned as transparent");
	}

	unsigned int tilesX = png.get_width() / width;
	unsigned int tilesY = png.get_height() / height;
	unsigned int numTiles = tileset->getImageCount();
	if (numTiles > tilesX * tilesY) numTiles = tilesX * tilesY;

	int imgSizeBytes = width * height;
	uint8_t *imgData = new uint8_t[imgSizeBytes];
	uint8_t *maskData = new uint8_t[imgSizeBytes];
	gg::StdImageDataPtr stdimg(imgData);
	gg::StdImageDataPtr stdmask(maskData);

	for (unsigned int t = 0; t < numTiles; t++) {
		gg::ImageConverterPtr img = tileset->openImage(t);
		gg::StdImageDataPtr data = img->toStandard();
		gg::StdImageDataPtr mask = img->toStandardMask();

		unsigned int offX = (t % tilesX) * width;
		unsigned int offY = (t / tilesX) * height;

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				uint8_t pixel = png[offY + y][offX + x];
				if (pixel == 0) { // Palette #0 must be transparent
					maskData[y * width + x] = 0x00; // transparent
				} else {
					maskData[y * width + x] = 0x01; // opaque
					imgData[y * width + x] = pixel - 1; // -1 to account for palette #0
				}
			}
		}

		gg::PalettePtr origPal = img->getPalette();
		if (origPal) {
			// TODO: This format supports custom palettes, so update it from the
			// PNG image.
		}
		img->fromStandard(stdimg, stdmask, origPal);
	}

	return;
}

/// Export an image to ANSI coloured text
/**
 * Convert the given image into ANSI text and print it on stdout.
 *
 * @param  img  The image to display.
 *
 * @param  width  Image width
 *
 * @param  height  Image height
 */
void imageToANSI(gg::ImageConverterPtr img, unsigned int width, unsigned int height)
{
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
				if (xp) {
					std::cout << "7;"; // reverse on (xp off)
					xp ^= 1;
				}
			} else {
				if (!xp) {
					std::cout << "27;"; // reverse off (xp on)
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

/// Export all images in the graphics file as either individual images or
/// tilesets.
/**
 * Extract all images or tilesets and convert them into PNG files.
 *
 * @param  prefix  ID prefix (used when recurring, use "0" on first call)
 *
 * @param  tilesetAsSingleImage  false to extract each image as a separate
 *         .png file, true to combine images into one .png per tileset
 *
 * @param  widthTiles  The width of the exported .png image, in tiles.  0 means
 *         as wide as possible (all tiles on a single row)
 *
 * @param  pGraphics  Graphics file to export
 *
 * @param  bScript  true if -s option given (produces easily parseable output)
 */
bool extractAllImages(std::string prefix, bool tilesetAsSingleImage,
	int widthTiles, gg::GraphicsPtr pGraphics, bool bScript
) {
	unsigned int tilesetCount = pGraphics->getTilesetCount();
	unsigned int imageCount = pGraphics->getImageCount();
	if ((tilesetCount > 4096) || (imageCount > 4096)) {
		std::cerr << "Too many images - aborting as this is probably an error." << std::endl;
		return false; // abort
	}

	unsigned int width, height;
	pGraphics->getTileSize(&width, &height);
	if ((tilesetAsSingleImage) && (imageCount > 0)) {
		std::ostringstream ssFilename;
		ssFilename << prefix << ".png";
		if (bScript) {
			std::cout << "id=" << prefix
				<< ";filename=" << ssFilename.str()
				<< ";status=";
		} else {
			std::cout << " extracting: " << ssFilename.str() << std::endl;
		}
		try {
			tilesetToPng(pGraphics, widthTiles, ssFilename.str());
			if (bScript) {
				std::cout << "ok" << std::endl;
			}
		} catch (std::exception& e) {
			if (bScript) {
				std::cout << "fail" << std::endl;
			} else {
				std::cout << " [failed; " << e.what() << "]" << std::endl;
			}
			//iRet = RET_NONCRITICAL_FAILURE; // one or more files failed
		}
	} else {
		for (unsigned int i = 0; i < imageCount; i++) {
			std::ostringstream ssFilename;
			ssFilename << prefix << '+' << i << ".png";
			if (bScript) {
				std::cout << "id=" << prefix << '+' << i
					<< ";filename=" << ssFilename.str()
					<< ";status=";
			} else {
				std::cout << " extracting: " << ssFilename.str() << std::endl;
			}
			try {
				gg::ImageConverterPtr img = pGraphics->openImage(i);
				imageToPng(img, width, height, ssFilename.str());
				if (bScript) {
					std::cout << "ok" << std::endl;
				}
			} catch (std::exception& e) {
				if (bScript) {
					std::cout << "fail" << std::endl;
				} else {
					std::cout << " [failed; " << e.what() << "]" << std::endl;
				}
				//iRet = RET_NONCRITICAL_FAILURE; // one or more files failed
			}
		}
	}
	for (int j = 0; j < tilesetCount; j++) {
		std::ostringstream ss;
		ss << prefix << '.' << j;
		if (!extractAllImages(
			ss.str(), tilesetAsSingleImage, widthTiles,
			pGraphics->getTileset(j), bScript
		)) {
			return false;
		}
	}
	return true; // keep going
}

/// Open a tileset or image from an ID
/**
 * Take the given ID and try to open it.
 *
 * @param  id  The ID to open.
 *
 * @param  img  On return, set to an image or null if the ID is for a tileset.
 *
 * @param  nextTileset  Root tileset to search under.  On return, set to the
 *         tileset containing img, or (if img is null) the tileset matching the
 *         ID, or null on an invalid ID.
 *
 * @return img and nextTileset are set as above.
 */
void openById(const std::string& strArchFile, gg::ImageConverterPtr& img, gg::GraphicsPtr& nextTileset)
{
	std::vector<int> id;
	if (explodeId(&id, strArchFile)) {
		// TODO: Make sure the next line's +1 condition works when it == id.end(),
		//       i.e. there's only images here, no subtilesets
		for (std::vector<int>::iterator i = id.begin() + 1; i != id.end(); i++) {
			if (*i < 0) {
				// Make sure image index is valid
				unsigned int index = (*i + 1) * -1;
				if (index < nextTileset->getImageCount()) {
					img = nextTileset->openImage(index);
				} else {
					std::cout << " [failed; invalid ID]" << std::endl;
					img.reset();
					nextTileset.reset();
				}
				break; // not really necessary but will prevent multiple '+' ids
			} else {
				// Make sure tileset index is valid
				if (*i < nextTileset->getTilesetCount()) {
					nextTileset = nextTileset->getTileset(*i);
				} else {
					std::cout << " [failed; invalid ID]";
					img.reset();
					nextTileset.reset();
					break;
				}
			}
		}
	} else {
		// Invalid ID
		nextTileset.reset();
		img.reset();
	}
	return;
}

int main(int iArgC, char *cArgV[])
{
	// Set a better exception handler
	std::set_terminate( __gnu_cxx::__verbose_terminate_handler );

	// Disable stdin/printf/etc. sync for a speed boost
	std::ios_base::sync_with_stdio(false);

	// Declare the supported options.
	po::options_description poActions("Actions");
	poActions.add_options()
		("list,l",
			"list contents of the graphics file")

		("extract-all-images,X",
			"extract all images as separate files")

		("extract-all-tilesets,T",
			"extract all images, combining into one image file per tileset")

		("extract,x", po::value<std::string>(),
			"extract a specific image")

		("print,p", po::value<std::string>(),
			"print a specific image in ANSI text")

		("overwrite,o", po::value<std::string>(),
			"replace an image with the given file")

		("set-size,z", po::value<std::string>(),
			"set the tileset to the given image size in pixels (e.g. 16x16)")

	;

	po::options_description poOptions("Options");
	poOptions.add_options()
		("type,t", po::value<std::string>(),
			"specify the file format (default is autodetect)")
		("script,s",
			"format output suitable for script parsing")
		("force,f",
			"force open even if the file is not in the given format")
		("width,w", po::value<int>(),
			"width (in tiles) when exporting whole tileset")
	;

	po::options_description poHidden("Hidden parameters");
	poHidden.add_options()
		("file", "file to manipulate")
		("help", "produce help message")
	;

	po::options_description poVisible("");
	poVisible.add(poActions).add(poOptions);

	po::options_description poComplete("Parameters");
	poComplete.add(poActions).add(poOptions).add(poHidden);
	po::variables_map mpArgs;

	std::string strFilename;
	std::string strType;

	bool bScript = false; // show output suitable for script parsing?
	bool bForceOpen = false; // open anyway even if archive not in given format?
	int iTilesetExportWidth = 0;  // Width when exporting whole tileset as single file (0 == entire tileset on one line)
	try {
		po::parsed_options pa = po::parse_command_line(iArgC, cArgV, poComplete);

		// Parse the global command line options
		for (std::vector<po::option>::iterator i = pa.options.begin(); i != pa.options.end(); i++) {
			if (i->string_key.empty()) {
				// If we've already got an archive filename, complain that a second one
				// was given (probably a typo.)
				if (!strFilename.empty()) {
					std::cerr << "Error: unexpected extra parameter (multiple archive "
						"filenames given?!)" << std::endl;
					return 1;
				}
				assert(i->value.size() > 0);  // can't have no values with no name!
				strFilename = i->value[0];
			} else if (i->string_key.compare("help") == 0) {
				std::cout <<
					"Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>\n"
					"This program comes with ABSOLUTELY NO WARRANTY.  This is free software,\n"
					"and you are welcome to change and redistribute it under certain conditions;\n"
					"see <http://www.gnu.org/licenses/> for details.\n"
					"\n"
					"Utility to manipulate graphics files used by games to store images.\n"
					"Build date " __DATE__ " " __TIME__ << "\n"
					"\n"
					"Usage: gamegfx <archive> <action> [action...]\n" << poVisible << "\n"
					<< std::endl;
				return RET_OK;
			} else if (
				(i->string_key.compare("t") == 0) ||
				(i->string_key.compare("type") == 0)
			) {
				if (i->value.size() == 0) {
					std::cerr << PROGNAME ": --type (-t) requires a parameter."
						<< std::endl;
					return RET_BADARGS;
				}
				strType = i->value[0];
			} else if (
				(i->string_key.compare("s") == 0) ||
				(i->string_key.compare("script") == 0)
			) {
				bScript = true;
			} else if (
				(i->string_key.compare("f") == 0) ||
				(i->string_key.compare("force") == 0)
			) {
				bForceOpen = true;
			} else if (
				(i->string_key.compare("w") == 0) ||
				(i->string_key.compare("width") == 0)
			) {
				if (i->value.size() == 0) {
					std::cerr << PROGNAME ": --width (-w) requires a parameter."
						<< std::endl;
					return RET_BADARGS;
				}
				iTilesetExportWidth = strtod(i->value[0].c_str(), NULL);
				if (iTilesetExportWidth < 1) {
					std::cerr << PROGNAME ": --width (-w) must be greater than zero."
						<< std::endl;
					return RET_BADARGS;
				}
			}
		}

		if (strFilename.empty()) {
			std::cerr << "Error: no graphics filename given" << std::endl;
			return RET_BADARGS;
		}
		std::cout << "Opening " << strFilename << " as type "
			<< (strType.empty() ? "<autodetect>" : strType) << std::endl;

		boost::shared_ptr<std::fstream> psGraphics(new std::fstream());
		psGraphics->exceptions(std::ios::badbit | std::ios::failbit);
		try {
			psGraphics->open(strFilename.c_str(), std::ios::in | std::ios::out | std::ios::binary);
		} catch (std::ios::failure& e) {
			std::cerr << "Error opening " << strFilename << std::endl;
			#ifdef DEBUG
				std::cerr << "e.what(): " << e.what() << std::endl;
			#endif
			return RET_SHOWSTOPPER;
		}

		// Get the format handler for this file format
		boost::shared_ptr<gg::Manager> pManager(gg::getManager());

		gg::GraphicsTypePtr pGfxType;
		if (strType.empty()) {
			// Need to autodetect the file format.
			gg::GraphicsTypePtr pTestType;
			int i = 0;
			while ((pTestType = pManager->getGraphicsType(i++))) {
				gg::E_CERTAINTY cert = pTestType->isInstance(psGraphics);
				switch (cert) {
					case gg::EC_DEFINITELY_NO:
						// Don't print anything (TODO: Maybe unless verbose?)
						break;
					case gg::EC_UNSURE:
						std::cout << "File could be a " << pTestType->getFriendlyName()
							<< " [" << pTestType->getCode() << "]" << std::endl;
						// If we haven't found a match already, use this one
						if (!pGfxType) pGfxType = pTestType;
						break;
					case gg::EC_POSSIBLY_YES:
						std::cout << "File is likely to be a " << pTestType->getFriendlyName()
							<< " [" << pTestType->getCode() << "]" << std::endl;
						// Take this one as it's better than an uncertain match
						pGfxType = pTestType;
						break;
					case gg::EC_DEFINITELY_YES:
						std::cout << "File is definitely a " << pTestType->getFriendlyName()
							<< " [" << pTestType->getCode() << "]" << std::endl;
						pGfxType = pTestType;
						// Don't bother checking any other formats if we got a 100% match
						goto finishTesting;
				}
			}
finishTesting:
			if (!pGfxType) {
				std::cerr << "Unable to automatically determine the file type.  Use "
					"the --type option to manually specify the file format." << std::endl;
				return RET_BE_MORE_SPECIFIC;
			}
		} else {
			gg::GraphicsTypePtr pTestType(pManager->getGraphicsTypeByCode(strType));
			if (!pTestType) {
				std::cerr << "Unknown file type given to -t/--type: " << strType
					<< std::endl;
				return RET_BADARGS;
			}
			pGfxType = pTestType;
		}

		assert(pGfxType != NULL);

		// Check to see if the file is actually in this format
		if (!pGfxType->isInstance(psGraphics)) {
			if (bForceOpen) {
				std::cerr << "Warning: " << strFilename << " is not a "
					<< pGfxType->getFriendlyName() << ", open forced." << std::endl;
			} else {
				std::cerr << "Invalid format: " << strFilename << " is not a "
					<< pGfxType->getFriendlyName() << "\n"
					<< "Use the -f option to try anyway." << std::endl;
				return 3;
			}
		}

		// See if the format requires any supplemental files
		gg::MP_SUPPLIST suppList = pGfxType->getRequiredSupps(strFilename);
		gg::MP_SUPPDATA suppData;
		if (suppList.size() > 0) {
			for (gg::MP_SUPPLIST::iterator i = suppList.begin(); i != suppList.end(); i++) {
				try {
					boost::shared_ptr<std::fstream> suppStream(new std::fstream());
					suppStream->exceptions(std::ios::badbit | std::ios::failbit);
					std::cout << "Opening supplemental file " << i->second << std::endl;
					suppStream->open(i->second.c_str(), std::ios::in | std::ios::out | std::ios::binary);
					gg::SuppItem si;
					si.stream = suppStream;
					si.fnTruncate = boost::bind<void>(truncate, i->second.c_str(), _1);
					suppData[i->first] = si;
				} catch (std::ios::failure e) {
					std::cerr << "Error opening supplemental file " << i->second.c_str() << std::endl;
					#ifdef DEBUG
						std::cerr << "e.what(): " << e.what() << std::endl;
					#endif
					return RET_SHOWSTOPPER;
				}
			}
		}

		// Open the graphics file
		gg::GraphicsPtr pGraphics(pGfxType->open(psGraphics, suppData));
		assert(pGraphics);
		pGraphics->fnTruncate = boost::bind<void>(truncate, strFilename.c_str(), _1);

		int iRet = RET_OK;

		// Run through the actions on the command line
		for (std::vector<po::option>::iterator i = pa.options.begin(); i != pa.options.end(); i++) {
			if (i->string_key.compare("list") == 0) {
				printTilesetList("0", pGraphics, bScript);

			} else if (i->string_key.compare("extract-all-images") == 0) {
				extractAllImages("0", false, iTilesetExportWidth, pGraphics, bScript);

			} else if (i->string_key.compare("extract-all-tilesets") == 0) {
				extractAllImages("0", true, iTilesetExportWidth, pGraphics, bScript);

			} else if (i->string_key.compare("extract") == 0) {
				std::string strArchFile, strLocalFile;
				bool bAltDest = split(i->value[0], '=', &strArchFile, &strLocalFile);
				if (!bAltDest) strLocalFile += ".png";

				std::cout << " extracting: " << strArchFile;
				if (bAltDest) std::cout << " (into " << strLocalFile << ")";
				std::cout << std::flush;

				gg::GraphicsPtr nextTileset = pGraphics;
				gg::ImageConverterPtr img;
				openById(strArchFile, img, nextTileset);
				if (img) {
					// We have a single image to extract
					unsigned int width, height;
					nextTileset->getTileSize(&width, &height);
					imageToPng(img, width, height, strLocalFile);
				} else if (nextTileset) {
					// We have an entire tileset to extract
					tilesetToPng(nextTileset, iTilesetExportWidth, strLocalFile);
				} else {
					std::cout << " [failed; invalid ID]";
					iRet = RET_NONCRITICAL_FAILURE;
				}
				std::cout << std::endl;

			} else if (i->string_key.compare("print") == 0) {
				gg::GraphicsPtr nextTileset = pGraphics;
				gg::ImageConverterPtr img;
				const std::string& strArchFile = i->value[0];
				openById(strArchFile, img, nextTileset);
				if (img) {
					// We have a single image to extract
					unsigned int width, height;
					nextTileset->getTileSize(&width, &height);
					imageToANSI(img, width, height);
				} else if (nextTileset) {
					std::cerr << "-p/--print parameter must be an image ID (not a tileset ID.)" << std::endl;
					return RET_BADARGS;
				} else {
					std::cerr << "ERROR: Invalid ID \"" << strArchFile << "\"" << std::endl;
					return RET_BADARGS;
				}

			} else if (i->string_key.compare("overwrite") == 0) {
				std::string strArchFile, strLocalFile;
				bool bAltDest = split(i->value[0], '=', &strArchFile, &strLocalFile);
				if (!bAltDest) strLocalFile += ".png";

				std::cout << "overwriting: " << strArchFile;
				if (bAltDest) std::cout << " (from " << strLocalFile << ")";
				std::cout << std::flush;

				gg::GraphicsPtr nextTileset = pGraphics;
				gg::ImageConverterPtr img;
				openById(strArchFile, img, nextTileset);
				if (img) {
					// We have a single image to overwrite
					unsigned int width, height;
					nextTileset->getTileSize(&width, &height);
					pngToImage(img, width, height, strLocalFile);
				} else if (nextTileset) {
					// We have an entire tileset to extract
					pngToTileset(nextTileset, strLocalFile);
				} else {
					std::cout << " [failed; invalid ID]";
					iRet = RET_NONCRITICAL_FAILURE;
				}
				std::cout << std::endl;

			} else if (i->string_key.compare("set-size") == 0) {
				std::string strArchFile, strNewSize;
				bool bHasSize = split(i->value[0], '=', &strArchFile, &strNewSize);
				if (bHasSize) {
					std::string strWidth, strHeight;
					bool bSizeValue = split(strNewSize, 'x', &strWidth, &strHeight);
					std::cout << "   set size: " << strArchFile;
					if (bSizeValue) {
						int newWidth = strtod(strWidth.c_str(), NULL);
						int newHeight = strtod(strHeight.c_str(), NULL);
						std::cout << " -> " << newWidth << 'x' << newHeight;

						gg::GraphicsPtr nextTileset = pGraphics;
						gg::ImageConverterPtr img;
						openById(strArchFile, img, nextTileset);
						if (img) {
							std::cout << " [failed; can't set the size of an image, use a "
								"tileset ID instead]" << std::endl;
							iRet = RET_NONCRITICAL_FAILURE;
						} else if (nextTileset) {
							if (nextTileset->getCaps() & gg::Graphics::ChangeTileSize) {
								try {
									nextTileset->setTileSize(newWidth, newHeight);
									std::cout << std::endl;
								} catch (std::exception& e) {
									std::cout << " [failed; " << e.what()
										<< ']' << std::endl;
								}
							} else {
								std::cout << " [failed; this file format has a "
									"fixed tile size]" << std::endl;
								// Abort in this case, because any subsequent
								// commands are likely to break if the tile size
								// wasn't changed as expected.
								return RET_BADARGS;
							}
						} else {
							std::cout << " [failed; invalid ID " << strArchFile << ']'
								<< std::endl;
							iRet = RET_NONCRITICAL_FAILURE;
						}
					} else {
						std::cout << " [failed; invalid size]" << std::endl;
						iRet = RET_NONCRITICAL_FAILURE;
					}
				} else {
					std::cerr << "-z/--set-size requires an ID and a size." << std::endl;
					return RET_BADARGS;
				}


			// Ignore --type/-t
			} else if (i->string_key.compare("type") == 0) {
			} else if (i->string_key.compare("t") == 0) {
			// Ignore --script/-s
			} else if (i->string_key.compare("script") == 0) {
			} else if (i->string_key.compare("s") == 0) {
			// Ignore --force/-f
			} else if (i->string_key.compare("force") == 0) {
			} else if (i->string_key.compare("f") == 0) {

			} // else it's the archive filename, but we already have that

		} // for (all command line elements)
	} catch (po::unknown_option& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return 1;
	} catch (po::invalid_command_line_syntax& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return 1;
	}

	return 0;
}
