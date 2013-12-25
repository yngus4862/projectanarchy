/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//HK_REFLECTION_PARSER_EXCLUDE_FILE

// Autogenerated by generateReflections.py (projectClassList.py)
// Changes will not be lost unless:
// - The workspace is re-generated using build.py
// - The corresponding reflection database (reflection.db) is deleted
// - The --force-output or --force-rebuild option is added to the pre-build generateReflection.py execution

HK_ABSTRACT_CLASS(hkMoppBvTreeShapeBase)
HK_ABSTRACT_CLASS(hkpBvTreeShape)
HK_ABSTRACT_CLASS(hkpCollidableCollidableFilter)
HK_ABSTRACT_CLASS(hkpCollisionFilter)
HK_ABSTRACT_CLASS(hkpConvexListFilter)
HK_ABSTRACT_CLASS(hkpConvexShape)
HK_ABSTRACT_CLASS(hkpConvexTransformShapeBase)
HK_ABSTRACT_CLASS(hkpPhantomCallbackShape)
HK_ABSTRACT_CLASS(hkpRayCollidableFilter)
HK_ABSTRACT_CLASS(hkpRayShapeCollectionFilter)
HK_ABSTRACT_CLASS(hkpShapeCollection)
HK_ABSTRACT_CLASS(hkpShapeCollectionFilter)
HK_ABSTRACT_CLASS(hkpShapeContainer)
HK_CLASS(hkpBoxShape)
HK_CLASS(hkpBvShape)
HK_CLASS(hkpCapsuleShape)
HK_CLASS(hkpCollisionFilterList)
HK_CLASS(hkpConvexListShape)
HK_CLASS(hkpConvexTransformShape)
HK_CLASS(hkpConvexTranslateShape)
HK_CLASS(hkpConvexVerticesConnectivity)
HK_CLASS(hkpConvexVerticesShape)
HK_CLASS(hkpCylinderShape)
HK_CLASS(hkpDefaultConvexListFilter)
HK_CLASS(hkpGroupFilter)
HK_CLASS(hkpListShape)
HK_CLASS(hkpMoppBvTreeShape)
HK_CLASS(hkpMultiRayShape)
HK_CLASS(hkpMultiSphereShape)
HK_CLASS(hkpNullCollisionFilter)
HK_CLASS(hkpRemoveTerminalsMoppModifier)
HK_CLASS(hkpShape)
HK_CLASS(hkpShapeBase)
HK_CLASS(hkpShapeInfo)
HK_CLASS(hkpSingleShapeContainer)
HK_CLASS(hkpSphereRepShape)
HK_CLASS(hkpSphereShape)
HK_CLASS(hkpTransformShape)
HK_CLASS(hkpTriangleShape)
HK_STRUCT(hkpAgent1nSector)
HK_STRUCT(hkpBroadPhaseHandle)
HK_STRUCT(hkpCdBody)
HK_STRUCT(hkpCollidable)
HK_STRUCT(hkpCollidableBoundingVolumeData)
HK_STRUCT(hkpLinkedCollidable)
HK_STRUCT(hkpListShapeChildInfo)
HK_STRUCT(hkpMeshMaterial)
HK_STRUCT(hkpMultiRayShapeRay)
HK_STRUCT(hkpNamedMeshMaterial)
HK_STRUCT(hkpTypedBroadPhaseHandle)
HK_STRUCT(hkpWeldingUtility)
#ifndef HK_EXCLUDE_FEATURE_hkpCompressedMeshShape
HK_CLASS(hkpCompressedMeshShape)
HK_STRUCT(hkpCompressedMeshShapeBigTriangle)
HK_STRUCT(hkpCompressedMeshShapeChunk)
HK_STRUCT(hkpCompressedMeshShapeConvexPiece)
#endif
#ifndef HK_EXCLUDE_FEATURE_hkpConvexPieceMeshShape
HK_CLASS(hkpConvexPieceMeshShape)
#endif
#ifndef HK_EXCLUDE_FEATURE_hkpExtendedMeshShape
HK_CLASS(hkpExtendedMeshShape)
HK_CLASS(hkpStorageExtendedMeshShape)
HK_CLASS(hkpStorageExtendedMeshShapeMeshSubpartStorage)
HK_CLASS(hkpStorageExtendedMeshShapeShapeSubpartStorage)
HK_STRUCT(hkpExtendedMeshShapeShapesSubpart)
HK_STRUCT(hkpExtendedMeshShapeSubpart)
HK_STRUCT(hkpExtendedMeshShapeTrianglesSubpart)
HK_STRUCT(hkpStorageExtendedMeshShapeMaterial)
#endif
#ifndef HK_EXCLUDE_FEATURE_hkpHeightField
HK_ABSTRACT_CLASS(hkpHeightFieldShape)
HK_CLASS(hkpCompressedSampledHeightFieldShape)
HK_CLASS(hkpPlaneShape)
HK_CLASS(hkpSampledHeightFieldShape)
HK_CLASS(hkpStorageSampledHeightFieldShape)
HK_CLASS(hkpTriSampledHeightFieldBvTreeShape)
HK_CLASS(hkpTriSampledHeightFieldCollection)
HK_STRUCT(hkpSampledHeightFieldShapeCoarseMinMaxLevel)
#endif
#ifndef HK_EXCLUDE_FEATURE_hkpMeshShape
HK_CLASS(hkpFastMeshShape)
HK_CLASS(hkpMeshShape)
HK_CLASS(hkpStorageMeshShape)
HK_CLASS(hkpStorageMeshShapeSubpartStorage)
HK_STRUCT(hkpMeshShapeSubpart)
#endif
#ifndef HK_EXCLUDE_FEATURE_hkpSimpleMeshShape
HK_CLASS(hkpSimpleMeshShape)
HK_STRUCT(hkpSimpleMeshShapeTriangle)
#endif

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
