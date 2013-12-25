/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VHAVOK_AI_NAVMESH_RESOURCE_HPP
#define __VHAVOK_AI_NAVMESH_RESOURCE_HPP

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiIncludes.hpp>
#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/Collide/NavMesh/hkaiNavMeshQueryMediator.h>

class vHavokAiNavMeshInstance;

class vHavokAiNavMeshResource : public VManagedResource
{
	///
	/// @name Constructor / Destructor
	/// @{
	///

protected:
	/// \brief
	///  called by vHavokAiNavMeshResourceManager::CreateResource
	VHAVOKAI_IMPEXP vHavokAiNavMeshResource(const char* filePath); 

public:
	VHAVOKAI_IMPEXP vHavokAiNavMeshResource(hkaiNavMesh* navMesh, hkaiNavMeshQueryMediator* mediator);

	VHAVOKAI_IMPEXP virtual ~vHavokAiNavMeshResource();

	///
	/// @}
	///

	///
	/// @name Overridden functions
	/// @{
	///

	VHAVOKAI_IMPEXP virtual BOOL Reload() HKV_OVERRIDE;

	VHAVOKAI_IMPEXP virtual BOOL Unload() HKV_OVERRIDE;

	VHAVOKAI_IMPEXP virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU) HKV_OVERRIDE;

#ifdef SUPPORTS_SNAPSHOT_CREATION
  VHAVOKAI_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

	///
	/// @}
	///

	///
	/// @name NavMesh File loading/unloading functions
	/// @{
	///

	/// \brief
	///   Save current contents in m_navMeshes and m_mediators array to specified hkt file and calls SetFileName.
  ///
	/// If filename is NULL, then filename will be autogenerated equal to navmesh_{navmesh section uid}.hkt
	VHAVOKAI_IMPEXP bool SetFilenameAndSaveNavMesh(const char* relativeFilePath, const char* rootDirectory);

	/// \brief
	///   Static helper function that saves nav mesh to specified stream
	VHAVOKAI_IMPEXP static bool SaveNavMesh(IVFileOutStream *pIn, const hkRefPtr<hkaiNavMesh>& navMesh, const hkRefPtr<hkaiNavMeshQueryMediator>& mediator);

	/// \brief
	///   Load nav meshes from stream.
	VHAVOKAI_IMPEXP static bool LoadNavMesh(IVFileInStream *pIn, hkRefPtr<hkaiNavMesh>& navMeshOut, hkRefPtr<hkaiNavMeshQueryMediator>& mediatorOut);

	/// \brief
	///   Load nav meshes from file. Just calls previous static function.
	VHAVOKAI_IMPEXP static bool LoadNavMesh(const char* filePath, hkRefPtr<hkaiNavMesh>& navMeshOut, hkRefPtr<hkaiNavMeshQueryMediator>& mediatorOut);

	///
	/// @}
	///

	///
	/// @name Misc
	/// @{
	///

	VHAVOKAI_IMPEXP hkaiNavMesh* GetNavMesh()														              { return m_navMesh; }
	VHAVOKAI_IMPEXP const hkaiNavMesh* GetNavMesh() const											        { return m_navMesh; }
	VHAVOKAI_IMPEXP hkaiNavMeshQueryMediator* GetNavMeshQueryMediator()								{ return m_mediator; }
	VHAVOKAI_IMPEXP const hkaiNavMeshQueryMediator* GetNavMeshQueryMediator() const		{ return m_mediator; }

	VHAVOKAI_IMPEXP int GetNumNavMeshVertices() const					{ return GetNavMesh() ? GetNavMesh()->getNumVertices() : -1; }
	VHAVOKAI_IMPEXP int GetNumNavMeshEdges() const						{ return GetNavMesh() ? GetNavMesh()->getNumEdges() : -1; }
	VHAVOKAI_IMPEXP int GetNumNavMeshFaces() const						{ return GetNavMesh() ? GetNavMesh()->getNumFaces() : -1; }
	VHAVOKAI_IMPEXP int GetNavMeshVertexSize() const					{ return sizeof(hkVector4); }
	VHAVOKAI_IMPEXP int GetNavMeshEdgeSize() const						{ return sizeof(hkaiNavMesh::Edge); }
	VHAVOKAI_IMPEXP int GetNavMeshFaceSize() const						{ return sizeof(hkaiNavMesh::Face); }
	VHAVOKAI_IMPEXP int GetNavMeshStructSize() const					{ return sizeof(hkaiNavMesh); }

	///
	/// @}
	///

protected:
	friend class vHavokAiNavMeshResourceManager;     

  /// \brief
	///   Common initialization function.
	void Init();

	hkRefPtr<hkaiNavMesh>					m_navMesh;
	hkRefPtr<hkaiNavMeshQueryMediator>		m_mediator;
};

#endif	// __VHAVOK_AI_NAVMESH_RESOURCE_HPP

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
