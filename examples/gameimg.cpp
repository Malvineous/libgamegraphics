/**
 * @file  gameimg.cpp
 * @brief Command-line interface to single image parts of libgamegraphics.
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
#include <camoto/gamegraphics.hpp>
#include <camoto/util.hpp>
#include <camoto/stream_file.hpp>
#include <iostream>
#include "common.hpp"

namespace po = boost::program_options;
namespace gg = camoto::gamegraphics;

#define PROGNAME "gameimg"

/*** Return values ***/
// All is good
#define RET_OK                 0
// Bad arguments (missing/invalid parameters)
#define RET_BADARGS            1
// Major error (couldn't open image file, etc.)
#define RET_SHOWSTOPPER        2
// More info needed (-t auto didn't work, specify a type)
#define RET_BE_MORE_SPECIFIC   3
// One or more files failed, probably user error (file not found, etc.)
#define RET_NONCRITICAL_FAILURE 4
// Some files failed, but not in a common way (cut off write, disk full, etc.)
#define RET_UNCOMMON_FAILURE   5

/// @see libgamearchive/examples/gamearch.cpp
/// find_last_of() changed to find_first_of() here so equal signs can be put
/// into instrument names, e.g. for URLs
bool split(const std::string& in, char delim, std::string *out1, std::string *out2)
{
	std::string::size_type iEqualPos = in.find_first_of(delim);
	*out1 = in.substr(0, iEqualPos);
	// Does the destination have a different filename?
	bool bAltDest = iEqualPos != std::string::npos;
	*out2 = bAltDest ? in.substr(iEqualPos + 1) : *out1;
	return bAltDest;
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

		("print,p",
			"print the image in ANSI text")

		("extract,x", po::value<std::string>(),
			"convert the image to a .png file")

		("overwrite,o", po::value<std::string>(),
			"replace the image with the given .png")

		("palette,l", po::value<std::string>(),
			"replace the image's palette with the one loaded from the given file")

//		("set-size,z", po::value<std::string>(),
//			"set the tileset to the given image size in pixels (e.g. 16x16)")

	;

	po::options_description poOptions("Options");
	poOptions.add_options()
		("type,t", po::value<std::string>(),
			"specify the file format (default is autodetect)")
		("script,s",
			"format output suitable for script parsing")
		("force,f",
			"force open even if the file is not in the given format")
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

	bool bForceOpen = false; // open anyway even if image not in given format?
	try {
		po::parsed_options pa = po::parse_command_line(iArgC, cArgV, poComplete);

		// Parse the global command line options
		for (auto& i : pa.options) {
			if (i.string_key.empty()) {
				// If we've already got an image filename, complain that a second one
				// was given (probably a typo.)
				if (!strFilename.empty()) {
					std::cerr << "Error: unexpected extra parameter (multiple image "
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
					"Utility to manipulate graphics files used by games to store images.\n"
					"Build date " __DATE__ " " __TIME__ << "\n"
					"\n"
					"Usage: gameimg <game image filename> <action> [action...]\n"
					<< poVisible << "\n"
					<< std::endl;
				return RET_OK;

			} else if (i.string_key.compare("list-types") == 0) {
				for (const auto& f : gg::ImageManager::formats()) {
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
				(i.string_key.compare("f") == 0) ||
				(i.string_key.compare("force") == 0)
			) {
				bForceOpen = true;
			}
		}

		if (strFilename.empty()) {
			std::cerr << "Error: no image filename given" << std::endl;
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

		gg::ImageManager::handler_t pImageType;
		if (strType.empty()) {
			// Need to autodetect the file format.
			for (const auto& i : gg::ImageManager::formats()) {
				auto cert = i->isInstance(*content);
				switch (cert) {
					case gg::ImageType::DefinitelyNo:
						// Don't print anything (TODO: Maybe unless verbose?)
						break;
					case gg::ImageType::Unsure:
						std::cout << "File could be a " << i->friendlyName()
							<< " [" << i->code() << "]" << std::endl;
						// If we haven't found a match already, use this one
						if (!pImageType) pImageType = i;
						break;
					case gg::ImageType::PossiblyYes:
						std::cout << "File is likely to be a " << i->friendlyName()
							<< " [" << i->code() << "]" << std::endl;
						// Take this one as it's better than an uncertain match
						pImageType = i;
						break;
					case gg::ImageType::DefinitelyYes:
						std::cout << "File is definitely a " << i->friendlyName()
							<< " [" << i->code() << "]" << std::endl;
						pImageType = i;
						// Don't bother checking any other formats if we got a 100% match
						goto finishTesting;
				}
			}
finishTesting:
			if (!pImageType) {
				std::cerr << "Unable to automatically determine the file type.  Use "
					"the --type option to manually specify the file format." << std::endl;
				return RET_BE_MORE_SPECIFIC;
			}
		} else {
			pImageType = gg::ImageManager::byCode(strType);
			if (!pImageType) {
				std::cerr << "Unknown file type given to -t/--type: " << strType
					<< std::endl;
				return RET_BADARGS;
			}
		}

		assert(pImageType != NULL);

		// Check to see if the file is actually in this format
		if (!pImageType->isInstance(*content)) {
			if (bForceOpen) {
				std::cerr << "Warning: " << strFilename << " is not a "
					<< pImageType->friendlyName() << ", open forced." << std::endl;
			} else {
				std::cerr << "Invalid format: " << strFilename << " is not a "
					<< pImageType->friendlyName() << "\n"
					<< "Use the -f option to try anyway." << std::endl;
				return RET_SHOWSTOPPER;
			}
		}

		// See if the format requires any supplemental files
		auto suppList = pImageType->getRequiredSupps(*content, strFilename);
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

		// Open the image file
		std::shared_ptr<gg::Image> pImage;
		try {
			pImage = pImageType->open(std::move(content), suppData);
			assert(pImage);
		} catch (const stream::error& e) {
			std::cerr << "Error opening image file: " << e.what() << std::endl;
			return RET_SHOWSTOPPER;
		}

		auto lastPal = pImage->palette();

		// Run through the actions on the command line
		for (auto& i : pa.options) {

			if (i.string_key.compare("extract") == 0) {
				imageToPng(*pImage, i.value[0], lastPal);

			} else if (i.string_key.compare("print") == 0) {
				imageToANSI(*pImage);

			} else if (i.string_key.compare("overwrite") == 0) {
				pngToImage(&*pImage, i.value[0]);

			} else if (i.string_key.compare("palette") == 0) {
				std::string palType;
				std::string palFile;
				if (!split(i.value[0], ':', &palType, &palFile)) {
					palType.clear(); // no type given, autodetect
				}

				std::unique_ptr<stream::file> palContent;
				try {
					palContent = std::make_unique<stream::file>(palFile, false);
				} catch (const stream::open_error& e) {
					std::cerr << "Error opening " << palFile << ": " << e.what()
						<< std::endl;
					return RET_SHOWSTOPPER;
				}

				gg::ImageManager::handler_t pPalType;
				if (palType.empty()) {
					// Need to autodetect the file format.
					for (const auto& i : gg::ImageManager::formats()) {
						auto cert = i->isInstance(*palContent);
						switch (cert) {
							case gg::ImageType::DefinitelyNo:
								// Don't print anything (TODO: Maybe unless verbose?)
								break;
							case gg::ImageType::Unsure:
								std::cout << "File could be a " << i->friendlyName()
									<< " [" << i->code() << "]" << std::endl;
								// If we haven't found a match already, use this one
								if (!pPalType) pPalType = i;
								break;
							case gg::ImageType::PossiblyYes:
								std::cout << "File is likely to be a " << i->friendlyName()
									<< " [" << i->code() << "]" << std::endl;
								// Take this one as it's better than an uncertain match
								pPalType = i;
								break;
							case gg::ImageType::DefinitelyYes:
								std::cout << "File is definitely a " << i->friendlyName()
									<< " [" << i->code() << "]" << std::endl;
								pPalType = i;
								// Don't bother checking any other formats if we got a 100% match
								goto palFinishTesting;
						}
					}
palFinishTesting:
					if (!pPalType) {
						std::cerr << "Unable to automatically determine the palette type.  "
							"Use --palette <type>:<filename> to manually specify the file "
							"format." << std::endl;
						return RET_BE_MORE_SPECIFIC;
					}
				} else {
					pPalType = gg::ImageManager::byCode(strType);
					if (!pPalType) {
						std::cerr << "Unknown file type given to -t/--type: " << strType
							<< std::endl;
						return RET_BADARGS;
					}
				}

				assert(pPalType != NULL);

				// Check to see if the file is actually in this format
				if (!pPalType->isInstance(*palContent)) {
					if (bForceOpen) {
						std::cerr << "Warning: " << strFilename << " is not a "
							<< pPalType->friendlyName() << ", open forced." << std::endl;
					} else {
						std::cerr << "Invalid format: " << strFilename << " is not a "
							<< pPalType->friendlyName() << "\n"
							<< "Use the -f option to try anyway." << std::endl;
						return RET_SHOWSTOPPER;
					}
				}

				// See if the format requires any supplemental files
				auto suppList = pPalType->getRequiredSupps(*content, strFilename);
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

				// Open the image file
				std::shared_ptr<gg::Image> pPal;
				try {
					pPal = pPalType->open(std::move(palContent), suppData);
					assert(pPal);
				} catch (const stream::error& e) {
					std::cerr << "Error opening palette file: " << e.what() << std::endl;
					return RET_SHOWSTOPPER;
				}

				// Remember the palette
				lastPal = pPal->palette();

			// Ignore --type/-t
			} else if (i.string_key.compare("type") == 0) {
			} else if (i.string_key.compare("t") == 0) {
			// Ignore --script/-s
			} else if (i.string_key.compare("script") == 0) {
			} else if (i.string_key.compare("s") == 0) {
			// Ignore --force/-f
			} else if (i.string_key.compare("force") == 0) {
			} else if (i.string_key.compare("f") == 0) {

			} // else it's the image filename, but we already have that

		} // for (all command line elements)

	} catch (po::unknown_option& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return RET_BADARGS;

	} catch (po::invalid_command_line_syntax& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return RET_BADARGS;
	}

	return RET_OK;
}
