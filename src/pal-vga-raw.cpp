/**
 * @file   pal-vga-raw.cpp
 * @brief  Palette interface to 768-byte raw 6-bit VGA palette files.
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

#include <boost/iostreams/stream.hpp>
#include "pal-vga-raw.hpp"

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

VGAPaletteImageType::VGAPaletteImageType()
	throw ()
{
}

VGAPaletteImageType::~VGAPaletteImageType()
	throw ()
{
}

std::string VGAPaletteImageType::getCode() const
	throw ()
{
	return "pal-vga-raw";
}

std::string VGAPaletteImageType::getFriendlyName() const
	throw ()
{
	return "Standard VGA palette";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> VGAPaletteImageType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pal");
	return vcExtensions;
}

std::vector<std::string> VGAPaletteImageType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty VGAPaletteImageType::isInstance(iostream_sptr psImage) const
	throw (std::ios::failure)
{
	psImage->seekg(0, std::ios::end);
	io::stream_offset len = psImage->tellg();

	if (len != 768) return DefinitelyNo;

	// See if the first colour is black, which is even more likely to mean it's
	// a VGA palette.
	psImage->seekg(0, std::ios::beg);
	uint8_t buf[3];
	psImage->read((char *)buf, 3);
	if ((buf[0] == 0) && (buf[1] == 0) && (buf[2] == 0)) return DefinitelyYes;

	// TESTED BY: TODO //fmt_grp_duke3d_isinstance_c01
	return PossiblyYes;
}

ImagePtr VGAPaletteImageType::create(iostream_sptr psImage,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	return ImagePtr(new VGAPalette(psImage, fnTruncate));
}

ImagePtr VGAPaletteImageType::open(iostream_sptr psImage,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	return ImagePtr(new VGAPalette(psImage, fnTruncate));
}

MP_SUPPLIST VGAPaletteImageType::getRequiredSupps(const std::string& filenameImage) const
	throw ()
{
	// No supplemental types/empty list
	return MP_SUPPLIST();
}


VGAPalette::VGAPalette(iostream_sptr data, FN_TRUNCATE fnTruncate)
	throw (std::ios::failure) :
		data(data),
		fnTruncate(fnTruncate)
{
}

VGAPalette::~VGAPalette()
	throw ()
{
}

PaletteTablePtr VGAPalette::getPalette()
	throw (std::ios::failure)
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(256);

	uint8_t buf[768];
	memset(buf, 0, 768);
	data->read((char *)buf, 768);
	// If the palette data is cut off (short read) the rest of the entries will
	// be black.
	int i = 0;
	while (i < 768) {
		PaletteEntry p;
		/*p.red = (buf[i] > 0x3F) ? 255 : buf[i] * 255 / 63;     i++;
		p.green = (buf[i] > 0x3F) ? 255 : buf[i] * 255 / 63;   i++;
		p.blue  = (buf[i] > 0x3F) ? 255 : buf[i] * 255 / 63;    i++;*/
		p.red   = (buf[i] << 2) | (buf[i] >> 4);  i++;
		p.green = (buf[i] << 2) | (buf[i] >> 4);  i++;
		p.blue  = (buf[i] << 2) | (buf[i] >> 4);  i++;
		pal->push_back(p);
	}

	return pal;
}

void VGAPalette::setPalette(PaletteTablePtr newPalette)
	throw (std::ios::failure)
{
	uint8_t buf[768];
	memset(buf, 0, 768);
	int i = 0;
	for (PaletteTable::const_iterator p = newPalette->begin(); p < newPalette->end(); p++) {
		buf[i++] = p->red * 63 / 255;
		buf[i++] = p->green * 63 / 255;
		buf[i++] = p->blue * 63 / 255;
	}
	this->fnTruncate(768);
	this->data->seekp(0, std::ios::beg);
	if (this->data->rdbuf()->sputn((char *)buf, 768) != 768) {
		throw std::ios::failure("Unable to write palette to stream (disk full?)");
	}
	return;
}

} // namespace gamegraphics
} // namespace camoto
