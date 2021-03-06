/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/ScaleformEnginePlugin.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/vScaleformManager.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformMovie.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformUtil.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/vScaleformInternal.hpp>

//we do not use namespace Scaleform since it contains ambiguous symbols like Ptr, Value, etc...
using namespace Scaleform::Render;
using namespace Scaleform::GFx;

//global switches
static const bool s_bAntiAlias = true;

#ifdef _VISION_MOBILE

class VScaleformMultitouch : public Scaleform::GFx::MultitouchInterface
{
protected:
  int m_iMaxTouchPoints;
public:
  VScaleformMultitouch(int iMaxTouchPoints) : m_iMaxTouchPoints(iMaxTouchPoints) {}

  virtual unsigned GetMaxTouchPoints() const HKV_OVERRIDE { return m_iMaxTouchPoints; }
  
  virtual Scaleform::UInt32   GetSupportedGesturesMask() const HKV_OVERRIDE { return 0; }
  
  virtual bool     SetMultitouchInputMode(Scaleform::GFx::MultitouchInterface::MultitouchInputMode eMode) HKV_OVERRIDE { return true; }
};

/*
 class VScaleformVirtualKeyboard : public Scaleform::GFx::VirtualKeyboardInterface
 {
 public:
 // Invoked when an input textfield recieves focus
 virtual void    OnInputTextfieldFocusIn(bool multiline, const Scaleform::GFx::Render::RectF& textBox)
 {
 //open virtual KB
 }
 
 // Invoked when an input textfield loses focus
 virtual void    OnInputTextfieldFocusOut()
 {
 //close virtual KB
 }
 };
 */

#endif

////////////////////////////////////////////
//          VScaleformAdvanceTask         //
//////////////////////////////////////////// 

VScaleformAdvanceTask::VScaleformAdvanceTask(VScaleformMovieInstance* pMovieInst)
  : VThreadedTask()
  , m_bInitInProgress(false)
  , m_fTimeDelta(0.0f)
  , m_pMovieInst(pMovieInst)
{
}

void VScaleformAdvanceTask::Init()
{
  // Set init to true in order to perform initialization during Run().
  m_bInitInProgress = true;
  Schedule(0.0f);
  WaitUntilFinished();
  m_bInitInProgress = false;
}

void VScaleformAdvanceTask::Schedule(float fTimeDelta)
{
  m_fTimeDelta = fTimeDelta;

  if (VScaleformManager::GlobalManager().IsMultithreadedAdvanceEnabled())
  {
    Vision::GetThreadManager()->ScheduleTask(this);
  }
  else
  {
    Run(NULL);
  }
}

void VScaleformAdvanceTask::ScheduleMinStep()
{
  VASSERT(m_pMovieInst != NULL && m_pMovieInst->GetGFxMovieInstance() != NULL);
  const float fFrameRate =  m_pMovieInst->GetGFxMovieInstance()->GetMovieDef()->GetFrameRate();

  if (fFrameRate > 0.0f)
    Schedule(1.0f / fFrameRate);
}

void VScaleformAdvanceTask::WaitUntilFinished()
{
  if (!VScaleformManager::GlobalManager().IsMultithreadedAdvanceEnabled())
    return;

  Vision::GetThreadManager()->WaitForTask(this, true);

  // Movie instance can now be accessed from the current thread.
  m_pMovieInst->GetGFxMovieInstance()->SetCaptureThread(Scaleform::GetCurrentThreadId());
}

void VScaleformAdvanceTask::Run(VManagedThread *pThread)
{
  VASSERT_MSG(m_pMovieInst != NULL && m_pMovieInst->GetGFxMovieInstance() != NULL, "No scaleform movie instance present!");
  m_pMovieInst->GetGFxMovieInstance()->SetCaptureThread(Scaleform::GetCurrentThreadId());

  // Initialization
  if (m_bInitInProgress)
  {
    m_pMovieInst->GetGFxMovieInstance()->HandleEvent(Event::SetFocus);
    m_pMovieInst->GetGFxMovieInstance()->Advance(0.0f, 0); // load first frame
  }
  // Advance Movie
  else
  {
    m_pMovieInst->GetGFxMovieInstance()->Advance(m_fTimeDelta);
  }
}

//////////////////////////////////////////////
//          VScaleformMovieInstance         //
//////////////////////////////////////////////

VScaleformMovieInstance::VScaleformMovieInstance(const char *szFilename, Scaleform::GFx::Loader* pLoader,
  const char *szCandidateMovie, const char *szImeXml, int iPosX, int iPosY, int iWidth, int iHeight)
  : m_pMovieInst(NULL)
  , m_phMovieDisplay(NULL)
  , m_referencedScaleformValues()
#ifdef USE_SF_IME
  , m_sCandidateMovie(szCandidateMovie)
  , m_sImeXml(szImeXml)
#endif
  , m_bInit(false)
  , m_bHandleInput(true)
  , m_bIsTexture(false)
  , m_iVisibleMask(VIS_ENTITY_VISIBLE_IN_WORLD)
  , m_pLastRenderContext(NULL)
  , m_iPosX(iPosX)
  , m_iPosY(iPosY)
  , m_iWidth(iWidth)
  , m_iHeight(iHeight)
  , m_iMovieAuthoredWidth(SF_MOVIE_SIZE_AUTHORED)
  , m_iMovieAuthoredHeight(SF_MOVIE_SIZE_AUTHORED)
  , m_sFileName(szFilename)
#if defined(WIN32)
  , m_pKeyModifiers(NULL)
#endif
  , m_pAdvanceTask(NULL)
{
  m_pAdvanceTask = new VScaleformAdvanceTask(this);

#if defined(WIN32)
  m_pKeyModifiers = new Scaleform::KeyModifiers();
#endif
  Scaleform::Ptr<MovieDef> pMovieDef = *pLoader->CreateMovie(szFilename, Loader::LoadAll|Loader::LoadWaitCompletion);
  VASSERT_MSG(pMovieDef != NULL, "Could not create Scaleform Movie Def!");

  //Scaleform::StringBuffer buffer;
  //Scaleform::Memory::pGlobalHeap->MemReport(buffer, Scaleform::MemoryHeap::MemReportFull);
  //hkvLog::Warning("\n-----------------\nCreateMovie report:\n\n%s\n-----------------\n", buffer.ToCStr());

  // get the movie size which was used when authoring the movie
  m_iMovieAuthoredWidth = static_cast<int>(pMovieDef->GetWidth());
  m_iMovieAuthoredHeight = static_cast<int>(pMovieDef->GetHeight());

  m_pMovieInst = pMovieDef->CreateInstance(false /*initFirstFrame*/);
  VASSERT_MSG(m_pMovieInst != NULL, "Could not create Scaleform Movie Instance!");

  //Scaleform::StringBuffer buffer2;
  //Scaleform::Memory::pGlobalHeap->MemReport(buffer2, Scaleform::MemoryHeap::MemReportFull);
  //hkvLog::Warning("\n-----------------\nCreateInstance report:\n\n%s\n-----------------\n", buffer2.ToCStr());

  m_phMovieDisplay = new MovieDisplayHandle(m_pMovieInst->GetDisplayHandle());

  m_pMovieInst->SetEdgeAAMode(s_bAntiAlias ? EdgeAA_On : EdgeAA_Disable);

  m_pMovieInst->SetUserData(this);

  // enable one mouse
  m_pMovieInst->SetMouseCursorCount(1);

  // Background Alpha = 0 for transparent background
  m_pMovieInst->SetBackgroundAlpha(0.0f);

  // Update viewport:
  // The viewport of the Scaleform movie has to be set prior to the first advance since otherwise the movie will be not displayed 
  // correctly in the first rendered frame. Since at this point the actually used render-context that will be used to display the 
  // movie can not be always obtained, the current render-context will be used to update the viewport. Though in most cases this 
  // will work correctly, for cases in which the resolution of the current and actually used render-context differ, the first frame
  // will be displayed with the wrong viewport. Please note that m_pLastRenderContext will then be reset within the first invocation 
  // of VScaleformManager::RenderMovies().
  m_pLastRenderContext = Vision::Contexts.GetCurrentContext();
  UpdateViewport();

#if defined(_VISION_MOBILE)
  //setup touch interface
  m_pMovieInst->SetMultitouchInterface(Scaleform::Ptr<MultitouchInterface>(*new VScaleformMultitouch(VScaleformManager::GlobalManager().GetMaxTouchPoints())));
  //setup virtual keyboard
  //m_pMovieInst->SetVirtualKeyboardInterface(Scaleform::Ptr<VirtualKeyboardInterface>(*new VScaleformVirtualKeyboard()));
#endif

#if defined(USE_SF_IME)
  InitImeManager();

  // Activate IME Manager if present.
  if (pLoader->GetIMEManager() != NULL)
  {
    pLoader->GetIMEManager()->SetActiveMovie(m_pMovieInst);
  }
#endif

  // Let the advance task do the initialization.
  m_pAdvanceTask->Init();
}

VScaleformMovieInstance::~VScaleformMovieInstance()
{
  Invalidate();
}

void VScaleformMovieInstance::Invalidate()
{
  if (!IsValid())
    return;

  m_pAdvanceTask->WaitUntilFinished();
  V_SAFE_DELETE(m_pAdvanceTask);

  #if defined(WIN32)
    V_SAFE_DELETE(m_pKeyModifiers);
  #endif

  for (int i = 0; i < m_referencedScaleformValues.GetSize(); i++)
  {
    delete m_referencedScaleformValues[i];
  }
  m_referencedScaleformValues.RemoveAll();

  for (int i = 0; i < m_queuedFSCommands.GetSize(); i++)
  {
    delete m_queuedFSCommands[i];
  }
  m_queuedFSCommands.RemoveAll();

  for (int i = 0; i < m_queuedExternalCalls.GetSize(); i++)
  {
    delete m_queuedExternalCalls[i];
  }
  m_queuedExternalCalls.RemoveAll();

  V_SAFE_RELEASE(m_pMovieInst);
  V_SAFE_DELETE(m_phMovieDisplay); //no ref count for movie display handle
}

VScaleformValue* VScaleformMovieInstance::GetVariable(const char * szVarName)
{
  VASSERT_MSG(szVarName != NULL, "Specify a valid var!");
  VASSERT_MSG(m_pMovieInst != NULL, "Movie not loaded");

  // Check if variable object has already been created.
  for (int i = 0; i < m_referencedScaleformValues.GetSize(); i++)
  {
    VScaleformValue* pValue = m_referencedScaleformValues[i];
    if (pValue->m_sVarName == szVarName)
      return pValue;
  }

  // Wait until the advance task has finished before retrieving the variable.
  m_pAdvanceTask->WaitUntilFinished();

  // Create new Scaleform variable.
  Scaleform::GFx::Value* pValue = new Scaleform::GFx::Value();
  if (!m_pMovieInst->GetVariable(pValue, szVarName))
  {
    delete pValue;
    return NULL;
  }

  // Wrap SF object.
  VScaleformValue *pWrappedValue = new VScaleformValue(pValue, m_pMovieInst, szVarName);

  // Get current scaleform value from the movie.
  pWrappedValue->SyncValueWithScaleform();

  // Add to internal list in order to update scaleform values every frame.
  m_referencedScaleformValues.Add(pWrappedValue);

  return pWrappedValue;
}

void VScaleformMovieInstance::SetOpacity(float fAlpha)
{
  VASSERT_MSG(m_pMovieInst != NULL, "Movie not loaded");
  m_pAdvanceTask->WaitUntilFinished();

  m_pMovieInst->SetBackgroundAlpha(fAlpha);
}

bool VScaleformMovieInstance::IsValid() const
{
  return (m_pMovieInst != NULL);
}

void VScaleformMovieInstance::SetVisibleBitmask(unsigned int iMask) 
{ 
  m_iVisibleMask = iMask; 
}

unsigned int VScaleformMovieInstance::GetVisibleBitmask() const 
{ 
  return m_iVisibleMask; 
}

bool VScaleformMovieInstance::IsVisibleInAnyContext() const
{
  return (m_iVisibleMask != 0);
}

void VScaleformMovieInstance::SetPaused(bool bPaused)
{
  m_pAdvanceTask->WaitUntilFinished();

  m_pMovieInst->SetPause(bPaused);
}

bool VScaleformMovieInstance::IsPaused() const
{
  m_pAdvanceTask->WaitUntilFinished();

  // 0 playback
  // 1 stopped
  return (m_pMovieInst->GetPlayState() != 0);
}

void VScaleformMovieInstance::Restart()
{
  m_pAdvanceTask->WaitUntilFinished();

  m_pMovieInst->Restart();
}

bool VScaleformMovieInstance::IsFocused() const
{
  if(m_pMovieInst == NULL) 
    return false;

  m_pAdvanceTask->WaitUntilFinished();
  return m_pMovieInst->IsMovieFocused();
}

void VScaleformMovieInstance::SetHandleInput(bool bEnabled)
{
  m_bHandleInput = bEnabled;
}

const char* VScaleformMovieInstance::GetFileName() const
{
  return m_sFileName.AsChar();
}

Scaleform::GFx::Movie* VScaleformMovieInstance::GetGFxMovieInstance() const
{
  return m_pMovieInst;
}

void VScaleformMovieInstance::WaitForAdvanceFinished() const
{
  m_pAdvanceTask->WaitUntilFinished();
}

void VScaleformMovieInstance::SetDimensions(int iPosX /*= 0*/, int iPosY /*= 0*/, int iWidth /*= -1*/, int iHeight /*= -1*/)
{
  m_iPosX = iPosX;
  m_iPosY = iPosY;
  m_iWidth = iWidth;
  m_iHeight = iHeight;

  UpdateViewport();
}

void VScaleformMovieInstance::GetDimensions(int& iPosX, int& iPosY, int& iWidth, int& iHeight)
{
  iPosX = m_iPosX;
  iPosY = m_iPosY;

  iWidth = m_iWidth;
  iHeight = m_iHeight;
}

void VScaleformMovieInstance::SetPosition(int iPosX, int iPosY)
{
  SetDimensions(iPosX, iPosY, m_iWidth, m_iHeight);
}

// gets the position
void VScaleformMovieInstance::GetPosition(int& iPosX, int& iPosY) const
{
  iPosX = m_iPosX;
  iPosY = m_iPosY;
}

// sets the size of the movie
void VScaleformMovieInstance::SetSize(int iWidth, int iHeight)
{
  SetDimensions(m_iPosX, m_iPosY, iWidth, iHeight);
}

// gets the currently set size of the movie
void VScaleformMovieInstance::GetSize(int& iWidth, int& iHeight) const
{
  iWidth = m_iWidth;
  iHeight = m_iHeight;
}

// gets the size which was used when authoring the movie
void VScaleformMovieInstance::GetAuthoredSize(int& iWidth, int& iHeight) const
{
  iWidth = m_iMovieAuthoredWidth;
  iHeight = m_iMovieAuthoredHeight;
}

void VScaleformMovieInstance::SetAuthoredSize()
{
  SetDimensions(m_iPosX, m_iPosY, SF_MOVIE_SIZE_AUTHORED, SF_MOVIE_SIZE_AUTHORED);
}

void VScaleformMovieInstance::UpdateViewport()
{
  if (m_pMovieInst == NULL || m_pLastRenderContext == NULL)
    return;

  // if the height or width is undefined use the render context size it is attached to
  int iRenderContextWidth, iRenderContextHeight;
  m_pLastRenderContext->GetSize(iRenderContextWidth, iRenderContextHeight);

  if(m_iWidth == SF_MOVIE_SIZE_RENDERCONTEXT)
    m_iWidth = iRenderContextWidth;
  else if(m_iWidth == SF_MOVIE_SIZE_AUTHORED)
    m_iWidth = m_iMovieAuthoredWidth;

  if(m_iHeight == SF_MOVIE_SIZE_RENDERCONTEXT)
    m_iHeight = iRenderContextHeight;
  else if(m_iHeight == SF_MOVIE_SIZE_AUTHORED)
    m_iHeight = m_iMovieAuthoredHeight;

  m_pAdvanceTask->WaitUntilFinished();

  unsigned int iFlags = 0;
  if (m_bIsTexture)
  {
    iFlags |= Scaleform::Render::Viewport::View_IsRenderTexture;
  }

  m_pMovieInst->SetViewport(iRenderContextWidth, iRenderContextHeight, m_iPosX, m_iPosY, m_iWidth, m_iHeight, iFlags);
}

bool VScaleformMovieInstance::ValidateFocus(float fX, float fY)
{
  VASSERT_MSG(m_pMovieInst!=NULL, "GFx Movie not present");
  m_pAdvanceTask->WaitUntilFinished();

  // focus will be restored when clicking anywhere inside the scaleform movie and the movie is visible
  if( IsVisibleInAnyContext() && (fX>=m_iPosX && fX<=(m_iPosX+m_iWidth)) && (fY>=m_iPosY && fY<=(m_iPosY+m_iHeight)) )
  {
    if(m_pMovieInst->IsMovieFocused()) return false;

    m_pMovieInst->HandleEvent(Event::SetFocus);
    return true;
  }

  //outside click
  m_pMovieInst->HandleEvent(Event::KillFocus);
  return false;
}

void VScaleformMovieInstance::SyncScaleformVariables()
{
  for (int i = 0; i < m_referencedScaleformValues.GetSize(); i++)
  {
    VScaleformValue* pValue = m_referencedScaleformValues.GetAt(i);

    pValue->SyncValueWithScaleform();
  }
}

void VScaleformMovieInstance::HandleScaleformCallbacks()
{
  // Process FS callbacks
  for (int i = 0; i < m_queuedFSCommands.GetSize(); i++)
  {
    VOnFSCommand* pData = m_queuedFSCommands[i];

    // call virtual function
    OnCommand(pData->m_sCommand, pData->m_sArgs); 

    // ..and also trigger the callback
    VOnFSCommand::OnFSCallback.TriggerCallbacks(pData);

    // inform scripting system about the callback...
    
    // call 'OnFsCommand' for the scene script (if callback is present)
    IVScriptInstance *pSceneInst = VScriptResourceManager::GlobalManager().GetSceneScript();
    if (pSceneInst != NULL)
      pSceneInst->ExecuteFunctionArg("OnFsCommand", "sss", GetFileName(), pData->m_sCommand.AsChar(), pData->m_sArgs.AsChar());

    // call 'OnFsCommand' for the game script (if callback is present)
    IVScriptInstance *pGameInst = VScriptResourceManager::GlobalManager().GetGameScript();
    if (pGameInst != NULL)
      pGameInst->ExecuteFunctionArg("OnFsCommand", "sss", GetFileName(), pData->m_sCommand.AsChar(), pData->m_sArgs.AsChar());

    delete pData;
  }

  m_queuedFSCommands.SetSize(0, -1, false /* Do not deallocate */);

  // Process External Interface Calls
  for (int i = 0; i < m_queuedExternalCalls.GetSize(); i++)
  {
    VOnExternalInterfaceCall* pData = m_queuedExternalCalls[i];

    // call virtual function
    OnExternalInterfaceCall(pData->m_sMethodName, pData->m_ppArgs, pData->m_uiArgCount);

    // ..and also trigger the callback
    VOnExternalInterfaceCall::OnExternalInterfaceCallback.TriggerCallbacks(pData);

    //inform scripting system about the callback...
    IVScriptInstance *pSceneInst = VScriptResourceManager::GlobalManager().GetSceneScript();
    IVScriptInstance *pGameInst = VScriptResourceManager::GlobalManager().GetGameScript();

    // call 'OnExternalInterfaceCall' for the scene script (if callback is present)
    if(pSceneInst != NULL || pGameInst != NULL)
    {
      VASSERT_MSG(pData->m_uiArgCount < 59, 
        "Exceeded max parameter count of 59 for VScaleformExternalInterfaceHandler in Lua (table truncated after parameter 5)");

      char** ppArray = new char* [pData->m_uiArgCount];
      VString* stringRepresentation = new VString[pData->m_uiArgCount];

      char szFormatStringBuffer[64] = "ss[";
      for(unsigned int i = 0; i < pData->m_uiArgCount; i++)
      {
        // this 'copy' is required since 'ToString()' will be
        // removed from the stack at the end of the loop
        stringRepresentation[i] = pData->m_ppArgs[i]->ToString().AsChar(); 

        // assign to void * array
        ppArray[i] = stringRepresentation[i].GetChar();

        // add the string parameter to the format string...
        szFormatStringBuffer[i+3] = 's';
      }

      // terminate the format string
      szFormatStringBuffer[pData->m_uiArgCount + 3] = ']';
      szFormatStringBuffer[pData->m_uiArgCount + 4] = 0;

      // scene script
      if (pSceneInst != NULL)
      {
        pSceneInst->ExecuteFunctionArg("OnExternalInterfaceCall", szFormatStringBuffer, 
          GetFileName(), pData->m_sMethodName.AsChar(), ppArray);
      }

      // game script
      if (pGameInst != NULL)
      {
        pGameInst->ExecuteFunctionArg("OnExternalInterfaceCall", szFormatStringBuffer, 
          GetFileName(), pData->m_sMethodName.AsChar(), ppArray);
      }

      V_SAFE_DELETE_ARRAY(ppArray);
      V_SAFE_DELETE_ARRAY(stringRepresentation);
    }

    delete pData;
  }

  m_queuedExternalCalls.SetSize(0, -1, false  /* Do not deallocate */);
}

VOnExternalInterfaceCall::VOnExternalInterfaceCall(
  VScaleformMovieInstance *pMovie, const char* szMethodName, const Scaleform::GFx::Value* pArgs, unsigned int uiArgCount) 
  : IVisCallbackDataObject_cl(&VOnExternalInterfaceCall::OnExternalInterfaceCallback)
  , m_pMovie(pMovie)
  , m_sMethodName(szMethodName)
  , m_ppArgs(NULL)
  , m_uiArgCount(uiArgCount)
{
  if(uiArgCount > 0)
  {
    m_ppArgs = new VScaleformValueConst*[uiArgCount];
    
    for(unsigned int i = 0; i < uiArgCount; i++)
    {
      // Create wrapped value and make copy of GFXValue.
      // Original value will not be accessible late on.
      m_ppArgs[i] = new VScaleformValueConst(new Scaleform::GFx::Value(pArgs[i]));

      // We're currently in the advance thread. So it is safe to sync the values.
      m_ppArgs[i]->SyncReferenceValueWithScaleform();
    }
  }
}

VOnExternalInterfaceCall::~VOnExternalInterfaceCall()
{
  for(unsigned int i = 0; i < m_uiArgCount; i++)
  {
    delete m_ppArgs[i];
  }
  V_SAFE_DELETE_ARRAY(m_ppArgs);
}

VOnFSCommand::VOnFSCommand(VScaleformMovieInstance *pMovie, const char* szCommand, const char* szArgs) 
  : IVisCallbackDataObject_cl(&VOnFSCommand::OnFSCallback)
  , m_pMovie(pMovie)
  , m_sCommand(szCommand)
  , m_sArgs(szArgs)
{
}

VOnFSCommand::~VOnFSCommand()
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
