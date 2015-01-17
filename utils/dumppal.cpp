/**
 * @file   dumppal.cpp
 * @brief  Extract the palette from a .png image such as a screenshot.
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
#include <camoto/stream_file.hpp>
#include <iostream>
//#include <fstream>
#include <png++/png.hpp>

#define PROGNAME "dumppal"

namespace po = boost::program_options;
namespace gg = camoto::gamegraphics;
namespace stream = camoto::stream;

int main(int iArgC, char *cArgV[])
{
#ifdef __GLIBCXX__
	// Set a better exception handler
	std::set_terminate(__gnu_cxx::__verbose_terminate_handler);
#endif

	// Disable stdin/printf/etc. sync for a speed boost
	std::ios_base::sync_with_stdio(false);

	// Declare the supported options.
	po::options_description poOptions("Options");
	poOptions.add_options()
		("type,t", po::value<std::string>(),
			"specify the output file format")
	;

	po::options_description poHidden("Hidden parameters");
	poHidden.add_options()
		("in", ".png to read")
		("out", "output palette filename")
		("help", "produce help message")
	;

	po::options_description poVisible("");
	poVisible.add(poOptions);

	po::options_description poComplete("Parameters");
	poComplete.add(poOptions).add(poHidden);
	po::variables_map mpArgs;

	std::string srcFile, dstFile;
	std::string strType;

	try {
		po::parsed_options pa = po::parse_command_line(iArgC, cArgV, poComplete);

		// Parse the global command line options
		for (std::vector<po::option>::iterator i = pa.options.begin(); i != pa.options.end(); i++) {
			if (i->string_key.empty()) {
				// If we've already got an archive filename, complain that a second one
				// was given (probably a typo.)
				if (!dstFile.empty()) {
					std::cerr << "Error: unexpected extra parameter (multiple output "
						"filenames given?!)" << std::endl;
					return 1;
				}
				assert(i->value.size() > 0);  // can't have no values with no name!
				if (srcFile.empty()) srcFile = i->value[0];
				else dstFile = i->value[0];
			} else if (i->string_key.compare("help") == 0) {
				std::cout <<
					"Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>\n"
					"This program comes with ABSOLUTELY NO WARRANTY.  This is free software,\n"
					"and you are welcome to change and redistribute it under certain conditions;\n"
					"see <http://www.gnu.org/licenses/> for details.\n"
					"\n"
					"Utility to manipulate image palettes used by games.\n"
					"Build date " __DATE__ " " __TIME__ << "\n"
					"\n"
					"Usage: " PROGNAME " <input>.png -t <output-type> <output-filename>\n"
					<< poVisible << "\n" << std::endl;
				return 0;
			} else if (
				(i->string_key.compare("t") == 0) ||
				(i->string_key.compare("type") == 0)
			) {
				if (i->value.size() == 0) {
					std::cerr << PROGNAME ": --type (-t) requires a parameter."
						<< std::endl;
					return 1;
				}
				strType = i->value[0];
			}
		}

		if (srcFile.empty()) {
			std::cerr << "Error: no input .png filename given" << std::endl;
			return 1;
		}
		if (dstFile.empty()) {
			std::cerr << "Error: no output filename given" << std::endl;
			return 1;
		}
		if (strType.empty()) {
			std::cerr << "Error: no output format type (-t) given" << std::endl;
			return 1;
		}
		// Get the format handler for this file format
		gg::ManagerPtr pManager(gg::getManager());

		gg::ImageTypePtr palType(pManager->getImageTypeByCode(strType));
		if (!palType) {
			std::cerr << "Unknown file type given to -t/--type: " << strType
				<< std::endl;
			return 1;
		}

		std::cout << "Extracting palette from " << srcFile << " to "
			<< dstFile << " as type " << strType << std::endl;

		png::image<png::index_pixel> png(
			srcFile, png::require_color_space<png::index_pixel>()
		);

		stream::file_sptr outStream(new stream::file());
		try {
			outStream->create(dstFile.c_str());
		} catch (const stream::open_error& e) {
			std::cerr << PROGNAME ": Unable to create " << dstFile << ": " << e.what()
				<< std::endl;
			return 2;
		}

		camoto::SuppData dummy;
		gg::ImagePtr palOut = palType->create(outStream, dummy);

		gg::PaletteTablePtr pal(new gg::PaletteTable());
		png::palette pngPal = png.get_palette();
		for (png::palette::iterator i = pngPal.begin(); i != pngPal.end(); i++) {
			gg::PaletteEntry p;
			p.red = i->red;
			p.green = i->green;
			p.blue = i->blue;
			pal->push_back(p);
		}
		palOut->setPalette(pal);

	} catch (const po::unknown_option& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return 1;
	} catch (const po::invalid_command_line_syntax& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return 1;
	}

	return 0;
}
