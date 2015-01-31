/**
 * @file  img-vga-raw-planar.cpp
 * @brief Image_VGA specialisation for planar fixed-size headerless images.
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

#include "pal-vga-raw.hpp"
#include "img-vga-raw-planar.hpp"

namespace camoto {
namespace gamegraphics {

std::vector<std::string> ImageType_VGARawPlanarBase::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pal");
	return vcExtensions;
}

std::vector<std::string> ImageType_VGARawPlanarBase::getGameList() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty ImageType_VGARawPlanarBase::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();

	// TESTED BY: TODO
	if (len == 64000) return PossiblyYes;

	// TESTED BY: TODO
	return DefinitelyNo;
}

ImagePtr ImageType_VGARawPlanarBase::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	psImage->truncate(64000);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 1000; i++) psImage->write(buf, 64);

	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], this->depth));
		pal = palFile->getPalette();
	}
	return ImagePtr(new Image_VGARawPlanar(psImage, 320, 200, pal));
}

ImagePtr ImageType_VGARawPlanarBase::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], this->depth));
		pal = palFile->getPalette();
	}
	return ImagePtr(new Image_VGARawPlanar(psImage, 320, 200, pal));
}

SuppFilenames ImageType_VGARawPlanarBase::getRequiredSupps(const std::string& filenameImage) const
{
	SuppFilenames supps;
	std::string filenameBase =
		filenameImage.substr(0, filenameImage.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".pal";
	return supps;
}


ImageType_VGA6RawPlanar::ImageType_VGA6RawPlanar()
{
	this->depth = 6;
}

std::string ImageType_VGA6RawPlanar::getCode() const
{
	return "img-vga-planar-fullscreen";
}

std::string ImageType_VGA6RawPlanar::getFriendlyName() const
{
	return "Planar VGA fullscreen image (6-bit palette)";
}


ImageType_VGA8RawPlanar::ImageType_VGA8RawPlanar()
{
	this->depth = 8;
}

std::string ImageType_VGA8RawPlanar::getCode() const
{
	return "img-vga-planar8-fullscreen";
}

std::string ImageType_VGA8RawPlanar::getFriendlyName() const
{
	return "Planar VGA fullscreen image (24-bit palette)";
}


Image_VGARawPlanar::Image_VGARawPlanar(stream::inout_sptr data, int width, int height,
	PaletteTablePtr pal)
	:	Image_VGAPlanar(data, 0),
		width(width),
		height(height),
		pal(pal)
{
}

Image_VGARawPlanar::~Image_VGARawPlanar()
{
}

int Image_VGARawPlanar::getCaps()
{
	return ColourDepthVGA | HasPalette;
}

void Image_VGARawPlanar::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->width;
	*height = this->height;
	return;
}

void Image_VGARawPlanar::setDimensions(unsigned int width, unsigned int height)
{
	throw stream::error("this image is a fixed size");
}

PaletteTablePtr Image_VGARawPlanar::getPalette()
{
	return this->pal;
}

} // namespace gamegraphics
} // namespace camoto
