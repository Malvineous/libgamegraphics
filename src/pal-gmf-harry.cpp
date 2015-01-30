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

GMFHarryPaletteImageType::GMFHarryPaletteImageType()
{
}

GMFHarryPaletteImageType::~GMFHarryPaletteImageType()
{
}

std::string GMFHarryPaletteImageType::getCode() const
{
	return "pal-gmf-harry";
}

std::string GMFHarryPaletteImageType::getFriendlyName() const
{
	return "Halloween Harry VGA palette";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> GMFHarryPaletteImageType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("gmf");
	return vcExtensions;
}

std::vector<std::string> GMFHarryPaletteImageType::getGameList() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty GMFHarryPaletteImageType::isInstance(stream::input_sptr psImage) const
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

ImagePtr GMFHarryPaletteImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new GMFHarryPalette(psImage));
}

ImagePtr GMFHarryPaletteImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new GMFHarryPalette(psImage));
}

SuppFilenames GMFHarryPaletteImageType::getRequiredSupps(const std::string& filenameImage) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


GMFHarryPalette::GMFHarryPalette(stream::inout_sptr data)
	:	data(data)
{
}

GMFHarryPalette::~GMFHarryPalette()
{
}

PaletteTablePtr GMFHarryPalette::getPalette()
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

void GMFHarryPalette::setPalette(PaletteTablePtr newPalette)
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
