/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file tunnel_map.cpp Map accessors for tunnels. */

#include "stdafx.h"
#include "tunnelbridge_map.h"

#include "safeguards.h"


/**
 * Gets the other end of the tunnel. Where a vehicle would reappear when it
 * enters at the given tile.
 * @param tile the tile to search from.
 * @return the tile of the other end of the tunnel.
 */
template <bool Tgeneric>
typename TileIndexT<Tgeneric>::T GetOtherTunnelEnd(typename TileIndexT<Tgeneric>::T tile)
{
	assert(IsTunnelTile(tile));

	DiagDirection dir = GetTunnelBridgeDirection(tile);
	TileIndexDiff delta = TileOffsByDiagDir<Tgeneric>(dir, MapOf(tile));
	uint h = TileHeight(tile);

	if (dir == DIAGDIR_NE || dir == DIAGDIR_NW) {
		h--;
continue_ne_nw:
	do {
		tile += delta;
		} while (TileHeight(tile) != h);
	} else {
continue_se_sw:
		tile += delta;
		do {
			tile += delta;
		} while (TileHeight(tile) != h);
		tile -= delta;
	}

	if (IsTunnelTile(tile) && GetTunnelBridgeDirection(tile) == ReverseDiagDir(dir)) {
	} else {
	/* Handle Chunnels.
	 * Only look for tunnel when hight level changes.
	 * And only at sea level.
	 */
		assert(h <= 1);
		(h == 0) ? h = 1 : h = 0;
		if (dir == DIAGDIR_NE || dir == DIAGDIR_NW) {
			goto continue_ne_nw;
		} else {
			goto continue_se_sw;
		}
	}

	return tile;
}
/* instantiate */
template TileIndex GetOtherTunnelEnd<false>(TileIndex tile);
template GenericTileIndex GetOtherTunnelEnd<true>(GenericTileIndex tile);

/**
 * Is there a Chunnel in the way in the given direction?
 * Only between height level 0 and 1.
 * Used to avoid building bridge or tunnel between existing chunnel.
 * @param tile the tile to search from.
 * @param dir  the direction to start searching to.
 * @return true if and only if there is a chunnel.
 */
bool IsBetweenChunnelPortals(TileIndex tile, DiagDirection dir)
{
	uint h = 0;
	TileIndexDiff delta = TileOffsByDiagDir(dir);
	if (GetTileZ(tile) > 0) return false;

	do {
		if (dir == DIAGDIR_NE || dir == DIAGDIR_NW) {
			do {
				tile += delta;
			if (!IsValidTile(tile)) return false;
			} while (TileHeight(tile) != h);
		} else {
			tile += delta;
			do {
				tile += delta;
			if (!IsValidTile(tile)) return false;
			} while (TileHeight(tile) != h);
			tile -= delta;
		}
		(h == 0) ? h = 1 : h = 0;
	} while (!IsTunnelTile(tile));

	if (GetTunnelBridgeDirection(tile) != ReverseDiagDir(dir)) return false;

	return true;
}

/**
 * Is there a tunnel in the way in the given direction?
 * Between level 0 and 1 terraforming is allowed. (No search)
 * @param tile the tile to search from.
 * @param z    the 'z' to search on.
 * @param dir  the direction to start searching to.
 * @return true if and only if there is a tunnel.
 */
bool IsTunnelInWayDir(TileIndex tile, int z, DiagDirection dir)
{
	/* Between level 0 and 1 terraforming is allowed. */
	if (GetTileZ(tile) <= 1) return false;

	TileIndexDiff delta = TileOffsByDiagDir(dir);
	int height;

	do {
		tile -= delta;
		if (!IsValidTile(tile)) return false;
		height = GetTileZ(tile);
	} while (z < height);

	return z == height && IsTunnelTile(tile) && GetTunnelBridgeDirection(tile) == dir;
}

/**
 * Is there a tunnel in the way in any direction?
 * @param tile the tile to search from.
 * @param z the 'z' to search on.
 * @return true if and only if there is a tunnel.
 */
bool IsTunnelInWay(TileIndex tile, int z)
{
	return IsTunnelInWayDir(tile, z, (TileX(tile) > (MapMaxX() / 2)) ? DIAGDIR_NE : DIAGDIR_SW) ||
			IsTunnelInWayDir(tile, z, (TileY(tile) > (MapMaxY() / 2)) ? DIAGDIR_NW : DIAGDIR_SE);
}
