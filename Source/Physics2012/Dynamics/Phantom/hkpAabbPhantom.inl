/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkArray<hkpCollidable*>& hkpAabbPhantom::getOverlappingCollidables()
{
	HK_WARN_ONCE_ON_DEBUG_IF( m_orderDirty, 0x5fe59c18, "Phantom queried for collidables without call to ensureDeterministicOrder(). The result is potentially nondeterministic.");
	return m_overlappingCollidables;
}


inline const hkAabb& hkpAabbPhantom::getAabb( ) const
{
	return m_aabb;
}


inline hkpAabbPhantom::hkpAabbPhantom( class hkFinishLoadedObjectFlag flag ) 
	: hkpPhantom ( flag )
{
}

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
