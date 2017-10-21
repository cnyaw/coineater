//
// gene.h
// Coin eater gene.
//
// Copyright (c) 2017 Waync Cheng.
// All Rights Reserved.
//
// 2017/2/25 Waync Cheng.
//

#pragma once

enum COINEATER_CONST {
  MAX_COL = 14,
  MAX_ROW = 14,
  INIT_COL = 10,
  INIT_ROW = 10,
  INIT_MAP_COIN_COUNT = 32,
  LEN_GENE = 243,
  TOTAL_MOVES = 100,
};

enum COINEATER_STATE {
  S_EMPTY = 0,
  S_COIN,
  S_WALL,
  S_COUNT
};

enum COINEATER_ACTION {
  A_UP = 0,
  A_LEFT,
  A_DOWN,
  A_RIGHT,
  A_EAT,
  A_COUNT
};

enum COINEATER_SCORE {
  EAT_OK = 10,
  EAT_FAIL = -10,
  MOVE_FAIL = -10
};

class CoinEaterEnv
{
public:
  std::vector<int> map;

  void init_map()
  {
    int left_col = (MAX_COL - INIT_COL) / 2;
    int right_col = left_col + INIT_COL;
    int top_row = (MAX_ROW - INIT_ROW) / 2;
    int bottom_row = top_row + INIT_ROW;

    map.resize(MAX_COL * MAX_ROW);
    for (int pos = 0; pos < MAX_COL * MAX_ROW; pos++) {
      int col = pos % MAX_COL;
      int row = pos / MAX_COL;
      if (left_col > col || right_col <= col || top_row > row || bottom_row <= row) {
        map[pos] = S_WALL;
      } else {
        map[pos] = S_EMPTY;
      }
    }

    for (int i = 0; i < INIT_MAP_COIN_COUNT; i++) {
      int pos = get_empty_pos();
      map[pos] = S_COIN;
    }
  }

  int get_empty_pos() const
  {
    while (true)
    {
      int pos = rand() % (MAX_COL * MAX_ROW);
      if (S_EMPTY == map[pos]) {
        return pos;
      }
    }
    return -1;
  }
};

class CoinEaterGene
{
public:
  int score;
  std::string gene;

  void init()
  {
    gene.resize(LEN_GENE);
    for (int i = 0; i < LEN_GENE; i++) { // Random init gene.
      gene[i] = (char)('0' + (rand() % A_COUNT));
    }
  }

  static int get_state(const CoinEaterEnv &env, int pos)
  {
    int col = pos % MAX_COL;
    int row = pos / MAX_COL;

    int b0 = env.map[pos];              // Center.
    int b1 = MAX_COL - 1 == col ? S_WALL : env.map[pos + 1]; // Right.
    int b2 = 0 == col ? S_WALL : env.map[pos - 1]; // Left.
    int b3 = MAX_ROW - 1 == row ? S_WALL : env.map[pos + MAX_COL]; // Down.
    int b4 = 0 == row ? S_WALL : env.map[pos - MAX_COL]; // Up.

    return b0 + 3 * b1 + 9 * b2 + 27 * b3 + 81 * b4;
  }

  static int step_move(int &pos, const std::string &gene, CoinEaterEnv &env, int &coin_pos)
  {
    int score = 0;
    int s = get_state(env, pos);
    int col = pos % MAX_COL;
    int row = pos / MAX_COL;
    coin_pos = -1;
    switch (gene[s] - '0')
    {
    case A_UP:
      if (0 == row || S_WALL == env.map[pos - MAX_COL]) {
        score += MOVE_FAIL;
      } else {
        pos -= MAX_COL;
      }
      break;
    case A_LEFT:
      if (0 == col || S_WALL == env.map[pos - 1]) {
        score += MOVE_FAIL;
      } else {
        pos -= 1;
      }
      break;
    case A_DOWN:
      if (MAX_ROW - 1 == row || S_WALL == env.map[pos + MAX_COL]) {
        score += MOVE_FAIL;
      } else {
        pos += MAX_COL;
      }
      break;
    case A_RIGHT:
      if (MAX_COL - 1 == col || S_WALL == env.map[pos + 1]) {
        score += MOVE_FAIL;
      } else {
        pos += 1;
      }
      break;
    case A_EAT:
      if (env.map[pos]) {
        score += EAT_OK;
        env.map[pos] = 0;
        coin_pos = pos;
      } else {
        score += EAT_FAIL;
      }
      break;
    }
    return score;
  }

  int sim(int start_pos) const
  {
    //
    // Init map.
    //

    CoinEaterEnv test_env;
    test_env.init_map();

    //
    // Start simulation.
    //

    int score = 0, dummy_coin_pos;
    for (int i = 0; i < TOTAL_MOVES; i++) {
      score += step_move(start_pos, gene, test_env, dummy_coin_pos);
    }

    return score;
  }
};

class CoinEater
{
public:
  std::string gene;
  int from_pos, pos;
  int hp;

  bool step_move(CoinEaterEnv &env, int &coin_pos, int &score)
  {
    from_pos = pos;
    score = CoinEaterGene::step_move(pos, gene, env, coin_pos);
    hp -= 1;                            // -1 for each step_move.
    if (0 > score) {
      hp += score;                      // Fail punishment.
    }
    return -1 != coin_pos;
  }
};

// end of gene.h
