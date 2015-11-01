/**
 * @file  gametls.cpp
 * @brief Command-line interface to libgamegraphics.
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

#include <boost/algorithm/string.hpp> // for case-insensitive string compare
#include <boost/program_options.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/bind.hpp>
#include <camoto/gamegraphics.hpp>
#include <camoto/util.hpp>
#include <camoto/stream_file.hpp>
#include <iostream>
#include "common.hpp"
#include "common-attributes.hpp"

namespace po = boost::program_options;

#define PROGNAME "gametls"

/*** Return values ***/
// All is good
#define RET_OK                 0
// Bad arguments (missing/invalid parameters)
#define RET_BADARGS            1
// Major error (couldn't open tileset file, etc.)
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
void printTilesetList(std::string prefix, gg::Tileset* pTileset, bool bScript)
{
	auto& tiles = pTileset->files();

	if (bScript) {
		std::cout << "id=" << prefix << ";type=tileset";
	} else {
		std::cout << prefix << ": Tileset";
	}

	if (pTileset->caps() & gg::Tileset::Caps::HasDimensions) {
		// This tileset has dimensions
		auto dims = pTileset->dimensions();
		if (bScript) {
			std::cout << ";width=" << dims.x << ";height=" << dims.y;
		} else {
			std::cout << " (" << dims.x << 'x' << dims.y << ")";
		}
	}
	if (bScript) {
		std::cout << ";numitems=" << tiles.size() << '\n';
	} else {
		std::cout << ", ";
		if (tiles.size()) {
			std::cout << tiles.size() << " items\n";
		} else {
			std::cout << "no images" << '\n';
		}
	}

	int j = 0;
	for (auto& i : tiles) {
		// If this is an empty slot skip it, because we can't open it of course
		if (i->fAttr & gg::Tileset::File::Attribute::Vacant) {
			if (bScript) {
				std::cout << "id=" << prefix << '.' << j
					<< ";type=empty\n";
			} else {
				std::cout << prefix << '.' << j << ": Empty slot\n";
			}
		// If this entry is a tileset, open it and dump its contents recursively
		} else if (i->fAttr & gg::Tileset::File::Attribute::Folder) {
			auto copyHandle = i;
			auto tileset = pTileset->openTileset(copyHandle);
			printTilesetList(createString(prefix << '.' << j), &*tileset, bScript);

		// Otherwise if it's an image just dump the specs
		} else {
			auto copyHandle = i;
			auto img = pTileset->openImage(copyHandle);
			auto imgDims = img->dimensions();
			if (bScript) {
				std::cout << "id=" << prefix << '.' << j
					<< ";type=image;width=" << imgDims.x
					<< ";height=" << imgDims.y << '\n';
			} else {
				std::cout << prefix << '.' << j << ": Image (" << imgDims.x << "x"
					<< imgDims.y << ")\n";
			}
		}
		j++;
	}

	return;
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
bool explodeId(std::vector<unsigned int> *id, const std::string& name)
{
	int next = 0;
	bool img = false; // is the next number an image? (if not, it's a tileset)
	for (unsigned int i = 0; i < name.length(); i++) {
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

/// Export a whole tileset to a single .png file
/**
 * Convert the given tileset into a PNG file on disk, by arranging each image
 * in the tileset as a grid of the given width.
 *
 * @param tileset
 *   The tileset to export.
 *
 * @param widthTiles
 *   The width of the exported .png image, in tiles.  0 means as wide as
 *   possible (all tiles on a single row.)
 *
 * @param destFile
 *   Filename of destination (including ".png")
 */
void tilesetToPng(gg::Tileset* tileset, unsigned int widthTiles,
	const std::string& destFile)
{
	if (widthTiles == 0) {
		widthTiles = tileset->layoutWidth();
	}
	if (widthTiles == 0) widthTiles = 16;

	auto& tiles = tileset->files();
	unsigned int numTiles = tiles.size();
	if (widthTiles > numTiles) widthTiles = numTiles;
	unsigned int heightTiles = (numTiles + widthTiles - 1) / widthTiles;

	if (!(tileset->caps() & gg::Tileset::Caps::HasDimensions)) {
		throw stream::error("This only works with tilesets where all tiles "
			"are the same size.");
	}
	auto dims = tileset->dimensions();
	assert((dims.x != 0) && (dims.y != 0));

	png::image<png::index_pixel> png(dims.x * widthTiles, dims.y * heightTiles);

	std::shared_ptr<const gg::Palette> srcPal;
	if (tileset->caps() & gg::Tileset::Caps::HasPalette) {
		srcPal = tileset->palette();
	}

	int forceXP = -1;
	png::palette pngPal;
	png::tRNS pngTNS;
	int palOffset;
	std::vector<bool> clrUsed(256, false);

// Prepare the .png file once.  If during the process, we discover there are
// transparent pixels, but there is no palette transparency, then we will look
// for an unused colour in the palette.  If one is found, we will jump back to
// this label to convert the image a second time, using the unused colour for
// palette transparency on the second run.  This ensures that the only time
// transparency will be dropped is when all 256 colours are in use.
rewrite_png:

	auto transparentIndex = preparePalette(tileset->colourDepth(), srcPal.get(),
		&pngPal, &pngTNS, &palOffset, forceXP);

	// forceXP will be -1 on the first loop, but if the goto brings us back,
	// we must have a transparent index or we'll end up in an infinite loop.
	assert((forceXP < 0) || (transparentIndex >= 0));

	png.set_palette(pngPal);
	if (pngTNS.size() > 0) png.set_tRNS(pngTNS);

	int t = 0;
	bool hasXP = false; // set to true later if a transparent pixel is found
	for (auto& i : tiles) {
		if (i->fAttr & gg::Tileset::File::Attribute::Folder) continue; // aah! tileset! bad!

		auto img = tileset->openImage(i);
		auto data = img->convert();
		auto mask = img->convert_mask();

		unsigned int offX = (t % widthTiles) * dims.x;
		unsigned int offY = (t / widthTiles) * dims.y;

		auto pixelData = data.data();
		auto pixelMask = mask.data();
		for (unsigned int y = 0; y < dims.y; y++) {
			for (unsigned int x = 0; x < dims.x; x++) {
				if (
					(transparentIndex >= 0)
					&& (*pixelMask & (int)gg::Image::Mask::Transparent)
				) {
					png[offY + y][offX + x] = png::index_pixel(transparentIndex);
				} else {
					auto pix = data[y * dims.x + x];
					png[offY + y][offX + x] = png::index_pixel(pix + palOffset);
					clrUsed[pix] = true;
				}
				hasXP = hasXP || (*pixelMask & (int)gg::Image::Mask::Transparent);
				pixelData++;
				pixelMask++;
			}
		}
		t++;
	}

	if ((transparentIndex < 0) && hasXP) {
		// No transparency was put in the image, but transparent pixels were found.
		// See if we can find an unused colour to make transparent.
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
		}
		if (forceXP < 0) {
			// Every single colour in the image is used!
			std::cerr << "Warning: This image uses all 256 colours plus "
				"transparency, so there is no room for palette-based transparency!  "
				"There will be no transparency in the output image.\n";
		} else {
			// We found a color, do it all again
			goto rewrite_png;
		}
	}

	png.write(destFile);
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
void pngToTileset(gg::Tileset* tileset, const std::string& srcFile)
{
	png::image<png::index_pixel> png(srcFile);

	auto dims = tileset->dimensions();
	if ((dims.x == 0) || (dims.y == 0)) {
		throw stream::error("this only works with tilesets where all tiles "
			"are the same size");
	}

	if ((png.get_width() % dims.x) != 0) {
		throw stream::error("image width must be a multiple of the tile width");
	}
	if ((png.get_height() % dims.y) != 0) {
		throw stream::error("image height must be a multiple of the tile height");
	}

	std::shared_ptr<const gg::Palette> tilesetPal;
	if (tileset->caps() & gg::Tileset::Caps::HasPalette) {
		tilesetPal = tileset->palette();
	}
	if (!tilesetPal) {
		// Need to use the default palette
		switch (tileset->colourDepth()) {
			case gg::ColourDepth::VGA:
				tilesetPal = gg::createPalette_DefaultVGA();
				break;
			case gg::ColourDepth::EGA:
				tilesetPal = gg::createPalette_DefaultEGA();
				break;
			case gg::ColourDepth::CGA:
				tilesetPal = gg::createPalette_CGA(gg::CGAPaletteType::CyanMagenta);
				break;
			case gg::ColourDepth::Mono:
				tilesetPal = gg::createPalette_DefaultMono();
				break;
		}
	}

	// -1 means that colour is transparent, 0 == EGA/VGA black, etc.
	std::vector<signed int> paletteMap;

	// Create a palette map in case the .png colours aren't in the same
	// order as the original palette.  This is needed because some image
	// editors (e.g. GIMP) omit colours from the palette if they are
	// unused, messing up the index values.
	paletteMap.resize(256, 0);
	int i_index = 0;
	for (auto& i : png.get_palette()) {
		int j_index = 0;
		for (auto& j : *tilesetPal) {
			if (
				(i.red == j.red) &&
				(i.green == j.green) &&
				(i.blue == j.blue)
			) {
				paletteMap[i_index] = j_index;
			}
			j_index++;
		}
		i_index++;
	}
	png::tRNS transparency = png.get_tRNS();
	for (auto& tx : transparency) {
		// This colour index is transparent
		paletteMap[tx] = -1;
	}

	unsigned int tilesX = png.get_width() / dims.x;
	unsigned int tilesY = png.get_height() / dims.y;
	auto& tiles = tileset->files();
	unsigned int numTiles = tiles.size();
	if (numTiles > tilesX * tilesY) numTiles = tilesX * tilesY;

	unsigned int imgSizeBytes = dims.x * dims.y;
	gg::Pixels pix, mask;
	pix.resize(imgSizeBytes, 0x00);
	mask.resize(imgSizeBytes, 0x00);

	int t = 0;
	for (auto& i : tiles) {
		if (i->fAttr & gg::Tileset::File::Attribute::Folder) continue; // aah! tileset! bad!

		auto copyHandle = i;
		auto img = tileset->openImage(copyHandle);

		unsigned int offX = (t % tilesX) * dims.x;
		unsigned int offY = (t / tilesX) * dims.y;

		for (unsigned int y = 0; y < dims.y; y++) {
			for (unsigned int x = 0; x < dims.x; x++) {
				signed int pixel = paletteMap[png[offY + y][offX + x]];
				if (pixel == -1) { // Palette #0 must be transparent
					mask[y * dims.x + x] = 0x01; // transparent
					pix[y * dims.x + x] = 0x00; // use black in case someone else ignores transparency
				} else {
					mask[y * dims.x + x] = 0x00; // opaque
					pix[y * dims.x + x] = pixel;
				}
			}
		}

		if (img->caps() & gg::Image::Caps::HasPalette) {
			// TODO: This format supports custom palettes, so update it from the
			// PNG image.
			//img->palette(...);
			std::cout << "TODO: Use image's custom palette\n";
		}
		img->convert(pix, mask);
		t++;
	}

	return;
}

/// Export all images in the graphics file as either individual images or
/// tilesets.
/**
 * Extract all images or tilesets and convert them into PNG files.
 *
 * @param prefix
 *   ID prefix (used when recurring, use "0" on first call)
 *
 * @param tilesetAsSingleImage
 *   false to extract each image as a separate .png file, true to combine
 *   images into one .png per tileset
 *
 * @param widthTiles
 *   The width of the exported .png image, in tiles.  0 means as wide as
 *   possible (all tiles on a single row)
 *
 * @param tiles
 *   Tileset file to export
 *
 * @param bScript
 *   true if -s option given (produces easily parseable output)
 */
void extractAllImages(std::string prefix, bool tilesetAsSingleImage,
	int widthTiles, gg::Tileset* tileset, bool bScript)
{
	auto tiles = tileset->files();

	std::shared_ptr<const gg::Palette> palTileset;
	if (tileset->caps() & gg::Tileset::Caps::HasPalette) {
		palTileset = tileset->palette();
	} else {
		palTileset = defaultPalette(tileset->colourDepth());
	}
	int j = 0;
	for (auto& i : tiles) {
		try {
			if (i->fAttr & gg::Tileset::File::Attribute::Folder) {
				if (tilesetAsSingleImage) {
					auto filename = createString(prefix << ".png");
					if (bScript) {
						std::cout << "id=" << prefix << '.' << j
							<< ";filename=" << filename
							<< ";status=";
					} else {
						std::cout << " extracting: " << filename << std::endl;
					}
					try {
						tilesetToPng(tileset, widthTiles, filename);
						if (bScript) {
							std::cout << "ok" << std::endl;
						}
					} catch (const std::exception& e) {
						if (bScript) {
							std::cout << "fail" << std::endl;
						} else {
							std::cout << " [failed; " << e.what() << "]" << std::endl;
						}
						//iRet = RET_NONCRITICAL_FAILURE; // one or more files failed
					}
				} else {
					auto prefix2 = createString(prefix << '.' << j);
					auto sub = tileset->openTileset(i);
					assert(sub); // must throw exception on failure
					extractAllImages(prefix2, tilesetAsSingleImage, widthTiles,
						sub.get(), bScript);
				}

			} else { // single image
				auto filename = createString(prefix << '.' << j << ".png");
				if (bScript) {
					std::cout << "id=" << prefix << '.' << j
						<< ";filename=" << filename
						<< ";status=";
				} else {
					std::cout << " extracting: " << filename << std::endl;
				}
				auto img = tileset->openImage(i);
				auto palUse = palTileset;
				if (img->caps() & gg::Image::Caps::HasPalette) {
					auto palImg = img->palette();
					if (palImg) palUse = palImg;
				}
				imageToPng(*img, filename, palUse);
				if (bScript) {
					std::cout << "ok" << std::endl;
				}
			}
		} catch (const std::exception& e) {
			if (bScript) {
				std::cout << "fail" << std::endl;
			} else {
				std::cout << " [failed; " << e.what() << "]" << std::endl;
			}
			//iRet = RET_NONCRITICAL_FAILURE; // one or more files failed
		}
	j++;
	}

	return;
}

/// Find the FileHandle for the given (user-supplied) ID.
/**
 * Take the given ID and try to open it.
 *
 * @param idText
 *   The ID to open.
 *
 * @param ep
 *   On return, set to an image or null if the ID is for a tileset.
 *
 * @param tileset
 *   Root tileset to search under.  On return, set to the tileset containing ep
 *   or NULL on invalid ID.
 *
 * @return FileHandle to file represented by id, or NULL if the ID specified a
 *   tileset (which is supplied in tileset, which could also be NULL if the ID
 *   was invalid.)
 */
gg::Tileset::FileHandle findById(const std::string& idText,
	std::shared_ptr<gg::Tileset>* tileset)
{
	auto& ts = *tileset;
	gg::Tileset::FileHandle ep;
	std::vector<unsigned int> id;
	if (explodeId(&id, idText)) {
		// TODO: Make sure the next line's +1 condition works when it == id.end(),
		//       i.e. there's only images here, no subtilesets
		for (std::vector<unsigned int>::iterator i = id.begin() + 1; i != id.end(); i++) {

			if (ep) {
				// An image has already been found but there are more ID components,
				// so technically the ID is invalid.
				std::cout << " [failed; invalid ID]" << std::endl;
				*tileset = nullptr;
				ep.reset();
				break;
			}

			auto tiles = ts->files();
			if (*i >= tiles.size()) {
				std::cout << " [failed; invalid ID]" << std::endl;
				*tileset = nullptr;
				break;
			} else {
				if (tiles[*i]->fAttr & gg::Tileset::File::Attribute::Folder) {
					*tileset = ts->openTileset(tiles[*i]);
				} else {
					ep = tiles[*i];
				}
			}

		}
	} else {
		// Invalid ID
		*tileset = nullptr;
	}
	return ep;
}

int main(int iArgC, char *cArgV[])
{
#ifdef __GLIBCXX__
	// Set a better exception handler
	std::set_terminate(__gnu_cxx::__verbose_terminate_handler);
#endif

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

		("insert-image,i", po::value<std::string>(),
			"insert an image at the given ID")

		("insert-tileset,n", po::value<std::string>(),
			"insert an empty tileset at the given ID")

		("metadata,m",
			"list archive attributes/metadata")

		("overwrite,o", po::value<std::string>(),
			"replace an image with the given file")

		("print,p", po::value<std::string>(),
			"print a specific image in ANSI text")

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
		("list-types",
			"list available types that can be passed to --type")
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

	int iRet = RET_OK;
	bool bScript = false; // show output suitable for script parsing?
	bool bForceOpen = false; // open anyway even if tileset not in given format?
	int iTilesetExportWidth = 0;  // Width when exporting whole tileset as single file (0 == entire tileset on one line)
	try {
		po::parsed_options pa = po::parse_command_line(iArgC, cArgV, poComplete);

		// Parse the global command line options
		for (auto& i : pa.options) {
			if (i.string_key.empty()) {
				// If we've already got an tileset filename, complain that a second one
				// was given (probably a typo.)
				if (!strFilename.empty()) {
					std::cerr << "Error: unexpected extra parameter (multiple tileset "
						"filenames given?!)" << std::endl;
					return 1;
				}
				assert(i.value.size() > 0);  // can't have no values with no name!
				strFilename = i.value[0];
			} else if (i.string_key.compare("help") == 0) {
				std::cout <<
					"Copyright (C) 2010-2015 Adam Nielsen <malvineous@shikadi.net>\n"
					"This program comes with ABSOLUTELY NO WARRANTY.  This is free software,\n"
					"and you are welcome to change and redistribute it under certain conditions;\n"
					"see <http://www.gnu.org/licenses/> for details.\n"
					"\n"
					"Utility to edit tileset files used by games to store collections of images.\n"
					"Build date " __DATE__ " " __TIME__ << "\n"
					"\n"
					"Usage: " PROGNAME " <tileset> <action> [action...]\n" << poVisible
					<< "\n" << std::endl;
				return RET_OK;
			} else if (
				(i.string_key.compare("list-types") == 0)
			) {
				for (const auto& f : gg::TilesetManager::formats()) {
					std::string code = f->code();
					std::cout << code;
					int len = code.length();
					if (len < 20) std::cout << std::string(20-code.length(), ' ');
					std::cout << ' ' << f->friendlyName() << '\n';
				}
				return RET_OK;
			} else if (
				(i.string_key.compare("t") == 0) ||
				(i.string_key.compare("type") == 0)
			) {
				if (i.value.size() == 0) {
					std::cerr << PROGNAME ": --type (-t) requires a parameter."
						<< std::endl;
					return RET_BADARGS;
				}
				strType = i.value[0];
			} else if (
				(i.string_key.compare("s") == 0) ||
				(i.string_key.compare("script") == 0)
			) {
				bScript = true;
			} else if (
				(i.string_key.compare("f") == 0) ||
				(i.string_key.compare("force") == 0)
			) {
				bForceOpen = true;
			} else if (
				(i.string_key.compare("w") == 0) ||
				(i.string_key.compare("width") == 0)
			) {
				if (i.value.size() == 0) {
					std::cerr << PROGNAME ": --width (-w) requires a parameter."
						<< std::endl;
					return RET_BADARGS;
				}
				iTilesetExportWidth = strtod(i.value[0].c_str(), NULL);
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

		std::unique_ptr<stream::file> content;
		try {
			content = std::make_unique<stream::file>(strFilename, false);
		} catch (const stream::open_error& e) {
			std::cerr << "Error opening " << strFilename << ": " << e.what()
				<< std::endl;
			return RET_SHOWSTOPPER;
		}

		gg::TilesetManager::handler_t pTilesetType;
		if (strType.empty()) {
			// Need to autodetect the file format.
			for (const auto& i : gg::TilesetManager::formats()) {
				auto cert = i->isInstance(*content);
				switch (cert) {

					case gg::TilesetType::DefinitelyNo:
						// Don't print anything (TODO: Maybe unless verbose?)
						break;

					case gg::TilesetType::Unsure:
						std::cout << "File could be a " << i->friendlyName()
							<< " [" << i->code() << "]" << std::endl;
						// If we haven't found a match already, use this one
						if (!pTilesetType) pTilesetType = i;
						break;

					case gg::TilesetType::PossiblyYes:
						std::cout << "File is likely to be a " << i->friendlyName()
							<< " [" << i->code() << "]" << std::endl;
						// Take this one as it's better than an uncertain match
						pTilesetType = i;
						break;

					case gg::TilesetType::DefinitelyYes:
						std::cout << "File is definitely a " << i->friendlyName()
							<< " [" << i->code() << "]" << std::endl;
						pTilesetType = i;
						// Don't bother checking any other formats if we got a 100% match
						goto finishTesting;
				}
			}
finishTesting:
			if (!pTilesetType) {
				std::cerr << "Unable to automatically determine the file type.  Use "
					"the --type option to manually specify the file format." << std::endl;
				return RET_BE_MORE_SPECIFIC;
			}
		} else {
			pTilesetType = gg::TilesetManager::byCode(strType);
			if (!pTilesetType) {
				std::cerr << "Unknown file type given to -t/--type: " << strType
					<< std::endl;
				return RET_BADARGS;
			}
		}

		assert(pTilesetType != NULL);

		// Check to see if the file is actually in this format
		if (!pTilesetType->isInstance(*content)) {
			if (bForceOpen) {
				std::cerr << "Warning: " << strFilename << " is not a "
					<< pTilesetType->friendlyName() << ", open forced." << std::endl;
			} else {
				std::cerr << "Invalid format: " << strFilename << " is not a "
					<< pTilesetType->friendlyName() << "\n"
					<< "Use the -f option to try anyway." << std::endl;
				return RET_SHOWSTOPPER;
			}
		}

		// See if the format requires any supplemental files
		auto suppList = pTilesetType->getRequiredSupps(*content, strFilename);
		camoto::SuppData suppData;
		for (auto& i : suppList) {
			try {
				std::cout << "Opening supplemental file " << i.second << std::endl;
				suppData[i.first] = std::make_unique<stream::file>(i.second, false);
			} catch (const stream::open_error& e) {
				std::cerr << "Error opening supplemental file " << i.second
					<< ": " << e.what() << std::endl;
				return RET_SHOWSTOPPER;
			}
		}

		// Open the tileset file
		std::shared_ptr<gg::Tileset> pTileset;
		try {
			pTileset = pTilesetType->open(std::move(content), suppData);
			assert(pTileset);
		} catch (const stream::error& e) {
			std::cerr << "Error opening tileset file: " << e.what() << std::endl;
			return RET_SHOWSTOPPER;
		}

		bool modified = false; // have we changed the tileset file?

		// Run through the actions on the command line
		for (auto& i : pa.options) {
			if (i.string_key.compare("list") == 0) {
				printTilesetList("0", pTileset.get(), bScript);

			} else if (i.string_key.compare("extract-all-images") == 0) {
				extractAllImages("0", false, iTilesetExportWidth, pTileset.get(),
					bScript);

			} else if (i.string_key.compare("extract-all-tilesets") == 0) {
				extractAllImages("0", true, iTilesetExportWidth, pTileset.get(),
					bScript);

			} else if (i.string_key.compare("metadata") == 0) {
				listAttributes(pTileset.get(), bScript);

			} else if (i.string_key.compare("extract") == 0) {
				std::string id, strLocalFile;
				bool bAltDest = split(i.value[0], '=', &id, &strLocalFile);
				if (!bAltDest) strLocalFile += ".png";

				if (bScript) {
					std::cout << "extracting=" << id
						<< ";wrote=" << strLocalFile
						<< ";status=";
				} else {
					std::cout << " extracting: " << id;
					if (bAltDest) std::cout << " (into " << strLocalFile << ")";
					std::cout << std::flush;
				}

				auto nextTileset = pTileset;
				auto ep = findById(id, &nextTileset);
				if (!ep) {
					if (nextTileset) {
						// We have an entire tileset to extract
						tilesetToPng(nextTileset.get(), iTilesetExportWidth, strLocalFile);
						if (bScript) std::cout << "ok";
					} else {
						if (bScript) std::cout << "fail";
						else std::cout << " [failed; invalid ID]";
						iRet = RET_NONCRITICAL_FAILURE;
					}
				} else {
					std::shared_ptr<const gg::Palette> palTileset;
					if (nextTileset->caps() & gg::Tileset::Caps::HasPalette) {
						palTileset = nextTileset->palette();
					} else {
						palTileset = defaultPalette(nextTileset->colourDepth());
					}

					// We have a single image to extract
					assert(nextTileset);
					auto img = nextTileset->openImage(ep);
					if (!img) {
						if (bScript) std::cout << "fail";
						else std::cout << " [failed; unable to open image]";
						iRet = RET_NONCRITICAL_FAILURE;
					} else {
						auto palUse = palTileset;
						if (img->caps() & gg::Image::Caps::HasPalette) {
							auto palImg = img->palette();
							if (palImg) palUse = palImg;
						}
						imageToPng(*img, strLocalFile, palUse);
						if (bScript) std::cout << "ok";
					}
				}
				std::cout << std::endl;

			} else if (i.string_key.compare("print") == 0) {
				auto nextTileset = pTileset;
				auto& id = i.value[0];
				auto ep = findById(id, &nextTileset);
				if (!ep) {
					if (nextTileset) {
						std::cerr << "-p/--print parameter must be an image ID (not a tileset ID.)" << std::endl;
						return RET_BADARGS;
					} else {
						std::cerr << "ERROR: Invalid ID \"" << id << "\"" << std::endl;
						return RET_BADARGS;
					}
				} else {
					// We have a single image to extract
					auto img = nextTileset->openImage(ep);
					if (!img) {
						std::cout << " [failed; unable to open image]";
						iRet = RET_NONCRITICAL_FAILURE;
					} else {
						imageToANSI(*img);
					}
				}

			} else if (i.string_key.compare("overwrite") == 0) {
				std::string id, strLocalFile;
				bool bAltDest = split(i.value[0], '=', &id, &strLocalFile);
				if (!bAltDest) strLocalFile += ".png";

				if (bScript) {
					std::cout << "overwriting=" << id
						<< ";read=" << strLocalFile
						<< ";status=";
				} else {
					std::cout << "overwriting: " << id;
					if (bAltDest) std::cout << " (from " << strLocalFile << ")";
					std::cout << std::flush;
				}

				auto nextTileset = pTileset;
				auto ep = findById(id, &nextTileset);
				if (!ep) {
					if (nextTileset) {
						// We have an entire tileset to extract
						pngToTileset(nextTileset.get(), strLocalFile);
						if (bScript) std::cout << "ok";
					} else {
						if (bScript) std::cout << "fail";
						else std::cout << " [failed; invalid ID]";
						iRet = RET_NONCRITICAL_FAILURE;
					}
				} else {
					// We have a single image to overwrite
					assert(nextTileset);
					auto img = nextTileset->openImage(ep);
					if (!img) {
						if (bScript) std::cout << "fail";
						else std::cout << " [failed; unable to open image]";
						iRet = RET_NONCRITICAL_FAILURE;
					} else {
						pngToImage(img.get(), strLocalFile);
						modified = true;
						if (bScript) std::cout << "ok";
					}
				}
				if (nextTileset) nextTileset->flush();
				std::cout << std::endl;

			} else if (i.string_key.compare("insert-image") == 0) {
				std::string id, strLocalFile;
				bool bAltDest = split(i.value[0], '=', &id, &strLocalFile);
				if (!bAltDest) strLocalFile += ".png";

				if (bScript) {
					std::cout << "inserting=" << id
						<< ";read=" << strLocalFile
						<< ";status=";
				} else {
					std::cout << "  inserting: " << id;
					if (bAltDest) std::cout << " (from " << strLocalFile << ")";
					std::cout << std::flush;
				}

				auto nextTileset = pTileset;
				auto epBefore = findById(id, &nextTileset);
				if (!epBefore) {
					if (bScript) std::cout << "fail";
					else std::cout << " [failed; invalid ID]";
					iRet = RET_NONCRITICAL_FAILURE;
				}
				try {
					auto ep = nextTileset->insert(epBefore,
						gg::Tileset::File::Attribute::Default);
					modified = true;

					auto img = nextTileset->openImage(ep);
					if (!img) {
						if (bScript) std::cout << "fail";
						else std::cout << " [failed; unable to open newly inserted image]";
						iRet = RET_NONCRITICAL_FAILURE;
					} else {
						pngToImage(img.get(), strLocalFile);
						if (bScript) std::cout << "ok";
					}
					nextTileset->flush();
				} catch (std::exception& e) {
					if (bScript) std::cout << "fail";
					else std::cout << " [failed; " << e.what() << ']' << std::endl;
				}
				std::cout << std::endl;

			} else if (i.string_key.compare("insert-tileset") == 0) {
				auto id = i.value[0];

				if (bScript) {
					std::cout << "addtileset=" << id
						<< ";status=";
				} else {
					std::cout << "add tileset: " << id << std::flush;
				}

				auto nextTileset = pTileset;
				auto epBefore = findById(id, &nextTileset);
				if (!epBefore) {
					if (bScript) std::cout << "fail";
					else std::cout << " [failed; invalid ID]";
					iRet = RET_NONCRITICAL_FAILURE;
				}
				try {
					auto ep = nextTileset->insert(epBefore,
						gg::Tileset::File::Attribute::Folder);
					modified = true;
					nextTileset->flush();
					if (bScript) std::cout << "ok";
				} catch (std::exception& e) {
					if (bScript) std::cout << "fail";
					else std::cout << " [failed; " << e.what() << ']' << std::endl;
				}
				std::cout << std::endl;

			} else if (i.string_key.compare("set-size") == 0) {
				std::string strId, strNewSize;
				bool bHasSize = split(i.value[0], '=', &strId, &strNewSize);
				if (bHasSize) {
					std::string strWidth, strHeight;
					bool bSizeValue = split(strNewSize, 'x', &strWidth, &strHeight);
					std::cout << "   set size: " << strId;
					if (bSizeValue) {
						long newWidth = (long)strtoul(strWidth.c_str(), NULL, 10);
						long newHeight = (long)strtoul(strHeight.c_str(), NULL, 10);
						std::cout << " -> " << newWidth << 'x' << newHeight;

						auto nextTileset = pTileset;
						auto ep = findById(strId, &nextTileset);
						if (!ep) {
							if (nextTileset) {
								// ID was for a tileset
								if (nextTileset->caps() & gg::Tileset::Caps::SetDimensions) {
									try {
										nextTileset->dimensions({newWidth, newHeight});
										modified = true;
										std::cout << std::endl;
									} catch (std::exception& e) {
										std::cout << " [failed; " << e.what()
											<< ']' << std::endl;
									}
								} else {
									std::cout << " [failed; this file format has a fixed tile size]"
										<< std::endl;
									// Abort in this case, because any subsequent
									// commands are likely to break if the tile size
									// wasn't changed as expected.
									return RET_BADARGS;
								}
							} else {
								std::cout << " [failed; invalid ID " << strId << ']'
									<< std::endl;
								iRet = RET_NONCRITICAL_FAILURE;
							}

						} else {
							// ID was for an image
							auto img = nextTileset->openImage(ep);
							if (img->caps() & gg::Image::Caps::SetDimensions) {
								img->dimensions({newWidth, newHeight});
								modified = true;
								std::cout << std::endl;
							} else {
								std::cout << " [failed; this image's size is fixed]"
									<< std::endl;
								iRet = RET_NONCRITICAL_FAILURE;
							}
						}
						if (nextTileset) nextTileset->flush();
					} else {
						std::cout << " [failed; invalid size]" << std::endl;
						iRet = RET_NONCRITICAL_FAILURE;
					}
				} else {
					std::cerr << "-z/--set-size requires an ID and a size." << std::endl;
					return RET_BADARGS;
				}


			// Ignore --type/-t
			} else if (i.string_key.compare("type") == 0) {
			} else if (i.string_key.compare("t") == 0) {
			// Ignore --script/-s
			} else if (i.string_key.compare("script") == 0) {
			} else if (i.string_key.compare("s") == 0) {
			// Ignore --force/-f
			} else if (i.string_key.compare("force") == 0) {
			} else if (i.string_key.compare("f") == 0) {

			} // else it's the tileset filename, but we already have that

		} // for (all command line elements)

		if (modified) {
			// Save changes
			pTileset->flush();
		}

	} catch (const stream::error& e) {
		std::cerr << PROGNAME ": I/O error - " << e.what() << std::endl;
		return RET_SHOWSTOPPER;

	} catch (const po::unknown_option& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return RET_BADARGS;

	} catch (const po::invalid_command_line_syntax& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return RET_BADARGS;
	}

	return iRet;
}
