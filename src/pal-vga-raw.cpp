/**
 * @file   pal-vga-raw.cpp
 * @brief  Palette interface to 768-byte raw 6-bit VGA palette files.
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
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

#include "pal-vga-raw.hpp"

namespace camoto {
namespace gamegraphics {

VGAPaletteImageType::VGAPaletteImageType()
{
}

VGAPaletteImageType::~VGAPaletteImageType()
{
}

std::string VGAPaletteImageType::getCode() const
{
	return "pal-vga-raw";
}

std::string VGAPaletteImageType::getFriendlyName() const
{
	return "Standard VGA palette";
}

std::vector<std::string> VGAPaletteImageType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pal");
	return vcExtensions;
}

std::vector<std::string> VGAPaletteImageType::getGameList() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty VGAPaletteImageType::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();

	if (len != 768) return DefinitelyNo;

	// See if the first colour is black, which is even more likely to mean it's
	// a VGA palette.
	psImage->seekg(0, stream::start);
	uint8_t buf[3];
	psImage->read((char *)buf, 3);
	if ((buf[0] == 0) && (buf[1] == 0) && (buf[2] == 0)) return DefinitelyYes;

	// TESTED BY: TODO
	return PossiblyYes;
}

ImagePtr VGAPaletteImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new VGAPalette(psImage));
}

ImagePtr VGAPaletteImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new VGAPalette(psImage));
}

SuppFilenames VGAPaletteImageType::getRequiredSupps(const std::string& filenameImage) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


VGAPalette::VGAPalette(stream::inout_sptr data)
	:	data(data)
{
}

VGAPalette::~VGAPalette()
{
}

PaletteTablePtr VGAPalette::getPalette()
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(256);

	uint8_t buf[768];
	memset(buf, 0, 768);
	this->data->seekg(0, stream::start);
	this->data->read(buf, 768);
	// If the palette data is cut off (short read) the rest of the entries will
	// be black.
	int i = 0;
	while (i < 768) {
		PaletteEntry p;
		if (buf[i] >= 0x40) buf[i] = 0x3F;
		p.red   = (buf[i] << 2) | (buf[i] >> 4);
		i++;
		if (buf[i] >= 0x40) buf[i] = 0x3F;
		p.green = (buf[i] << 2) | (buf[i] >> 4);
		i++;
		if (buf[i] >= 0x40) buf[i] = 0x3F;
		p.blue  = (buf[i] << 2) | (buf[i] >> 4);
		i++;
		p.alpha = 255;
		pal->push_back(p);
	}

	return pal;
}

void VGAPalette::setPalette(PaletteTablePtr newPalette)
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
	this->data->seekp(0, stream::start);
	this->data->write(buf, 768);
	return;
}

} // namespace gamegraphics
} // namespace camoto
