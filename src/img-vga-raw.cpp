/**
 * @file   img-vga-raw.cpp
 * @brief  VGAImage specialisation for fixed-size headerless images.
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
#include "img-vga-raw.hpp"

namespace camoto {
namespace gamegraphics {

VGARawImageType::VGARawImageType()
	throw ()
{
}

VGARawImageType::~VGARawImageType()
	throw ()
{
}

std::string VGARawImageType::getCode() const
	throw ()
{
	return "img-vga-raw-fullscreen";
}

std::string VGARawImageType::getFriendlyName() const
	throw ()
{
	return "Raw VGA fullscreen image";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> VGARawImageType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	return vcExtensions;
}

std::vector<std::string> VGARawImageType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty VGARawImageType::isInstance(stream::input_sptr psImage) const
	throw (stream::error)
{
	stream::pos len = psImage->size();

	// TESTED BY: TODO
	if (len == 64000) return PossiblyYes;

	// TESTED BY: TODO
	return DefinitelyNo;
}

ImagePtr VGARawImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
	throw (stream::error)
{
	psImage->truncate(64000);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 1000; i++) psImage->write(buf, 64);

	ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette]));
	PaletteTablePtr pal = palFile->getPalette();
	return ImagePtr(new VGARawImage(psImage, 320, 200, pal));
}

ImagePtr VGARawImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
	throw (stream::error)
{
	ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette]));
	PaletteTablePtr pal = palFile->getPalette();
	return ImagePtr(new VGARawImage(psImage, 320, 200, pal));
}

SuppFilenames VGARawImageType::getRequiredSupps(const std::string& filenameImage) const
	throw ()
{
	SuppFilenames supps;
	std::string filenameBase =
		filenameImage.substr(0, filenameImage.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".pal";
	return supps;
}


VGARawImage::VGARawImage(stream::inout_sptr data, int width, int height,
	PaletteTablePtr pal)
	throw () :
		VGAImage(data, 0),
		width(width),
		height(height),
		pal(pal)
{
}

VGARawImage::~VGARawImage()
	throw ()
{
}

int VGARawImage::getCaps()
	throw ()
{
	return ColourDepthVGA | HasPalette;
}

void VGARawImage::getDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->width;
	*height = this->height;
	return;
}

void VGARawImage::setDimensions(unsigned int width, unsigned int height)
	throw (stream::error)
{
	throw stream::error("this image is a fixed size");
}

PaletteTablePtr VGARawImage::getPalette()
	throw (stream::error)
{
	return this->pal;
}

} // namespace gamegraphics
} // namespace camoto
