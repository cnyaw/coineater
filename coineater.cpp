// coineater.cpp : main source file for coineater.exe
//

#include "stdafx.h"

#include <time.h>

#include <atlframe.h>
#include <atlcrack.h>
#include <atlmisc.h>

#include "resource.h"

#include "../good/src/good/rt/rt.h"
#include "../good/src/good/app/wtl_player.h"

#include "coineater.h"

class CPlayer : public good::rt::CPlayerWindowImpl<CPlayer>
{
  CPlayer()
  {
    tip = "Press O to toggle trace messages";
    showFPS = true;
  }
public:

  typedef good::rt::CPlayerWindowImpl<CPlayer> BaseT;

  CoinEaterGame<CPlayer> game;

  static CPlayer& getInst()
  {
    static CPlayer inst;
    return inst;
  }

  void onAppCreate()
  {
    game.load_game();
  }

  void onAppDestroy()
  {
    game.save_game();
  }

  void doUserIntEvent(int i)
  {
    game.handle_int_event(i);
  }

  void onTrigger()
  {
    if (-1 != mRoot && "coin eater" == mRes.mName) {
      game.step_move();
    }
  }

  BEGIN_MSG_MAP_EX(CPlayer)
    MSG_WM_CHAR(OnChar)
    CHAIN_MSG_MAP(BaseT)
  END_MSG_MAP()

  void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
    if ('O' == nChar) {
      ToggleOutput();
    } else if ('T' == nChar) {
      ToggleTexInfo();
    }
  }
};

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
  CGLMessageLoop theLoop;
  _Module.AddMessageLoop(&theLoop);

  ::srand((unsigned int)::time(NULL));

  CPlayer& app = CPlayer::getInst();

  if (app.CreateEx() == NULL) {
    ATLTRACE(_T("Main GL window creation failed!\n"));
    return 0;
  }

  if (!app.init("./coineater.txt")) {
    ATLTRACE(_T("Init good failed!\n"));
    return 0;
  }

  app.ShowWindow(SW_SHOW);

  timeBeginPeriod(1);
  int nRet = app.theLoop.Run();
  timeEndPeriod(1);

  app.game.save_game();

  _Module.RemoveMessageLoop();
  return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
