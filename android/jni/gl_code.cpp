/*
  gl_code.cpp
  Coin eater android app implementation.

  Copyright (c) 2016 Waync Cheng.
  All Rights Reserved.

  2016/9/25 Waync created
 */

#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define JNI_ACTIVITY "weilican/good/goodJniActivity"

#define  LOG_TAG    "libgood"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include <algorithm>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <cctype>

#define glOrtho glOrthof                // Fixed GLES.

JNIEnv *tmpEnv;

#include "rt/rt.h"
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

  void doTrigger(int keys, int mousex, int mousey)
  {
    good::rt::AndroidApplication<CoinEaterApp>::doTrigger(keys, mousex, mousey);
    if (-1 != mRoot && "coin eater" == mRes.mName) {
      game.step_move();
    }
  }
};

CoinEaterApp& g = CoinEaterApp::getInst();

#include "app/android_lib.h"
