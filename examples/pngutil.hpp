/**
 * @file  pngutil.hpp
 * @brief Utility functions for connecting Camoto and libpng.
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

/// Create a default palette for the given colour depth
std::shared_ptr<camoto::gamegraphics::Palette> defaultPalette(
	camoto::gamegraphics::ColourDepth depth)
{
	std::shared_ptr<camoto::gamegraphics::Palette> dstPal;
	switch (depth) {
		case camoto::gamegraphics::ColourDepth::VGA:
			dstPal = camoto::gamegraphics::createPalette_DefaultVGA();
			// Force last colour to be transparent
			dstPal->at(255).red = 255;
			dstPal->at(255).green = 0;
			dstPal->at(255).blue = 192;
			dstPal->at(255).alpha = 0;
			break;
		case camoto::gamegraphics::ColourDepth::EGA:
			dstPal = camoto::gamegraphics::createPalette_DefaultEGA();
			break;
		case camoto::gamegraphics::ColourDepth::CGA:
			dstPal = camoto::gamegraphics::createPalette_CGA(
				camoto::gamegraphics::CGAPaletteType::CyanMagenta);
			break;
		case camoto::gamegraphics::ColourDepth::Mono:
			dstPal = camoto::gamegraphics::createPalette_DefaultMono();
			break;
	}
	return dstPal;
}

/// Prepare the palette for an image export
int preparePalette(camoto::gamegraphics::ColourDepth depth,
	const camoto::gamegraphics::Palette* srcPal, png::palette* pngPal,
	png::tRNS* pngTNS, int* palOffset, int forceXP)
{
	std::shared_ptr<camoto::gamegraphics::Palette> dstPal;
	if (srcPal) {
		// Copy the palette so we can change it later if needed
		dstPal = std::make_shared<camoto::gamegraphics::Palette>(*srcPal);
	} else {
		// Need to use the default palette
		dstPal = defaultPalette(depth);
	}
	*pngPal = png::palette(dstPal->size());
	*pngTNS = png::tRNS(dstPal->size());
	*palOffset = 0;
	int xp = -1; // palette index for transparency, -1 for none
	int j = 0;
	for (auto& i : *dstPal) {
		(*pngPal)[j] = png::color(i.red, i.green, i.blue);
		(*pngTNS)[j] = i.alpha;
		if (i.alpha == 0) xp = j;
		j++;
	}
	if ((dstPal->size() < 255) && (xp < 0)) {
		// Palette has no transparent entry but has room for one, so insert entry at
		// start of palette (this means tRNS block can be only one byte long)
		pngPal->insert(pngPal->begin(), png::color(255, 0, 192));
		pngTNS->insert(pngTNS->begin(), 0);
		*palOffset = 1; // increment all palette indices from now on
		xp = 0;
	}
	if ((xp < 0) && (forceXP >= 0)) {
		// The palette has no transparency, but the caller has indicated it is
		// required.  Use the suggested colour index as transparent.  Typically
		// this colour will have been selected by the caller as it is not used in
		// the image.
		if ((unsigned int)forceXP < pngTNS->size()) {
			(*pngTNS)[forceXP] = 0;
			xp = forceXP;
		} else {
			std::cerr << "BUG: Tried to fallback transparent colour #" << forceXP
				<< " but the palette only contains " << pngTNS->size() << " entries!\n";
		}
	}
	return xp;
}
