/**
 * @file   img-pcx.cpp
 * @brief  Image specialisation for PCX format images.
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

#include <camoto/iostream_helpers.hpp>
#include "img-pcx.hpp"

namespace camoto {
namespace gamegraphics {

PCXImageType::PCXImageType()
	throw ()
{
}

PCXImageType::~PCXImageType()
	throw ()
{
}

std::string PCXImageType::getCode() const
	throw ()
{
	return "img-pcx";
}

std::string PCXImageType::getFriendlyName() const
	throw ()
{
	return "PCX image";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> PCXImageType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pcx");
	return vcExtensions;
}

std::vector<std::string> PCXImageType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	// Too many to bother listing, and format used outside of games anyway
	return vcGames;
}

ImageType::Certainty PCXImageType::isInstance(iostream_sptr psImage) const
	throw (std::ios::failure)
{
	psImage->seekg(0, std::ios::end);
	unsigned long lenData = psImage->tellg();
	psImage->seekg(0, std::ios::beg);

	boost::shared_ptr<uint8_t> imgData(new uint8_t[lenData]);
	psImage->read((char *)imgData.get(), lenData);

	try {
		Magick::Blob blob;
		blob.update(imgData.get(), lenData);
		Magick::Image img(blob);
		if (img.magick().compare("PCX") == 0) return DefinitelyYes;
	} catch (Magick::Exception& e) {
		return DefinitelyNo;
	}

	// TESTED BY: TODO
	return DefinitelyNo;
}

ImagePtr PCXImageType::create(iostream_sptr psImage,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	// TODO: Create blank PCX
	return ImagePtr(new PCXImage(psImage, fnTruncate));
}

ImagePtr PCXImageType::open(iostream_sptr psImage,
	FN_TRUNCATE fnTruncate, SuppData& suppData) const
	throw (std::ios::failure)
{
	return ImagePtr(new PCXImage(psImage, fnTruncate));
}

SuppFilenames PCXImageType::getRequiredSupps(const std::string& filenameImage) const
	throw ()
{
	return SuppFilenames();
}


PCXImage::PCXImage(iostream_sptr data, FN_TRUNCATE fnTruncate)
	throw (std::ios::failure) :
		data(data),
		fnTruncate(fnTruncate)
{
	this->data->seekg(0, std::ios::end);
	unsigned long lenData = this->data->tellg();
	this->data->seekg(0, std::ios::beg);

	boost::shared_ptr<uint8_t> imgData(new uint8_t[lenData]);
	this->data->read((char *)imgData.get(), lenData);

	try {
		Magick::Blob blob;
		blob.update(imgData.get(), lenData);
		this->img.read(blob);
	} catch (Magick::Exception& e) {
		throw std::ios::failure(e.what());
	}

}

PCXImage::~PCXImage()
	throw ()
{
}

int PCXImage::getCaps()
	throw ()
{
	return Image::CanSetDimensions | Image::HasPalette | Image::CanSetPalette
		| Image::ColourDepthVGA;
}

void PCXImage::getDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->img.columns();
	*height = this->img.rows();
	return;
}

void PCXImage::setDimensions(unsigned int width, unsigned int height)
	throw (std::ios::failure)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	this->img.zoom(Magick::Geometry(width, height));
	return;
}

StdImageDataPtr PCXImage::toStandard()
	throw (std::ios::failure)
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));

	try {
		Magick::Pixels pixCache(this->img);
		pixCache.get(0, 0, width, height);
		Magick::IndexPacket *pixels = pixCache.indexes();

		unsigned long dataSize = width * height;
		uint8_t *imgData = new uint8_t[dataSize];
		StdImageDataPtr ret(imgData);
		for (dataSize++; dataSize > 0; dataSize--) *imgData++ = *pixels++;
		return ret;

	} catch (Magick::Exception& e) {
		throw std::ios::failure(e.what());
	}
}

StdImageDataPtr PCXImage::toStandardMask()
	throw ()
{
	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));
	int dataSize = width * height;

	// Return an entirely opaque mask
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	memset(imgData, 0, dataSize);

	return ret;
}

void PCXImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw (std::ios::failure)
{
	assert(this->fnTruncate);

	unsigned int width, height;
	this->getDimensions(&width, &height);
	assert((width != 0) && (height != 0));

	try {
		Magick::Pixels pixCache(this->img);
		pixCache.get(0, 0, width, height);
		Magick::IndexPacket *pixels = pixCache.indexes();

		unsigned long dataSize = width * height;
		uint8_t *imgData = newContent.get();
		for (int i = 0; i < dataSize; i++) *pixels++ = *imgData++;

		Magick::Blob out;
		this->img.write(&out);

		this->data->seekp(0, std::ios::beg);
		this->data->rdbuf()->sputn((char *)out.data(), out.length());

		this->fnTruncate(out.length());

	} catch (Magick::Exception& e) {
		throw std::ios::failure(e.what());
	}

	return;
}

PaletteTablePtr PCXImage::getPalette()
	throw (std::ios::failure)
{
	int numColours = this->img.colorMapSize();

	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(numColours);

	for (int c = 0; c < numColours; c++) {
		Magick::Color s = this->img.colorMap(c);
		PaletteEntry p;
		p.red = s.redQuantum() * 255 / QuantumRange;
		p.green = s.greenQuantum() * 255 / QuantumRange;
		p.blue = s.blueQuantum() * 255 / QuantumRange;
		p.alpha = 255;
		pal->push_back(p);
	}

	return pal;
}

void PCXImage::setPalette(PaletteTablePtr newPalette)
	throw (std::ios::failure)
{
	int numColours = newPalette->size();

	try {
		this->img.colorMapSize(numColours);

		for (int c = 0; c < numColours; c++) {
			Magick::Color s;
			s.redQuantum((*newPalette)[c].red * QuantumRange / 255);
			s.greenQuantum((*newPalette)[c].green * QuantumRange / 255);
			s.blueQuantum((*newPalette)[c].blue * QuantumRange / 255);
			this->img.colorMap(c, s);
		}
	} catch (Magick::Exception& e) {
		throw std::ios::failure(e.what());
	}

	return;
}

} // namespace gamegraphics
} // namespace camoto
