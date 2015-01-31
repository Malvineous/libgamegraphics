/**
 * @file  main.cpp
 * @brief Entry point for libgamegraphics.
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

#include <camoto/gamegraphics/manager.hpp>

// Include all the file formats for the Manager to load
#include "tls-actrinfo.hpp"
#include "tls-bash.hpp"
#include "tls-bash-sprite.hpp"
#include "tls-catacomb.hpp"
#include "tls-ccaves-sub.hpp"
#include "tls-ccaves-main.hpp"
#include "tls-ccomic.hpp"
#include "tls-ccomic2.hpp"
#include "tls-cosmo.hpp"
#include "tls-czone.hpp"
#include "tls-ddave.hpp"
#include "tls-got.hpp"
#include "tls-harry-chr.hpp"
#include "tls-harry-hsb.hpp"
#include "tls-harry-ico.hpp"
#include "tls-hocus.hpp"
#include "tls-jill.hpp"
#include "tls-sagent.hpp"
#include "tls-stryker.hpp"
#include "tls-vinyl.hpp"
#include "tls-wacky.hpp"
#include "tls-wordresc.hpp"
#include "tls-zone66.hpp"
#include "tls-zone66-map.hpp"
#include "img-ccomic.hpp"
#include "img-cga.hpp"
#include "img-ega-backdrop.hpp"
#include "img-ega-planar.hpp"
#include "img-mono.hpp"
#include "img-nukem2.hpp"
#include "img-pcx.hpp"
#include "img-pic-raptor.hpp"
#include "img-scr-vinyl.hpp"
#include "img-tv-fog.hpp"
#include "img-zone66_tile.hpp"
#include "img-vga-raw.hpp"
#include "img-vga-raw-planar.hpp"
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
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_Actrinfo()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_MonsterBashBackground()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_MonsterBashForeground()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_MonsterBashSprite()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_CatacombCGA()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_CatacombEGA()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_CCavesMain()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_CCavesSub()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new CComicSpriteType()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_CComic()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_CComic2()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_Cosmo()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_CosmoMasked()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_CZone()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_DDaveCGA()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_DDaveEGA()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_DDaveVGA()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_GOT()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_HarryCHR()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_HarryHSB()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_HarryICO()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_Hocus()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_Jill()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_SAgent2k()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_SAgent8k()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_Stryker()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_StrykerMasked()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_Vinyl()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_Wacky()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_Wordresc()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_Zone66()));
	this->vcTilesetTypes.push_back(TilesetTypePtr(new TilesetType_Zone66Map()));

	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_CComic()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_CosmoBackdrop()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_CGARawLinear()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_EGARawPlanarBGRI()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_Mono()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_Nukem2Backdrop()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_Nukem2()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_PCX_LinearVGA()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_PCX_PlanarEGA()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_RaptorPIC()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_TVFog()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_VGA6Raw()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_VGA8Raw()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_VGA6RawPlanar()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_VGA8RawPlanar()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_VinylSCR()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_Zone66Tile()));

	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_Palette_HarryGMF()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_Palette_VGA()));
	this->vcImageTypes.push_back(ImageTypePtr(new ImageType_VGA8Palette()));
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
