/**
 * @file   main.cpp
 * @brief  Entry point for libgamegraphics.
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

#include <camoto/gamegraphics/manager.hpp>

// Include all the file formats for the Manager to load
#include "tls-actrinfo.hpp"
#include "tls-bash.hpp"
#include "tls-bash-sprite.hpp"
#include "tls-catacomb.hpp"
#include "tls-ccaves-sub.hpp"
#include "tls-ccaves-main.hpp"
#include "tls-ccomic.hpp"
#include "tls-cosmo.hpp"
#include "tls-czone.hpp"
#include "tls-ddave.hpp"
#include "tls-harry-chr.hpp"
#include "tls-harry-hsb.hpp"
#include "tls-harry-ico.hpp"
#include "tls-hocus.hpp"
#include "tls-jill.hpp"
#include "tls-sagent.hpp"
#include "tls-vinyl.hpp"
#include "tls-wacky.hpp"
#include "tls-wordresc.hpp"
#include "tls-zone66.hpp"
#include "tls-zone66-map.hpp"
#include "img-ccomic.hpp"
#include "img-cga.hpp"
#include "img-ega-planar.hpp"
#include "img-mono.hpp"
#include "img-nukem2.hpp"
#include "img-pcx.hpp"
#include "img-pic-raptor.hpp"
#include "img-scr-vinyl.hpp"
#include "img-tv-fog.hpp"
#include "img-zone66_tile.hpp"
#include "img-vga-raw.hpp"
#include "pal-vga-raw.hpp"
#include "pal-gmf-harry.hpp"

namespace camoto {
namespace gamegraphics {

class ActualManager: virtual public Manager
{
	private:
		/// List of available graphics types.
		TilesetTypeVector vcTilesetTypes;
		ImageTypeVector vcImageTypes;

	public:
		ActualManager();
		~ActualManager();

		virtual const TilesetTypePtr getTilesetType(unsigned int iIndex) const;
		virtual const TilesetTypePtr getTilesetTypeByCode(const std::string& strCode) const;
		virtual const ImageTypePtr getImageType(unsigned int iIndex) const;
		virtual const ImageTypePtr getImageTypeByCode(const std::string& strCode) const;
};

const ManagerPtr getManager()
{
	return ManagerPtr(new ActualManager());
}

ActualManager::ActualManager()
{
	this->vcTilesetTypes.push_back(TilesetTypePtr(new ActrinfoTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new MonsterBashBackgroundTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new MonsterBashForegroundTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new MonsterBashSpriteTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new CatacombCGATilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new CatacombEGATilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new CCavesMainTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new CCavesSubTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new CComicSpriteType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new CComicTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new CosmoTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new CosmoMaskedTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new CZoneTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new DDaveCGATilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new DDaveEGATilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new DDaveVGATilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new HarryCHRTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new HarryHSBTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new HarryICOTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new HocusTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new JillTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new SAgent2kTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new SAgent8kTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new VinylTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new WackyTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new WordrescTilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new Zone66TilesetType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new Zone66MapTilesetType()));

	this->vcImageTypes.push_back(ImageTypePtr(new CComicImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new CGARawLinearImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new EGARawPlanarBGRIImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new MonoImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new Nukem2ImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new PCX_LinearVGA_ImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new PCX_PlanarEGA_ImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new RaptorPICImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new TVFogImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new VGA6RawImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new VGA8RawImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new VinylSCRImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new Zone66TileImageType()));

	this->vcImageTypes.push_back(ImageTypePtr(new GMFHarryPaletteImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new VGAPaletteImageType()));
	this->vcImageTypes.push_back(ImageTypePtr(new VGA8PaletteImageType()));
}

ActualManager::~ActualManager()
{
}

const TilesetTypePtr ActualManager::getTilesetType(unsigned int iIndex) const
{
	if (iIndex >= this->vcTilesetTypes.size()) return TilesetTypePtr();
	return this->vcTilesetTypes[iIndex];
}

const TilesetTypePtr ActualManager::getTilesetTypeByCode(
	const std::string& strCode) const
{
	for (TilesetTypeVector::const_iterator i = this->vcTilesetTypes.begin();
		i != this->vcTilesetTypes.end();
		i++
	) {
		if ((*i)->getCode().compare(strCode) == 0) return *i;
	}
	return TilesetTypePtr();
}

const ImageTypePtr ActualManager::getImageType(unsigned int iIndex) const
{
	if (iIndex >= this->vcImageTypes.size()) return ImageTypePtr();
	return this->vcImageTypes[iIndex];
}

const ImageTypePtr ActualManager::getImageTypeByCode(const std::string& strCode)
	const
{
	for (ImageTypeVector::const_iterator i = this->vcImageTypes.begin();
		i != this->vcImageTypes.end();
		i++
	) {
		if ((*i)->getCode().compare(strCode) == 0) return *i;
	}
	return ImageTypePtr();
}

} // namespace gamegraphics
} // namespace camoto
