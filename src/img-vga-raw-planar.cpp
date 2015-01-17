/**
 * @file   img-vga-raw-planar.cpp
 * @brief  VGAImage specialisation for planar fixed-size headerless images.
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

std::vector<std::string> VGARawPlanarBaseImageType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pal");
	return vcExtensions;
}

std::vector<std::string> VGARawPlanarBaseImageType::getGameList() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty VGARawPlanarBaseImageType::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();

	// TESTED BY: TODO
	if (len == 64000) return PossiblyYes;

	// TESTED BY: TODO
	return DefinitelyNo;
}

ImagePtr VGARawPlanarBaseImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	psImage->truncate(64000);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 1000; i++) psImage->write(buf, 64);

	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], this->depth));
		pal = palFile->getPalette();
	}
	return ImagePtr(new VGARawPlanarImage(psImage, 320, 200, pal));
}

ImagePtr VGARawPlanarBaseImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], this->depth));
		pal = palFile->getPalette();
	}
	return ImagePtr(new VGARawPlanarImage(psImage, 320, 200, pal));
}

SuppFilenames VGARawPlanarBaseImageType::getRequiredSupps(const std::string& filenameImage) const
{
	SuppFilenames supps;
	std::string filenameBase =
		filenameImage.substr(0, filenameImage.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".pal";
	return supps;
}


VGA6RawPlanarImageType::VGA6RawPlanarImageType()
{
	this->depth = 6;
}

std::string VGA6RawPlanarImageType::getCode() const
{
	return "img-vga-planar-fullscreen";
}

std::string VGA6RawPlanarImageType::getFriendlyName() const
{
	return "Planar VGA fullscreen image (6-bit palette)";
}


VGA8RawPlanarImageType::VGA8RawPlanarImageType()
{
	this->depth = 8;
}

std::string VGA8RawPlanarImageType::getCode() const
{
	return "img-vga-planar8-fullscreen";
}

std::string VGA8RawPlanarImageType::getFriendlyName() const
{
	return "Planar VGA fullscreen image (24-bit palette)";
}


VGARawPlanarImage::VGARawPlanarImage(stream::inout_sptr data, int width, int height,
	PaletteTablePtr pal)
	:	VGAPlanarImage(data, 0),
		width(width),
		height(height),
		pal(pal)
{
}

VGARawPlanarImage::~VGARawPlanarImage()
{
}

int VGARawPlanarImage::getCaps()
{
	return ColourDepthVGA | HasPalette;
}

void VGARawPlanarImage::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->width;
	*height = this->height;
	return;
}

void VGARawPlanarImage::setDimensions(unsigned int width, unsigned int height)
{
	throw stream::error("this image is a fixed size");
}

PaletteTablePtr VGARawPlanarImage::getPalette()
{
	return this->pal;
}

} // namespace gamegraphics
} // namespace camoto
