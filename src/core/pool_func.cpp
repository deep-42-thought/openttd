/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file pool_func.cpp Implementation of PoolBase methods. */

#include "../stdafx.h"
#include "pool_type.hpp"

PoolBase::~PoolBase()
{
	PoolVector *pools = PoolBase::GetPools();
	pools->Erase(pools->Find(this));
	if (pools->Length() == 0) delete pools;
}

/* static */ void PoolBase::CleanAll()
{
	PoolVector *pools = PoolBase::GetPools();
	PoolBase **end = pools->End();
	for (PoolBase **ppool = pools->Begin(); ppool != end; ppool++) {
		PoolBase *pool = *ppool;
		pool->CleanPool();
	}
}