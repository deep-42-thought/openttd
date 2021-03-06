/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file tile_map.h Map writing/reading functions for tiles. */

#ifndef TILE_MAP_H
#define TILE_MAP_H

#include "slope_type.h"
#include "map_func.h"
#include "core/bitmath_func.hpp"
#include "settings_type.h"

/**
 * Returns the height of a tile
 *
 * This function returns the height of the northern corner of a tile.
 * This is saved in the global map-array. It does not take affect by
 * any slope-data of the tile.
 *
 * @param tile The tile to get the height from
 * @return the height of the tile
 * @pre IsValidTileIndex(tile)
 */
template <bool Tgeneric>
static inline uint TileHeight(typename TileIndexT<Tgeneric>::T tile)
{
	assert(IsValidTileIndex(tile));
	return GetTile(tile)->height;
}
/** @copydoc TileHeight(TileIndexT<Tgeneric>::T) */
static inline uint TileHeight(TileIndex tile) { return TileHeight<false>(tile); }
/** @copydoc TileHeight(TileIndexT<Tgeneric>::T) */
static inline uint TileHeight(GenericTileIndex tile) { return TileHeight<true>(tile); }

uint TileHeightOutsideMap(int x, int y);

/**
 * Sets the height of a tile.
 *
 * This function sets the height of the northern corner of a tile.
 *
 * @param tile The tile to change the height
 * @param height The new height value of the tile
 * @pre IsValidTileIndex(tile)
 * @pre heigth <= MAX_TILE_HEIGHT
 */
template <bool Tgeneric>
static inline void SetTileHeight(typename TileIndexT<Tgeneric>::T tile, uint height)
{
	assert(IsValidTileIndex(tile));
	assert(height <= MAX_TILE_HEIGHT);
	GetTile(tile)->height = height;
}
/** @copydoc SetTileHeight(TileIndexT<Tgeneric>::T,uint) */
static inline void SetTileHeight(TileIndex tile, uint height) { SetTileHeight<false>(tile, height); }
/** @copydoc SetTileHeight(TileIndexT<Tgeneric>::T,uint) */
static inline void SetTileHeight(GenericTileIndex tile, uint height) { SetTileHeight<true>(tile, height); }

/**
 * Returns the height of a tile in pixels.
 *
 * This function returns the height of the northern corner of a tile in pixels.
 *
 * @param tile The tile to get the height
 * @return The height of the tile in pixel
 */
template <bool Tgeneric>
static inline uint TilePixelHeight(typename TileIndexT<Tgeneric>::T tile)
{
	return TileHeight(tile) * TILE_HEIGHT;
}
/** @copydoc TilePixelHeight(TileIndexT<Tgeneric>::T) */
static inline uint TilePixelHeight(TileIndex tile) { return TilePixelHeight<false>(tile); }
/** @copydoc TilePixelHeight(TileIndexT<Tgeneric>::T) */
static inline uint TilePixelHeight(GenericTileIndex tile) { return TilePixelHeight<true>(tile); }

/**
 * Returns the tile height for a coordinate outside map.  Such a height is
 * needed for painting the area outside map using completely black tiles.
 * The idea is descending to heightlevel 0 as fast as possible.
 * @param x The X-coordinate (same unit as TileX).
 * @param y The Y-coordinate (same unit as TileY).
 * @return The height in pixels in the same unit as TilePixelHeight.
 */
static inline uint TilePixelHeightOutsideMap(int x, int y)
{
	return TileHeightOutsideMap(x, y) * TILE_HEIGHT;
}

/**
 * Get the tiletype of a given tile.
 *
 * @param tile The tile to get the TileType
 * @return The tiletype of the tile
 * @pre IsValidTileIndex(tile)
 */
template <bool Tgeneric>
static inline TileType GetTileType(typename TileIndexT<Tgeneric>::T tile)
{
	assert(IsValidTileIndex(tile));
	return (TileType)GB(GetTile(tile)->type, 4, 4);
}
/** @copydoc GetTileType(TileIndexT<Tgeneric>::T) */
static inline TileType GetTileType(TileIndex tile) { return GetTileType<false>(tile); }
/** @copydoc GetTileType(TileIndexT<Tgeneric>::T) */
static inline TileType GetTileType(GenericTileIndex tile) { return GetTileType<true>(tile); }

/**
 * Check if a tile is within the map (not a border)
 *
 * @param tile The tile to check
 * @return Whether the tile is in the interior of the map
 * @pre IsValidTileIndex(tile)
 */
template <bool Tgeneric>
static inline bool IsInnerTile(typename TileIndexT<Tgeneric>::T tile)
{
	assert(IsValidTileIndex(tile));

	uint x = TileX(tile);
	uint y = TileY(tile);

	return x < MapMaxX(MapOf(tile)) && y < MapMaxY(MapOf(tile)) &&
			((x > 0 && y > 0) || !IsMainMapTile(tile) || !_settings_game.construction.freeform_edges);
}
/** @copydoc IsInnerTile(TileIndexT<Tgeneric>::T) */
static inline bool IsInnerTile(TileIndex tile) { return IsInnerTile<false>(tile); }
/** @copydoc IsInnerTile(TileIndexT<Tgeneric>::T) */
static inline bool IsInnerTile(GenericTileIndex tile) { return IsInnerTile<true>(tile); }

/**
 * Set the type of a tile
 *
 * This functions sets the type of a tile. If the type
 * MP_VOID is selected the tile must be at the south-west or
 * south-east edges of the map and vice versa.
 *
 * @param tile The tile to save the new type
 * @param type The type to save
 * @pre IsValidTileIndex(tile)
 * @pre type MP_VOID <=> tile is on the south-east or south-west edge.
 */
template <bool Tgeneric>
static inline void SetTileType(typename TileIndexT<Tgeneric>::T tile, TileType type)
{
	assert(IsValidTileIndex(tile));
	/* VOID tiles (and no others) are exactly allowed at the lower left and right
	 * edges of the map. If _settings_game.construction.freeform_edges is true,
	 * the upper edges of the map are also VOID tiles. */
	assert(IsInnerTile(tile) == (type != MP_VOID));
	SB(GetTile(tile)->type, 4, 4, type);
}
/** @copydoc SetTileType(TileIndexT<Tgeneric>::T,TileType) */
static inline void SetTileType(TileIndex tile, TileType type) { return SetTileType<false>(tile, type); }
/** @copydoc SetTileType(TileIndexT<Tgeneric>::T) */
static inline void SetTileType(GenericTileIndex tile, TileType type) { return SetTileType<true>(tile, type); }

/**
 * Checks if a tile is a give tiletype.
 *
 * This function checks if a tile got the given tiletype.
 *
 * @param tile The tile to check
 * @param type The type to check against
 * @return true If the type matches against the type of the tile
 */
template <bool Tgeneric>
static inline bool IsTileType(typename TileIndexT<Tgeneric>::T tile, TileType type)
{
	return GetTileType(tile) == type;
}
/** @copydoc IsTileType(TileIndexT<Tgeneric>::T,TileType) */
static inline bool IsTileType(TileIndex tile, TileType type) { return IsTileType<false>(tile, type); }
/** @copydoc IsTileType(TileIndexT<Tgeneric>::T,TileType) */
static inline bool IsTileType(GenericTileIndex tile, TileType type) { return IsTileType<true>(tile, type); }

/**
 * Checks if a tile is valid
 *
 * @param tile The tile to check
 * @return True if the tile is on the map and not one of MP_VOID.
 */
template <bool Tgeneric>
static inline bool IsValidTile(typename TileIndexT<Tgeneric>::T tile)
{
	return IsValidTileIndex(tile) && !IsTileType(tile, MP_VOID);
}
/** @copydoc IsValidTile(TileIndexT<Tgeneric>::T) */
static inline bool IsValidTile(TileIndex tile) { return IsValidTile<false>(tile); }
/** @copydoc IsValidTile(TileIndexT<Tgeneric>::T) */
static inline bool IsValidTile(GenericTileIndex tile) { return IsValidTile<true>(tile); }

/**
 * Returns the owner of a tile
 *
 * This function returns the owner of a tile. This cannot used
 * for tiles which type is one of MP_HOUSE, MP_VOID and MP_INDUSTRY
 * as no company owned any of these buildings.
 *
 * @param tile The tile to check
 * @return The owner of the tile
 * @pre IsValidTile(tile)
 * @pre The type of the tile must not be MP_HOUSE and MP_INDUSTRY
 */
template <bool Tgeneric>
static inline Owner GetTileOwner(typename TileIndexT<Tgeneric>::T tile)
{
	assert(IsValidTile(tile));
	assert(!IsTileType(tile, MP_HOUSE));
	assert(!IsTileType(tile, MP_INDUSTRY));

	return (Owner)GB(GetTile(tile)->m1, 0, 5);
}
/** @copydoc GetTileOwner(TileIndexT<Tgeneric>::T) */
static inline Owner GetTileOwner(TileIndex tile) { return GetTileOwner<false>(tile); }
/** @copydoc GetTileOwner(TileIndexT<Tgeneric>::T) */
static inline Owner GetTileOwner(GenericTileIndex tile) { return GetTileOwner<true>(tile); }

/**
 * Sets the owner of a tile
 *
 * This function sets the owner status of a tile. Note that you cannot
 * set a owner for tiles of type MP_HOUSE, MP_VOID and MP_INDUSTRY.
 *
 * @param tile The tile to change the owner status.
 * @param owner The new owner.
 * @pre IsValidTile(tile)
 * @pre The type of the tile must not be MP_HOUSE and MP_INDUSTRY
 */
template <bool Tgeneric>
static inline void SetTileOwner(typename TileIndexT<Tgeneric>::T tile, Owner owner)
{
	assert(IsValidTile(tile));
	assert(!IsTileType(tile, MP_HOUSE));
	assert(!IsTileType(tile, MP_INDUSTRY));

	SB(GetTile(tile)->m1, 0, 5, owner);
}
/** @copydoc SetTileOwner(TileIndexT<Tgeneric>::T,Owner) */
static inline void SetTileOwner(TileIndex tile, Owner owner) { SetTileOwner<false>(tile, owner); }
/** @copydoc SetTileOwner(TileIndexT<Tgeneric>::T,Owner) */
static inline void SetTileOwner(GenericTileIndex tile, Owner owner) { SetTileOwner<true>(tile, owner); }

/**
 * Checks if a tile belongs to the given owner
 *
 * @param tile The tile to check
 * @param owner The owner to check against
 * @return True if a tile belongs the the given owner
 */
template <bool Tgeneric>
static inline bool IsTileOwner(typename TileIndexT<Tgeneric>::T tile, Owner owner)
{
	return GetTileOwner(tile) == owner;
}
/** @copydoc IsTileOwner(TileIndexT<Tgeneric>::T,Owner) */
static inline bool IsTileOwner(TileIndex tile, Owner owner) { return IsTileOwner<false>(tile, owner); }
/** @copydoc IsTileOwner(TileIndexT<Tgeneric>::T,Owner) */
static inline bool IsTileOwner(GenericTileIndex tile, Owner owner) { return IsTileOwner<true>(tile, owner); }

/**
 * Set the tropic zone
 * @param tile the tile to set the zone of
 * @param type the new type
 * @pre tile < MapSize()
 */
static inline void SetTropicZone(TileIndex tile, TropicZone type)
{
	assert(tile < MapSize());
	assert(!IsTileType(tile, MP_VOID) || type == TROPICZONE_NORMAL);
	SB(GetTile(tile)->type, 0, 2, type);
}

/**
 * Get the tropic zone
 * @param tile the tile to get the zone of
 * @pre tile < MapSize()
 * @return the zone type
 */
static inline TropicZone GetTropicZone(TileIndex tile)
{
	assert(tile < MapSize());
	return (TropicZone)GB(GetTile(tile)->type, 0, 2);
}

/**
 * Get the current animation frame
 * @param t the tile
 * @pre IsTileType(t, MP_HOUSE) || IsTileType(t, MP_OBJECT) || IsTileType(t, MP_INDUSTRY) ||IsTileType(t, MP_STATION)
 * @return frame number
 */
static inline byte GetAnimationFrame(TileIndex t)
{
	assert(IsTileType(t, MP_HOUSE) || IsTileType(t, MP_OBJECT) || IsTileType(t, MP_INDUSTRY) ||IsTileType(t, MP_STATION));
	return GetTileEx(t)->m7;
}

/**
 * Set a new animation frame
 * @param t the tile
 * @param frame the new frame number
 * @pre IsTileType(t, MP_HOUSE) || IsTileType(t, MP_OBJECT) || IsTileType(t, MP_INDUSTRY) ||IsTileType(t, MP_STATION)
 */
static inline void SetAnimationFrame(TileIndex t, byte frame)
{
	assert(IsTileType(t, MP_HOUSE) || IsTileType(t, MP_OBJECT) || IsTileType(t, MP_INDUSTRY) ||IsTileType(t, MP_STATION));
	GetTileEx(t)->m7 = frame;
}

template <bool Tgeneric>
Slope GetTileSlope(typename TileIndexT<Tgeneric>::T tile, int *h = NULL);
/** @copydoc GetTileSlope(TileIndexT<Tgeneric>::T,int*) */
static inline Slope GetTileSlope(TileIndex tile, int *h = NULL) { return GetTileSlope<false>(tile, h); }
/** @copydoc GetTileSlope(TileIndexT<Tgeneric>::T,int*) */
static inline Slope GetTileSlope(GenericTileIndex tile, int *h = NULL) { return GetTileSlope<true>(tile, h); }

template <bool Tgeneric>
int GetTileZ(typename TileIndexT<Tgeneric>::T tile);
/** @copydoc GetTileZ(TileIndexT<Tgeneric>::T) */
static inline int GetTileZ(TileIndex tile) { return GetTileZ<false>(tile); }
/** @copydoc GetTileZ(TileIndexT<Tgeneric>::T) */
static inline int GetTileZ(GenericTileIndex tile) { return GetTileZ<true>(tile); }

template <bool Tgeneric>
int GetTileMaxZ(typename TileIndexT<Tgeneric>::T tile);
/** @copydoc GetTileMaxZ(TileIndexT<Tgeneric>::T) */
static inline int GetTileMaxZ(TileIndex tile) { return GetTileMaxZ<false>(tile); }
/** @copydoc GetTileMaxZ(TileIndexT<Tgeneric>::T) */
static inline int GetTileMaxZ(GenericTileIndex tile) { return GetTileMaxZ<true>(tile); }

bool IsTileFlat(TileIndex tile, int *h = NULL);

/**
 * Return the slope of a given tile
 * @param tile Tile to compute slope of
 * @param h    If not \c NULL, pointer to storage of z height
 * @return Slope of the tile, except for the HALFTILE part
 */
template <bool Tgeneric>
static inline Slope GetTilePixelSlope(typename TileIndexT<Tgeneric>::T tile, int *h)
{
	Slope s = GetTileSlope(tile, h);
	if (h != NULL) *h *= TILE_HEIGHT;
	return s;
}
/** @copydoc GetTilePixelSlope(TileIndexT<Tgeneric>::T,int*) */
static inline Slope GetTilePixelSlope(TileIndex tile, int *h) { return GetTilePixelSlope<false>(tile, h); }
/** @copydoc GetTilePixelSlope(TileIndexT<Tgeneric>::T,int*) */
static inline Slope GetTilePixelSlope(GenericTileIndex tile, int *h) { return GetTilePixelSlope<true>(tile, h); }

Slope GetTilePixelSlopeOutsideMap(int x, int y, int *h);

/**
 * Get bottom height of the tile
 * @param tile Tile to compute height of
 * @return Minimum height of the tile
 */
template <bool Tgeneric>
static inline int GetTilePixelZ(typename TileIndexT<Tgeneric>::T tile)
{
	return GetTileZ(tile) * TILE_HEIGHT;
}
/** @copydoc GetTilePixelZ(TileIndexT<Tgeneric>::T) */
static inline int GetTilePixelZ(TileIndex tile) { return GetTilePixelZ<false>(tile); }
/** @copydoc GetTilePixelZ(TileIndexT<Tgeneric>::T) */
static inline int GetTilePixelZ(GenericTileIndex tile) { return GetTilePixelZ<true>(tile); }

int GetTilePixelZOutsideMap(int x, int y);

/**
 * Get top height of the tile
 * @param t Tile to compute height of
 * @return Maximum height of the tile
 */
template <bool Tgeneric>
static inline int GetTileMaxPixelZ(typename TileIndexT<Tgeneric>::T tile)
{
	return GetTileMaxZ(tile) * TILE_HEIGHT;
}
/** @copydoc GetTileMaxPixelZ(TileIndexT<Tgeneric>::T) */
static inline int GetTileMaxPixelZ(TileIndex tile) { return GetTileMaxPixelZ<false>(tile); }
/** @copydoc GetTileMaxPixelZ(TileIndexT<Tgeneric>::T) */
static inline int GetTileMaxPixelZ(GenericTileIndex tile) { return GetTileMaxPixelZ<true>(tile); }

int GetTileMaxPixelZOutsideMap(int x, int y);


/**
 * Calculate a hash value from a tile position
 *
 * @param x The X coordinate
 * @param y The Y coordinate
 * @return The hash of the tile
 */
static inline uint TileHash(uint x, uint y)
{
	uint hash = x >> 4;
	hash ^= x >> 6;
	hash ^= y >> 4;
	hash -= y >> 6;
	return hash;
}

/**
 * Get the last two bits of the TileHash
 *  from a tile position.
 *
 * @see TileHash()
 * @param x The X coordinate
 * @param y The Y coordinate
 * @return The last two bits from hash of the tile
 */
static inline uint TileHash2Bit(uint x, uint y)
{
	return GB(TileHash(x, y), 0, 2);
}

#endif /* TILE_MAP_H */
