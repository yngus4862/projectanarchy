/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// NodeManaged.cpp : Defines the entry point for the DLL application.

// Note: the dependency on _vcclrit.h has been removed (during the migration to VS2005).
// See the following page for details: http://msdn2.microsoft.com/en-us/library/ms173267(VS.80).aspx
// The _vcclrit.h header file was used in VS2003 for fixing static member initialisation/deinitialisation (MS-KB #814472).

#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainManagedPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationModel.hpp>
#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainDecorationBillboardMesh.hpp>

VIMPORT IVisPlugin_cl* GetEnginePlugin_VisionEnginePlugin();

namespace TerrainManaged
{
  public ref class ManagedModule
  {
  public:
  
    static void InitManagedModule()
    {
      // make sure the native plugin is initialized:
      GetEnginePlugin_VisionEnginePlugin()->InitEnginePlugin();

      // billboard model factory class (fake model to generate static billboard meshes per sector)
      VTerrainDecorationModelManager::GlobalManager().RegisterFactory(&VTerrainDecorationBillboardMesh::g_BillboardMeshFactory);
    }

    static void DeInitManagedModule()
    {
      // billboard model factory class (fake model to generate static billboard meshes per sector)
      VTerrainDecorationModelManager::GlobalManager().UnregisterFactory(&VTerrainDecorationBillboardMesh::g_BillboardMeshFactory);

      GetEnginePlugin_VisionEnginePlugin()->DeInitEnginePlugin();
    }
  };

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
