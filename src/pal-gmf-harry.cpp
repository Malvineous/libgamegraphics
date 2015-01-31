/**
 * @file  pal-gmf-harry.cpp
 * @brief Interface to the palette stored inside Halloween Harry levels.
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

#include "pal-gmf-harry.hpp"

namespace camoto {
namespace gamegraphics {

ImageType_Palette_HarryGMF::ImageType_Palette_HarryGMF()
{
}

ImageType_Palette_HarryGMF::~ImageType_Palette_HarryGMF()
{
}

std::string ImageType_Palette_HarryGMF::getCode() const
{
	return "pal-gmf-harry";
}

std::string ImageType_Palette_HarryGMF::getFriendlyName() const
{
	return "Halloween Harry VGA palette";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> ImageType_Palette_HarryGMF::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("gmf");
	return vcExtensions;
}

std::vector<std::string> ImageType_Palette_HarryGMF::getGameList() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty ImageType_Palette_HarryGMF::isInstance(stream::input_sptr psImage) const
{
	psImage->seekg(0, stream::start);

	char sig[0x12];
	psImage->read(sig, 0x12);
	if (strncmp(sig, "\x11SubZero Game File", 0x12) != 0) return DefinitelyNo;

	psImage->seekg(0x1D, stream::start);

	// Check palette is within range
	char pal[768];
	psImage->read(pal, 768);
	for (int i = 0; i < 768; i++) {
		// TESTED BY: TODO
		if (pal[i] > 0x40) return DefinitelyNo;
	}

	return DefinitelyYes;
}

ImagePtr ImageType_Palette_HarryGMF::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new Palette_HarryGMF(psImage));
}

ImagePtr ImageType_Palette_HarryGMF::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new Palette_HarryGMF(psImage));
}

SuppFilenames ImageType_Palette_HarryGMF::getRequiredSupps(const std::string& filenameImage) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


Palette_HarryGMF::Palette_HarryGMF(stream::inout_sptr data)
	:	data(data)
{
}

Palette_HarryGMF::~Palette_HarryGMF()
{
}

PaletteTablePtr Palette_HarryGMF::getPalette()
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(256);

	uint8_t buf[768];
	memset(buf, 0, 768);
	data->seekg(0x1d, stream::start);
	data->read((char *)buf, 768);
	// If the palette data is cut off (short read) the rest of the entries will
	// be black.
	int i = 0;
	while (i < 768) {
		PaletteEntry p;
		p.red   = (buf[i] << 2) | (buf[i] >> 4);  i++;
		p.green = (buf[i] << 2) | (buf[i] >> 4);  i++;
		p.blue  = (buf[i] << 2) | (buf[i] >> 4);  i++;
		p.alpha = (i == 3) ? 0 : 255; // first entry is transparency
		pal->push_back(p);
	}

	return pal;
}

void Palette_HarryGMF::setPalette(PaletteTablePtr newPalette)
{
	uint8_t buf[768];
	memset(buf, 0, 768);
	int i = 0;
	for (PaletteTable::const_iterator p = newPalette->begin(); p < newPalette->end(); p++) {
		buf[i++] = p->red >> 2;
		buf[i++] = p->green >> 2;
		buf[i++] = p->blue >> 2;
	}
	this->data->truncate(768);
	this->data->seekp(0x1D, stream::start);
	this->data->write(buf, 768);
	return;
}

} // namespace gamegraphics
} // namespace camoto
