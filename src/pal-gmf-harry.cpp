/**
 * @file   pal-gmf-harry.cpp
 * @brief  Interface to the palette stored inside Halloween Harry levels.
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

#include <boost/iostreams/stream.hpp>
#include "pal-gmf-harry.hpp"

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

GMFHarryPaletteImageType::GMFHarryPaletteImageType()
	throw ()
{
}

GMFHarryPaletteImageType::~GMFHarryPaletteImageType()
	throw ()
{
}

std::string GMFHarryPaletteImageType::getCode() const
	throw ()
{
	return "pal-gmf-harry";
}

std::string GMFHarryPaletteImageType::getFriendlyName() const
	throw ()
{
	return "Halloween Harry VGA palette";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> GMFHarryPaletteImageType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("gmf");
	return vcExtensions;
}

std::vector<std::string> GMFHarryPaletteImageType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty GMFHarryPaletteImageType::isInstance(iostream_sptr psImage) const
	throw (std::ios::failure)
{
	psImage->seekg(0, std::ios::beg);

	char sig[0x12];
	psImage->read(sig, 0x12);
	if (strncmp(sig, "\x11SubZero Game File", 0x12) != 0) return DefinitelyNo;

	psImage->seekg(0x1D, std::ios::beg);

	// Check palette is within range
	char pal[768];
	psImage->read(pal, 768);
	for (int i = 0; i < 768; i++) {
		// TESTED BY: TODO
		if (pal[i] > 0x40) return DefinitelyNo;
	}

	return DefinitelyYes;
}

ImagePtr GMFHarryPaletteImageType::create(iostream_sptr psImage,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	return ImagePtr(new GMFHarryPalette(psImage, fnTruncate));
}

ImagePtr GMFHarryPaletteImageType::open(iostream_sptr psImage,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	return ImagePtr(new GMFHarryPalette(psImage, fnTruncate));
}

SuppFilenames GMFHarryPaletteImageType::getRequiredSupps(const std::string& filenameImage) const
	throw ()
{
	// No supplemental types/empty list
	return SuppFilenames();
}


GMFHarryPalette::GMFHarryPalette(iostream_sptr data, FN_TRUNCATE fnTruncate)
	throw (std::ios::failure) :
		data(data),
		fnTruncate(fnTruncate)
{
}

GMFHarryPalette::~GMFHarryPalette()
	throw ()
{
}

PaletteTablePtr GMFHarryPalette::getPalette()
	throw (std::ios::failure)
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(256);

	uint8_t buf[768];
	memset(buf, 0, 768);
	data->seekg(0x1d, std::ios::beg);
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
	throw (std::ios::failure)
{
	uint8_t buf[768];
	memset(buf, 0, 768);
	int i = 0;
	for (PaletteTable::const_iterator p = newPalette->begin(); p < newPalette->end(); p++) {
		buf[i++] = p->red >> 2;
		buf[i++] = p->green >> 2;
		buf[i++] = p->blue >> 2;
	}
	this->fnTruncate(768);
	this->data->seekp(0x1D, std::ios::beg);
	if (this->data->rdbuf()->sputn((char *)buf, 768) != 768) {
		throw std::ios::failure("Unable to write palette to stream (disk full?)");
	}
	return;
}

} // namespace gamegraphics
} // namespace camoto
