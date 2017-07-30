/**
 * @file  libgamegraphics/src/main.cpp
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

#include "tls-bash.hpp"
#include "tls-bash-sprite.hpp"
#include "tls-catacomb.hpp"
#include "tls-ccaves-sub.hpp"
#include "tls-ccaves-main.hpp"
#include "tls-ccomic.hpp"
#include "tls-ccomic2.hpp"
#include "tls-cosmo.hpp"
#include "tls-cosmo-actrinfo.hpp"
#include "tls-ddave.hpp"
#include "tls-got.hpp"
#include "tls-harry-chr.hpp"
#include "tls-harry-hsb.hpp"
#include "tls-harry-ico.hpp"
#include "tls-hocus.hpp"
#include "tls-jill.hpp"
#include "tls-nukem2-czone.hpp"
#include "tls-sagent.hpp"
#include "tls-stryker.hpp"
#include "tls-vinyl.hpp"
#include "tls-wacky.hpp"
#include "tls-wordresc.hpp"
#include "tls-zone66.hpp"
#include "tls-zone66-map.hpp"
#include "img-ccomic.hpp"
#include "img-ega-backdrop.hpp"
#include "img-ega-linear.hpp"
#include "img-ega-planar.hpp"
#include "img-mono.hpp"
#include "img-nukem2.hpp"
#include "img-pcx.hpp"
#include "img-pic-raptor.hpp"
#include "img-scr-vinyl.hpp"
#include "img-sw93beta.hpp"
#include "img-sw93beta-planar.hpp"
#include "img-tv-fog.hpp"
#include "img-zone66_tile.hpp"
#include "img-vga-raw.hpp"
#include "img-vga-raw-planar.hpp"
#include "pal-vga-raw.hpp"
#include "pal-gmf-harry.hpp"

using namespace camoto::gamegraphics;

namespace camoto {

template <>
const std::vector<std::shared_ptr<const TilesetType> > CAMOTO_GAMEGRAPHICS_API
	FormatEnumerator<TilesetType>::formats()
{
	std::vector<std::shared_ptr<const TilesetType> > list;
	FormatEnumerator<TilesetType>::addFormat<
		TilesetType_Cosmo_ActrInfo,
		TilesetType_MonsterBashBackground,
		TilesetType_MonsterBashForeground,
		TilesetType_MonsterBashSprite,
		TilesetType_CatacombCGA,
		TilesetType_CatacombEGA,
		TilesetType_CCavesMain,
		TilesetType_CCavesSub,
		TilesetType_CComic,
		TilesetType_CComic_Sprite,
		TilesetType_CComic2,
		TilesetType_Cosmo,
		TilesetType_CosmoMasked,
		TilesetType_DDaveCGA,
		TilesetType_DDaveEGA,
		TilesetType_DDaveVGA,
		TilesetType_GOT,
		TilesetType_HarryCHR,
		TilesetType_HarryHSB,
		TilesetType_HarryICO,
		TilesetType_Hocus,
		TilesetType_Jill,
		TilesetType_Nukem2_CZone,
		TilesetType_SAgent2k,
		TilesetType_SAgent8k,
		TilesetType_Stryker,
		TilesetType_StrykerMasked,
		TilesetType_Vinyl,
		TilesetType_Wacky,
		TilesetType_Wordresc,
		TilesetType_Zone66,
		TilesetType_Zone66Map
	>(list);
	return list;
}

template <>
const std::vector<std::shared_ptr<const ImageType> > CAMOTO_GAMEGRAPHICS_API
	FormatEnumerator<ImageType>::formats()
{
	std::vector<std::shared_ptr<const ImageType> > list;
	FormatEnumerator<ImageType>::addFormat<
		ImageType_CComic,
		ImageType_CosmoBackdrop,
		ImageType_EGA_RawPlanarBGRI,
		ImageType_Mono,
		ImageType_Nukem2Backdrop,
		ImageType_Nukem2,
		ImageType_PCX_LinearCGA,
		ImageType_PCX_LinearVGA,
		ImageType_PCX_LinearVGA_NoRLE,
		ImageType_PCX_PlanarEGA,
		ImageType_RaptorPIC,
		ImageType_SW93Beta_BG,
		ImageType_SW93Beta_Wall,
		ImageType_SW93Beta_Planar,
		ImageType_TVFog,
		ImageType_VGA6Raw,
		ImageType_VGA8Raw,
		ImageType_VGA_Planar_Raw6,
		ImageType_VGA_Planar_Raw8,
		ImageType_VinylSCR,
		ImageType_Zone66Tile,
		ImageType_Palette_HarryGMF,
		ImageType_Palette_VGA,
		ImageType_VGA8Palette
	>(list);
	return list;
}

namespace gamegraphics {

constexpr const char* const ImageType::obj_t_name;
constexpr const char* const TilesetType::obj_t_name;

} // namespace gamegraphics

} // namespace camoto
