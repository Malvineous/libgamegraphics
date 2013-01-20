/**
 * @file   palettetable.cpp
 * @brief  Palette functions, for accessing indexed colour maps used by images.
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

#include <camoto/gamegraphics/palettetable.hpp>

namespace camoto {
namespace gamegraphics {

PaletteTablePtr createPalette_DefaultMono()
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(2);

	for (int i = 0; i < 2; i++) {
		PaletteEntry p;
		p.red = p.green = p.blue = 255 * i;
		p.alpha = 255;
		pal->push_back(p);
	}

	return pal;
}

PaletteTablePtr createPalette_CGA(CGAPaletteType cgaPal)
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(4);

	PaletteTablePtr fullPal = createPalette_FullCGA();

	// Use the lower four bits of cgaPal to select the first colour.
	pal->push_back(fullPal->at(cgaPal & 0x0F));

#define CALC_PAL(t, f) case CGAPal_ ## t: pal->push_back(fullPal->at(f));
	for (int i = 1; i <= 3; i++) {
		switch (cgaPal & 0xF0) {
			CALC_PAL(GreenRed,               i << 1           ); break;
			CALC_PAL(GreenRedBright,    8 | (i << 1)          ); break;
			CALC_PAL(CyanMagenta,           (i << 1) | 1      ); break;
			CALC_PAL(CyanMagentaBright, 8 | (i << 1) | 1      ); break;
			CALC_PAL(CyanRed,               (i << 1) | (i & 1)); break;
			CALC_PAL(CyanRedBright,     8 | (i << 1) | (i & 1)); break;
		}
	}
	return pal;
}

PaletteTablePtr createPalette_FullCGA()
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(16);

	for (int i = 0; i < 16; i++) {
		PaletteEntry p;
		p.red   = (i & 4) ? ((i & 8) ? 0xFF : 0xAA) : ((i & 8) ? 0x55 : 0x00);
		p.green = (i & 2) ? ((i & 8) ? 0xFF : 0xAA) : ((i & 8) ? 0x55 : 0x00);
		p.blue  = (i & 1) ? ((i & 8) ? 0xFF : 0xAA) : ((i & 8) ? 0x55 : 0x00);
		p.alpha = 255;
		if (i == 6) {
			p.green = 0x55;
		}
		pal->push_back(p);
	}

	return pal;
}

PaletteTablePtr createPalette_FullEGA()
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(64);

	uint8_t low = 0x55;

	for (int i = 0; i < 64; i++) {
		PaletteEntry p;
		p.red   = ((i & 4) ? ~low : 0) | ((i & 32) ? low : 0);
		p.green = ((i & 2) ? ~low : 0) | ((i & 16) ? low : 0);
		p.blue  = ((i & 1) ? ~low : 0) | ((i & 8) ? low : 0);
		p.alpha = 255;
		pal->push_back(p);
	}

	return pal;
}

PaletteTablePtr createPalette_DefaultVGA()
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(256);

	// First 16 colours are identical to CGA/EGA
	PaletteTablePtr temp = createPalette_DefaultEGA();
	int i = 0;
	for (; i < 16; i++) {
		pal->push_back(temp->at(i));
	}

	PaletteEntry p;

#define ADD_PAL(r, g, b) \
	p.red = r; \
	p.green = g; \
	p.blue = b; \
	p.alpha = 255; \
	pal->push_back(p); \
	i++;

	// Next 16 colours are greyscale
	// Doesn't seem to be any way to calculate these!
	ADD_PAL(0x00, 0x00, 0x00);
	ADD_PAL(0x15, 0x15, 0x15);
	ADD_PAL(0x22, 0x22, 0x22);
	ADD_PAL(0x2E, 0x2E, 0x2E);
	ADD_PAL(0x3B, 0x3B, 0x3B);
	ADD_PAL(0x44, 0x44, 0x44);
	ADD_PAL(0x55, 0x55, 0x55);
	ADD_PAL(0x66, 0x66, 0x66);
	ADD_PAL(0x77, 0x77, 0x77);
	ADD_PAL(0x88, 0x88, 0x88);
	ADD_PAL(0x99, 0x99, 0x99);
	ADD_PAL(0xAA, 0xAA, 0xAA);
	ADD_PAL(0xBF, 0xBF, 0xBF);
	ADD_PAL(0xCC, 0xCC, 0xCC);
	ADD_PAL(0xEE, 0xEE, 0xEE);
	ADD_PAL(0xFF, 0xFF, 0xFF);

	// Next lot of 72 colours is repeated three times at different intensities
	// Doesn't seem to be any way to calculate these either
	const uint8_t block[] = {
		0x00, 0x00, 0x3F,
		0x10, 0x00, 0x3F,
		0x1F, 0x00, 0x3F,
		0x2F, 0x00, 0x3F,
		0x3F, 0x00, 0x3F,
		0x3F, 0x00, 0x2F,
		0x3F, 0x00, 0x1F,
		0x3F, 0x00, 0x10,
		0x3F, 0x00, 0x00,
		0x3F, 0x10, 0x00,
		0x3F, 0x1F, 0x00,
		0x3F, 0x2F, 0x00,
		0x3F, 0x3F, 0x00,
		0x2F, 0x3F, 0x00,
		0x1F, 0x3F, 0x00,
		0x10, 0x3F, 0x00,
		0x00, 0x3F, 0x00,
		0x00, 0x3F, 0x10,
		0x00, 0x3F, 0x1F,
		0x00, 0x3F, 0x2F,
		0x00, 0x3F, 0x3F,
		0x00, 0x2F, 0x3F,
		0x00, 0x1F, 0x3F,
		0x00, 0x10, 0x3F,
		0x1F, 0x1F, 0x3F,
		0x27, 0x1F, 0x3F,
		0x2F, 0x1F, 0x3F,
		0x37, 0x1F, 0x3F,
		0x3F, 0x1F, 0x3F,
		0x3F, 0x1F, 0x37,
		0x3F, 0x1F, 0x2F,
		0x3F, 0x1F, 0x27,
		0x3F, 0x1F, 0x1F,
		0x3F, 0x27, 0x1F,
		0x3F, 0x2F, 0x1F,
		0x3F, 0x37, 0x1F,
		0x3F, 0x3F, 0x1F,
		0x37, 0x3F, 0x1F,
		0x2F, 0x3F, 0x1F,
		0x27, 0x3F, 0x1F,
		0x1F, 0x3F, 0x1F,
		0x1F, 0x3F, 0x27,
		0x1F, 0x3F, 0x2F,
		0x1F, 0x3F, 0x37,
		0x1F, 0x3F, 0x3F,
		0x1F, 0x37, 0x3F,
		0x1F, 0x2F, 0x3F,
		0x1F, 0x27, 0x3F,
		0x2D, 0x2D, 0x3F,
		0x31, 0x2D, 0x3F,
		0x36, 0x2D, 0x3F,
		0x3A, 0x2D, 0x3F,
		0x3F, 0x2D, 0x3F,
		0x3F, 0x2D, 0x3A,
		0x3F, 0x2D, 0x36,
		0x3F, 0x2D, 0x31,
		0x3F, 0x2D, 0x2D,
		0x3F, 0x31, 0x2D,
		0x3F, 0x36, 0x2D,
		0x3F, 0x3A, 0x2D,
		0x3F, 0x3F, 0x2D,
		0x3A, 0x3F, 0x2D,
		0x36, 0x3F, 0x2D,
		0x31, 0x3F, 0x2D,
		0x2D, 0x3F, 0x2D,
		0x2D, 0x3F, 0x31,
		0x2D, 0x3F, 0x36,
		0x2D, 0x3F, 0x3A,
		0x2D, 0x3F, 0x3F,
		0x2D, 0x3A, 0x3F,
		0x2D, 0x36, 0x3F,
		0x2D, 0x31, 0x3F
	};

	for (unsigned int b = 0; b < 3; b++) {
		double multiplier;
		switch (b) {
			case 0: multiplier = 1.0; break;   // Normal
			case 1: multiplier = 0.46; break;  // Dim
			case 2: multiplier = 0.259; break; // Really dim
		}
		for (unsigned int base = 0; base < 72; base++) {
			unsigned int r = block[base * 3 + 0] * multiplier;
			unsigned int g = block[base * 3 + 1] * multiplier;
			unsigned int b = block[base * 3 + 2] * multiplier;
			ADD_PAL(
				(r << 2) | (r >> 2),
				(g << 2) | (g >> 2),
				(b << 2) | (b >> 2)
			);
		}
	}

	// Final entries are undefined, we'll use black
	while (i < 256) {
		ADD_PAL(0x00, 0x00, 0x00);
	}

	return pal;
}

} // namespace gamegraphics
} // namespace camoto
