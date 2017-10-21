/*
  CoinEaterActivity.java

  Copyright (c) 2016 Waync Cheng.
  All Rights Reserved.

  2016/9/25 Waync created
 */

package weilican.coineater;

import android.os.Bundle;

import weilican.good.*;

public class CoinEaterActivity extends goodJniActivity
{
  @Override protected void onCreate(Bundle b)
  {
    super.onCreate(b);
  }

  @Override protected void DoChooseFile() {
    goodJniLib.create("coineater.good", getAssets());
  }
}
