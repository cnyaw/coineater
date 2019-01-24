/*
  gl_code.cpp
  Coin eater android app implementation.

  Copyright (c) 2016 Waync Cheng.
  All Rights Reserved.

  2016/9/25 Waync created
 */

#include "app/android_app.h"

#include "../../coineater.h"

class CoinEaterApp : public good::rt::AndroidApplication<CoinEaterApp>
{
  CoinEaterApp()
  {
    srand((unsigned int)time(NULL));
  }
public:
  static CoinEaterApp& getInst()
  {
    static CoinEaterApp i;
    return i;
  }

  CoinEaterGame<CoinEaterApp> game;

  void onAppCreate()
  {
    game.load_game();
  }

  void onAppDestroy()
  {
    game.save_game();
  }

  void onAppPause()
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
};

CoinEaterApp& g = CoinEaterApp::getInst();

#include "app/android_lib.h"
