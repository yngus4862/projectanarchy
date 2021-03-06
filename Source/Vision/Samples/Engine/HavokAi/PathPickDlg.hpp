/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef PATHPICKDLG_H_INCLUDED
#define PATHPICKDLG_H_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>

class PathPickDlg : public VDialog
{
public:
  PathPickDlg();

	virtual void OnSetFocus(bool bStatus) HKV_OVERRIDE;

	virtual VCursor* GetCurrentCursor(VGUIUserInfo_t &user) HKV_OVERRIDE;

	virtual void OnClick(VMenuEventDataObject *pEvent) HKV_OVERRIDE;

  inline const hkvVec3 GetStartPoint() const { return m_vStartPoint; }

  inline const hkvVec3 GetEndPoint() const { return m_vEndPoint; }

private:
	VCursorPtr m_spCursor;
  hkvVec3 m_vStartPoint;
  hkvVec3 m_vEndPoint;
};

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
