//
// coineater.h
// Coin eater implementation.
//
// Copyright (c) 2016 Waync Cheng.
// All Rights Reserved.
//
// 2016/9/25 Waync Cheng.
//

#pragma once

#include <string>

#include "swIni.h"
#include "swStageStack.h"

#include "gene.h"

enum COINEATER_GAME_CONST {
  RAND_CHANGE = 100,
  N_RAND_CHANGE = 3,
  TOTAL_GENERATIONS = 200,
  TILE_W = 32,
  TILE_H = 32,
  INIT_STEP_MOVE_TICK = 6,
  INIT_TOTAL_COIN_COUNT = 10000,
  INIT_START_POINT_TRAIN_COUNT = 200,
  BASE_TRAIN_COUNT = 2,
  INIT_MAX_COIN_EATER = 10,
  COST_PUT_COIN_EATER = 1,
  INIT_STABILITY = 5,
  MAX_STABILITY = 80,
  INC_STABILITY_COIN_EATER_COUNT = 500,
  MAX_AUTO_GAIN_COUNTER = 480,
  INIT_INC_MAX_COIN_EATER_COST = 100,
  MAX_MAX_COIN_EATER = 30,
  INIT_INC_STABILITY_COST = 250,
  INIT_COIN_HP = 200,
  EXTRA_COIN_HP_RANGE = 50,
  INC_COIN_EATER_ADD_EXTRA_COIN_MOD = 5,
  EXTRA_GENERATEION_RANGE = 30,
  UNLOCK_MAP_12_COST = 12500,
  UNLOCK_MAP_14_COST = 25000,
  INIT_COIN_EATER_WALKER_COST = 5,
  NEXT_EXTRA_COIN_EATER_WALKER_COST = 200,
  EXTRA_UNLOCK_MAP12_COIN = 5,
  MAX_CHEST_LEVEL = 15,
  INIT_NEXT_CHEST_COUNTER = 10
};

enum COINEATER_RESOURCE_ID {
  TEXTURE_COIN_ID = 6,
  TEXTURE_EATER_ID = 7,
  TEXTURE_EXCLAMATION_ID = 16,
  TEXTURE_CHEST_ID = 70,
  MAP_10_ID = 24,
  MAP_12_ID = 30,
  MAP_14_ID = 31,
  LEVEL_MAP_ID = 25,
  SOUND_GAIN_COIN = 67,
  CURR_SEL_COIN_EATER = 9
};

enum COINEATER_COMMAND {
  CMD_INIT_GAME = 1,
  CMD_SET_START_POINT,
  CMD_PUT_COIN_EATER,
  CMD_INC_MAX_COIN_EATER,
  CMD_INC_STABILITY,
  CMD_SEL_COIN_EATER,
  CMD_RESET_GAME,
  CMD_QUIT_GAME,
  CMD_GEN_INFO_MSG
};

#define COINEATER_SAVE_GAME_FILE_NAME "cecpp.sav"

static const int COIN_EATER_RES_ID[] = {7, 35, 34, 36, 37, 51, 38, 39, 40, 41,
                                        42, 13, 43, 44, 45, 46, 47, 48, 50, 52,
                                        57, 6, 55, 60, 56, 27, 53, 54, 58, 59};

const int COIN_EATER_RES_ID_COUNT = sizeof(COIN_EATER_RES_ID)/sizeof(COIN_EATER_RES_ID[0]);

class CoinObj
{
public:
  int obj_id;
  int pos;
  int hp;
  int coin_count;
};

class CoinEaterObj : public CoinEater
{
public:
  int obj_id;
  int tick;
};

template<class AppT>
class CoinEaterGame
{
public:
  std::vector<CoinEaterGene> pool;
  int start_pos;
  CoinEaterEnv env;
  std::map<int, CoinObj> coin_obj;      // <objid, CoinObj>
  std::map<int, CoinEaterObj> coin_eater;  // <objid, CoinEaterObj>
  int total_coin, gained_coin, spent_coin;
  int max_coin_eater, sent_coin_eater, next_max_coin_eater_cost;
  int avg_score, best_score;
  int stability, next_stability_cost;   // 1%~80%. When put coin eater, used as selection range.
  int stability_counter, next_stability_counter;
  int unlock_next_map_cost;
  int auto_gain_counter;                // Auto gain 1 coin by time.
  int cur_coin_eater_res_idx, sel_coin_eater_res;
  int next_walker_counter, next_walker_cost;
  int chest_pos, next_chest_counter;
  int replay_count;
  int play_time;
  int max_iq;
  sw2::StageStack<CoinEaterGame<AppT> > stage;

  CoinEaterGame() : start_pos(-1)
  {
    cur_coin_eater_res_idx = gained_coin = spent_coin = sent_coin_eater = play_time = replay_count = 0;
    max_iq = -10000;
    sel_coin_eater_res = TEXTURE_EATER_ID;
    next_walker_counter = next_walker_cost = INIT_COIN_EATER_WALKER_COST;
  }

  static bool pred(CoinEaterGene const &a, CoinEaterGene const &b)
  {
    return a.score > b.score;
  }

  void load_game()
  {
    sw2::Ini ini;

    if (!ini.load(COINEATER_SAVE_GAME_FILE_NAME)) {
      return;
    }

    //
    // Attr.
    //

    sw2::Ini &attr = ini["attr"];

    cur_coin_eater_res_idx = attr["cur_coin_eater_res_idx"];
    sel_coin_eater_res = attr["sel_coin_eater_res"];
    next_walker_counter = attr["next_walker_counter"];
    next_walker_cost = attr["next_walker_cost"];

    total_coin = attr["total_coin"];
    gained_coin = attr["gained_coin"];
    spent_coin = attr["spent_coin"];

    avg_score = attr["avg_score"];
    best_score = attr["best_score"];

    max_coin_eater = attr["max_coin_eater"];
    sent_coin_eater = attr["sent_coin_eater"];
    next_max_coin_eater_cost = attr["next_max_coin_eater_cost"];

    stability = attr["stability"];
    next_stability_cost = attr["next_stability_cost"];
    stability_counter = attr["stability_counter"];
    next_stability_counter = attr["next_stability_counter"];

    unlock_next_map_cost = attr["unlock_next_map_cost"];

    replay_count = attr["replay_count"];
    play_time = attr["play_time"];

    if (attr.find("max_iq")) {
      max_iq = attr["max_iq"];
    }

    //
    // Gene pool.
    //

    sw2::Ini &gpool = ini["pool"];

    int pool_size = 0;
    pool_size = gpool["size"];

    pool.resize(pool_size);
    for (int i = 0; i < pool_size; i++) {
      char buff[32];
      sprintf(buff, "%d", i);
      pool[i].gene = gpool[(const char*)buff].value;
    }

    //
    // Map.
    //

    sw2::Ini &map = ini["map"];
    start_pos = map["start_pos"];

    const std::string &map_state = map["env"];
    env.map.resize(MAX_COL * MAX_ROW);
    for (size_t i = 0; i < env.map.size(); i++) {
      env.map[i] = map_state[i] - '0';
    }
  }

  void save_game() const
  {
    sw2::Ini ini;

    //
    // Attr.
    //

    sw2::Ini &attr = ini["attr"];

    attr["cur_coin_eater_res_idx"] = cur_coin_eater_res_idx;
    attr["sel_coin_eater_res"] = sel_coin_eater_res;
    attr["next_walker_counter"] = next_walker_counter;
    attr["next_walker_cost"] = next_walker_cost;

    attr["total_coin"] = total_coin;
    attr["gained_coin"] = gained_coin;
    attr["spent_coin"] = spent_coin;

    attr["avg_score"] = avg_score;
    attr["best_score"] = best_score;

    attr["max_coin_eater"] = max_coin_eater;
    attr["sent_coin_eater"] = sent_coin_eater;
    attr["next_max_coin_eater_cost"] = next_max_coin_eater_cost;

    attr["stability"] = stability;
    attr["next_stability_cost"] = next_stability_cost;
    attr["stability_counter"] = stability_counter;
    attr["next_stability_counter"] = next_stability_counter;

    attr["unlock_next_map_cost"] = unlock_next_map_cost;

    attr["replay_count"] = replay_count;
    attr["play_time"] = play_time;
    attr["max_iq"] = max_iq;

    //
    // Gene pool.
    //

    sw2::Ini &gpool = ini["pool"];
    gpool["size"] = pool.size();

    for (size_t i = 0; i < pool.size(); i++) {
      char buff[32];
      sprintf(buff, "%d", i);
      gpool[(const char*)buff] = pool[i].gene;
    }

    //
    // Map.
    //

    sw2::Ini &map = ini["map"];
    map["start_pos"] = start_pos;

    std::string str_map;
    for (size_t i = 0; i < env.map.size(); i++) {
      str_map += '0' + env.map[i];
    }

    map["env"] = str_map;

    ini.store(COINEATER_SAVE_GAME_FILE_NAME);
  }

  void init_game()
  {
    //
    // Init msg obj.
    //

    coin_obj.clear();
    coin_eater.clear();

    chest_pos = -1;
    next_chest_counter = INIT_NEXT_CHEST_COUNTER;

    AppT &app = AppT::getInst();

    if (-1 == start_pos) {

      //
      // Init counters.
      //

      start_pos = -1;
      total_coin = INIT_TOTAL_COIN_COUNT;
      max_coin_eater = INIT_MAX_COIN_EATER;
      stability_counter = auto_gain_counter = 0;
      avg_score = best_score = -10000;
      stability = INIT_STABILITY;
      next_max_coin_eater_cost = INIT_INC_MAX_COIN_EATER_COST;
      next_stability_cost = INIT_INC_STABILITY_COST;
      next_stability_counter = INC_STABILITY_COIN_EATER_COUNT;
      unlock_next_map_cost = UNLOCK_MAP_12_COST;

      //
      // Init coin eater gene pool.
      //

      pool.resize(TOTAL_GENERATIONS);
      for (int i = 0; i < (int)pool.size(); i++) {
        pool[i].init();
      }

      //
      // Init map.
      //

      env.init_map();

      //
      // Init game stage.
      //

      stage.initialize(this, &CoinEaterGame::stage_init);

    } else {

      //
      // Set map lock state.
      //

      int idMap = MAP_10_ID;
      if (S_WALL != env.map[1 + MAX_COL]) { // 12x12 is unlock.
        idMap = MAP_12_ID;
      } else if (S_WALL != env.map[0]) { // 14x14 is unlock.
        idMap = MAP_14_ID;
      }
      app.doLuaScript("SetMapTex(%d,%d)", LEVEL_MAP_ID, idMap);

      //
      // Init map coins .
      //

      create_map_coin_objs();

      //
      // Init game stage.
      //

      stage.initialize(this, &CoinEaterGame::stage_game);
    }

    //
    // Init coin eater collections.
    //

    if (0 != cur_coin_eater_res_idx ||
        TEXTURE_EATER_ID != sel_coin_eater_res ||
        INIT_COIN_EATER_WALKER_COST != next_walker_cost) {
      std::string res_lst;
      res_lst += '{';
      for (int i = 0; i < cur_coin_eater_res_idx; i++) {
        char buff[32];
        sprintf(buff, "%d,", COIN_EATER_RES_ID[i]);
        res_lst += buff;
      }
      res_lst[res_lst.size() - 1] = '}';
      int next_res = COIN_EATER_RES_ID_COUNT > cur_coin_eater_res_idx ? COIN_EATER_RES_ID[cur_coin_eater_res_idx] : -1;
      app.doLuaScript("InitCoinEaterWalker(%s,%d)", res_lst.c_str(), next_res);
    }

    if (-1 == start_pos) {
      app.doLuaScript("InitGame()");
    } else {
      app.doLuaScript("ContinueGame(%d)", start_pos);
    }

    update_ui_msg();
    sel_coin_eater(sel_coin_eater_res);
  }

  void init_train()
  {
    stage.popAndPush(&CoinEaterGame::stage_init_train);
  }

  void set_start_pos(int init_pos_col, int init_pos_row)
  {
    //
    // Init env state.
    //

    start_pos = init_pos_col + init_pos_row * MAX_COL;
    create_map_coin_objs();

    replay_count += 1;
    save_game();
  }

  void set_obj_pos(AppT &app, int o, int col, int row, float offsetx = .0f, float offsety = .0f) const
  {
    app.setPos(o, col * (float)TILE_W + offsetx, row * (float)TILE_H + offsety);
  }

  int get_chest_level() const
  {
    return (std::min)(cur_coin_eater_res_idx, (int)MAX_CHEST_LEVEL);
  }

  int get_chest_coin_count() const
  {
    int level = get_chest_level();
    int sum = 0;
    for (int i = 1; i <= level; i++) {
      sum += i;
    }
    sum = 1 + (rand() % sum);
    for (int i = level; i >= 1; i--) {
      sum -= i;
      if (0 >= sum) {
        return 2 * (level - i + 1);
      }
    }
    return 1;
  }

  void create_map_coin_objs()
  {
    //
    // Create map obj.
    //

    AppT &app = AppT::getInst();

    coin_obj.clear();
    for (int row = 0; row < MAX_ROW; row++) {
      int pos = row * MAX_COL;
      for (int col = 0; col < MAX_COL; col++, pos++) {
        if (S_COIN != env.map[pos]) {
          continue;
        }
        CoinObj c;
        c.obj_id = app.genObj(LEVEL_MAP_ID, TEXTURE_COIN_ID, "");
        c.hp = INIT_COIN_HP + (c.obj_id % EXTRA_COIN_HP_RANGE);
        c.coin_count = 1;
        c.pos = pos;
        set_obj_pos(app, c.obj_id, col, row);
        coin_obj[c.obj_id] = c;
      }
    }
  }

  void next_generation()
  {
    //
    // Generate next generation coin eater.
    //

    std::vector<std::string> next_gene;
    next_gene.resize(TOTAL_GENERATIONS);

    int s = (std::min)((int)MAX_STABILITY, EXTRA_GENERATEION_RANGE + stability);
    int range = (int)(((100 - s) / 100.0f) * pool.size());

    for (size_t i = 0; i < pool.size(); i++) {
      int igene;
      if (80 >= rand() % 100) {
        igene = rand() % range;
      } else {
        igene = range + (rand() % (pool.size() - range));
      }
      int cut_pos = rand() % LEN_GENE;
      std::string gene1(pool[i].gene, 0, cut_pos);
      gene1.append(pool[igene].gene.substr(cut_pos));
      if (5 > (rand() % RAND_CHANGE)) {
        int n = 1 + rand() % N_RAND_CHANGE;
        for (int j = 0; j < n; j++) {
          gene1[rand() % LEN_GENE] = '0' + (rand() % A_COUNT);
        }
      }
      next_gene[i] = gene1;
    }

    for (size_t i = 0; i < pool.size(); i++) {
      pool[i].gene = next_gene[i];
    }
  }

  void score_coin_eater(int test_round)
  {
    //
    // Calc coin eaters score.
    //

    for (size_t i = 0; i < pool.size(); i++) {
      pool[i].score = 0;
      for (int j = 0; j < test_round; j++) {
        pool[i].score += pool[i].sim(start_pos);
      }
      pool[i].score /= test_round;
    }

    std::sort(pool.begin(), pool.end(), &pred);

    avg_score = 0;
    for (size_t i = 0; i < pool.size(); i++) {
      avg_score += pool[i].score;
    }
    avg_score /= (int)pool.size();
    best_score = (std::max)(best_score, avg_score);
    max_iq = (std::max)(max_iq, best_score);
  }

  void train(int test_round)
  {
    next_generation();
    score_coin_eater(test_round);
  }

  int get_pool_range() const
  {
    return (int)(((100 - stability) / 100.0f) * pool.size());
  }

  void update_stability_counter()
  {
    stability_counter += 1;
    if (next_stability_counter <= stability_counter) {
      stability = (std::min)((int)MAX_STABILITY, stability + 1);
      stability_counter = 0;
      next_stability_counter += INC_STABILITY_COIN_EATER_COUNT;
      save_game();
    }
  }

  void put_coin_eater()
  {
    if (-1 == start_pos || COST_PUT_COIN_EATER > total_coin || max_coin_eater <= (int)coin_eater.size()) {
      return;
    }

    AppT &app = AppT::getInst();

    CoinEaterObj c;
    c.hp = TOTAL_MOVES + INIT_MAX_COIN_EATER - max_coin_eater;
    c.tick = 0;
    c.from_pos = c.pos = start_pos;
    c.gene = pool[rand() % get_pool_range()].gene;
    c.obj_id = app.genObj(LEVEL_MAP_ID, sel_coin_eater_res, "");
    set_obj_pos(app, c.obj_id, start_pos % MAX_COL, start_pos / MAX_COL);

    coin_eater[c.obj_id] = c;
    sent_coin_eater += 1;

    train(BASE_TRAIN_COUNT);
    total_coin -= COST_PUT_COIN_EATER;
    spent_coin += COST_PUT_COIN_EATER;

    update_stability_counter();
    update_coin_obj_hp();
  }

  void inc_max_coin_eater()
  {
    if (MAX_MAX_COIN_EATER == max_coin_eater || next_max_coin_eater_cost > total_coin) {
      return;
    }

    max_coin_eater += 1;
    total_coin -= next_max_coin_eater_cost;
    spent_coin += next_max_coin_eater_cost;
    next_max_coin_eater_cost = (int)(next_max_coin_eater_cost * 1.35f);

    //
    // Every INC_COIN_EATER_ADD_EXTRA_COIN_MOD coin eater add 1 coin to the map.
    //

    if (0 == ((max_coin_eater - INIT_MAX_COIN_EATER) % INC_COIN_EATER_ADD_EXTRA_COIN_MOD)) {
      gen_coin_obj();
    }

    save_game();
  }

  void inc_stability()
  {
    if (MAX_STABILITY == stability || next_stability_cost > total_coin) {
      return;
    }

    stability += 1;
    total_coin -= next_stability_cost;
    spent_coin += next_stability_cost;
    next_stability_cost = (int)(next_stability_cost * 1.2f);

    save_game();
  }

  void update_ui_msg()
  {
    AppT &app = AppT::getInst();
    app.doLuaScript("UpdateTotalCoinMsg(%d,%d,%d)", total_coin, -1 == start_pos ? 0 : MAX_AUTO_GAIN_COUNTER, auto_gain_counter);
    app.doLuaScript("UpdateCoinEaterMsg(%d,%d,%d,%d)", coin_eater.size(), max_coin_eater, next_max_coin_eater_cost, MAX_MAX_COIN_EATER);
    app.doLuaScript("UpdateScoreMsg(%d,%d)", avg_score, best_score);
    app.doLuaScript("UpdateStabilityMsg(%d,%d,%d)", stability, next_stability_cost, MAX_STABILITY);
    app.doLuaScript("UpdateUnlockNextMapMsg(%d,%d)", -1 == start_pos || S_WALL != env.map[0] ? 0 : unlock_next_map_cost);
    app.doLuaScript("UpdateNextWalkerMsg(%d)", next_walker_counter);
  }

  void gen_coin_obj()
  {
    AppT &app = AppT::getInst();

    int pos = env.get_rand_empty_pos();

    int coin_tex_id = TEXTURE_COIN_ID;
    int cc = 1;
    if (-1 == chest_pos) {
      next_chest_counter -= 1;
      if (0 >= next_chest_counter) {
        cc = get_chest_coin_count();
        chest_pos = pos;
        coin_tex_id = TEXTURE_CHEST_ID;
      }
    }

    int o = app.genObj(LEVEL_MAP_ID, coin_tex_id, "");
    app.addChild(LEVEL_MAP_ID, o, 0);
    set_obj_pos(app, o, pos % MAX_COL, pos / MAX_COL);
    CoinObj c;
    c.obj_id = o;
    c.hp = INIT_COIN_HP + (o % EXTRA_COIN_HP_RANGE);
    c.coin_count = cc;
    c.pos = pos;
    coin_obj[c.obj_id] = c;
    env.map[pos] = S_COIN;
  }

  void update_coin_obj_hp()
  {
    //
    // Every time put a new coin eater, all coin obj reduced 1 hp.
    // If coin obj hp becomes 0 then move it to new pos.
    //

    AppT &app = AppT::getInst();

    std::map<int, CoinObj>::iterator it = coin_obj.begin();
    for (; coin_obj.end() != it; ++it) {
      CoinObj &c = it->second;
      c.hp -= 1;
      if (0 >= c.hp) {
        env.map[c.pos] = S_EMPTY;
        int new_pos = env.get_rand_empty_pos();
        env.map[new_pos] = S_COIN;
        if (c.pos == chest_pos) {
          chest_pos = new_pos;
        }
        c.pos = new_pos;
        c.hp = INIT_COIN_HP + (c.obj_id % EXTRA_COIN_HP_RANGE);
        set_obj_pos(app, c.obj_id, new_pos % MAX_COL, new_pos / MAX_COL);
      }
    }
  }

  std::map<int, CoinObj>::iterator find_coin_obj(int pos)
  {
    std::map<int, CoinObj>::iterator it = coin_obj.begin();
    for (; coin_obj.end() != it; ++it) {
      if (it->second.pos == pos) {
        return it;
      }
    }
    return coin_obj.end();
  }

  void step_move()
  {
    stage.trigger();
  }

  void stage_init(int state, sw2::uint_ptr)
  {
    // NOP.
  }

  void stage_init_train(int state, sw2::uint_ptr)
  {
    static int pool_index = 0;

    if (sw2::JOIN == state) {
      pool_index = 0;
      return;
    }

    if (sw2::TRIGGER == state) {
      pool[pool_index].score = 0;
      for (int i = 0; i < INIT_START_POINT_TRAIN_COUNT; i++) {
        pool[pool_index].score += pool[pool_index].sim(start_pos);
      }
      pool[pool_index].score /= INIT_START_POINT_TRAIN_COUNT;
      pool_index += 1;
      if (pool.size() == pool_index) {
        std::sort(pool.begin(), pool.end(), &pred);
        train(1);
        stage.popAndPush(&CoinEaterGame::stage_game);
      }
      AppT::getInst().doLuaScript("InitTrainStep(%d,%d)", pool_index, pool.size());
    }
  }

  void unlock_map_12()
  {
    AppT &app = AppT::getInst();

    for (int col = 1; col < MAX_COL - 1; col++) {
      env.map[col + MAX_COL] = S_EMPTY;
    }
    for (int row = 2; row < MAX_ROW - 2; row++) {
      env.map[1 + row * MAX_COL] = S_EMPTY;
      env.map[MAX_COL - 2 + row * MAX_COL] = S_EMPTY;
    }
    for (int col = 1; col < MAX_COL - 1; col++) {
      env.map[col + (MAX_ROW - 2) * MAX_COL] = S_EMPTY;
    }

    for (int i = 0; i < EXTRA_UNLOCK_MAP12_COIN; i++) {
      gen_coin_obj();
    }

    app.doLuaScript("SetMapTex(%d,%d)", LEVEL_MAP_ID, MAP_12_ID);
    app.doLuaScript("UnlockMap12()");
  }

  void unlock_map_14()
  {
    AppT &app = AppT::getInst();

    for (int col = 0; col < MAX_COL; col++) {
      env.map[col] = S_EMPTY;
    }
    for (int row = 1; row < MAX_ROW - 1; row++) {
      env.map[row * MAX_COL] = S_EMPTY;
      env.map[MAX_COL - 1 + row * MAX_COL] = S_EMPTY;
    }
    for (int col = 0; col < MAX_COL; col++) {
      env.map[col + (MAX_ROW - 1) * MAX_COL] = S_EMPTY;
    }

    app.doLuaScript("SetMapTex(%d,%d)", LEVEL_MAP_ID, MAP_14_ID);
    app.doLuaScript("UnlockMap14()");
  }

  void check_unlock_map(int count)
  {
    if (S_EMPTY == env.map[0]) {
      return;
    }

    unlock_next_map_cost -= count;
    if (0 >= unlock_next_map_cost) {
      if (S_WALL == env.map[1 + MAX_COL]) {
        unlock_next_map_cost += UNLOCK_MAP_14_COST;
        unlock_map_12();
        save_game();
      } else if (S_WALL == env.map[0]) {
        unlock_next_map_cost = 0;
        unlock_map_14();
        save_game();
      }
    }
  }

  void gain_coin(int coin_pos)
  {
    AppT &app = AppT::getInst();

    std::map<int, CoinObj>::iterator it_coin = find_coin_obj(coin_pos);
    int cc = it_coin->second.coin_count;
    app.killObj(it_coin->second.obj_id);
    coin_obj.erase(it_coin);
    gen_coin_obj();

    if (coin_pos == chest_pos) {
      chest_pos = -1;
      next_chest_counter = INIT_NEXT_CHEST_COUNTER;
    }

    total_coin += cc;
    gained_coin += cc;
    for (int i = 0; i < cc; i++) {
      int o = app.genObj(LEVEL_MAP_ID, TEXTURE_COIN_ID, 1 < cc ? "GainChestCoin" : "GainCoin");
      set_obj_pos(app, o, coin_pos % MAX_COL, coin_pos / MAX_COL);
    }

    app.doLuaScript("PlaySound(%d)", SOUND_GAIN_COIN);

    check_unlock_map(cc);
    check_coin_eater_walker_counter(cc);
  }

  void step_move_coin_eater(CoinEaterObj &c)
  {
    int from_col = c.from_pos % MAX_COL;
    int from_row = c.from_pos / MAX_COL;
    int to_col = c.pos % MAX_COL;
    int to_row = c.pos / MAX_COL;
    float delta = (c.tick / (float)INIT_STEP_MOVE_TICK);
    float offsetx = TILE_W * (to_col - from_col) * delta;
    float offsety = TILE_H * (to_row - from_row) * delta;
    set_obj_pos(AppT::getInst(), c.obj_id, from_col, from_row, offsetx, offsety);
  }

  void check_coin_eater_walker_counter(int count)
  {
    if (-1 == next_walker_counter) {
      return;
    }

    next_walker_counter -= count;
    if (0 >= next_walker_counter) {
      next_walker_cost += NEXT_EXTRA_COIN_EATER_WALKER_COST;
      next_walker_counter = next_walker_cost;
      if (COIN_EATER_RES_ID_COUNT > cur_coin_eater_res_idx) {
        int res = COIN_EATER_RES_ID[cur_coin_eater_res_idx];
        cur_coin_eater_res_idx += 1;
        int next_res = COIN_EATER_RES_ID_COUNT > cur_coin_eater_res_idx ? COIN_EATER_RES_ID[cur_coin_eater_res_idx] : -1;
        AppT::getInst().doLuaScript("AddCoinEaterWalker(%d,%d)", res, next_res);
        save_game();
      } else {
        next_walker_counter = -1;
      }
    }
  }

  void stage_game(int state, sw2::uint_ptr)
  {
    AppT &app = AppT::getInst();

    auto_gain_counter += 1;
    if (MAX_AUTO_GAIN_COUNTER == auto_gain_counter) {
      total_coin += 1;
      auto_gain_counter = 0;
    }

    play_time += 1;

    std::map<int, CoinEaterObj>::iterator it = coin_eater.begin();
    for (; coin_eater.end() != it;) {
      CoinEaterObj &c = it->second;
      c.tick += 1;
      if (INIT_STEP_MOVE_TICK > c.tick) {
        step_move_coin_eater(c);
        it++;
        continue;
      }
      c.tick = 0;
      int coin_pos, score;
      if (c.step_move(env, coin_pos, score)) {
        gain_coin(coin_pos);
      } else if (0 > score) {
        int o = app.genObj(c.obj_id, TEXTURE_EXCLAMATION_ID, "StateIcon");
        app.setPos(o, 0, -16);
      }
      if (0 >= c.hp) {
        app.killObj(c.obj_id);
        coin_eater.erase(it++);
      } else {
        ++it;
      }
    }

    update_ui_msg();
  }

  void sel_coin_eater(int sel_res)
  {
    sel_coin_eater_res = sel_res;
    AppT::getInst().setTexId(CURR_SEL_COIN_EATER, sel_res);
  }

  void gen_info_msg()
  {
    AppT::getInst().doLuaScript("GenInfoMsg(%d,%d,%d,%d,%d,%d,%d)", gained_coin, spent_coin, sent_coin_eater, get_chest_level(), replay_count, play_time / 60, max_iq);
  }

  void handle_int_event(int i)
  {
    int cmd = i & 0xff;
    int p1 = (i >> 8) & 0xff;
    int p2 = (i >> 16) & 0xff;

    switch (cmd)
    {
    case CMD_INIT_GAME:
      init_game();
      break;
    case CMD_SET_START_POINT:
      set_start_pos(p1, p2);
      init_train();
      break;
    case CMD_PUT_COIN_EATER:
      put_coin_eater();
      break;
    case CMD_INC_MAX_COIN_EATER:
      inc_max_coin_eater();
      break;
    case CMD_INC_STABILITY:
      inc_stability();
      break;
    case CMD_SEL_COIN_EATER:
      sel_coin_eater(p1);
      break;
    case CMD_RESET_GAME:
      start_pos = -1;
      // Fall throw.
    case CMD_QUIT_GAME:
      save_game();
      break;
    case CMD_GEN_INFO_MSG:
      gen_info_msg();
      break;
    }
  }
};

// end of coineater.h
