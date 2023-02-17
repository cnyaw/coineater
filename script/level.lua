math.randomseed(os.time())

local SAVE_FILE_NAME = "coineater.sav"

local CMD_INIT_GAME = 1
local CMD_SET_START_POINT = 2
local CMD_PUT_COIN_EATER = 3
local CMD_INC_MAX_COIN_EATER = 4
local CMD_INC_STABILITY = 5
local CMD_RESET_GAME = 7
local CMD_QUIT_GAME = 8
local CMD_GEN_INFO_MSG = 9

local DIALOG_W, DIALOG_H = 260, 120
local OPTION_DIALOG_W, OPTION_DIALOG_H = 260, 260
local DIALOG_TEXT_COLOR = 0xff000000
local DIALOG_FONT_SIZE = 16

local auto_switch = nil
local auto_counter = 0

local snd_switch = nil
local bgm_switch = nil
local bgm_id = -1

local init_game_mask = nil

local init_train_dialog = nil
local init_train_progress = nil
local quit_game_dialog = nil
local reset_game_dialog = nil

local option_dialog = nil
local option_snd = nil
local option_bgm = nil
local option_reset = nil
local option_info = nil

local info_dialog = nil
local info_msg_root = nil

if (nil == snd_switch) then             -- Not load toggle state yet, load them from save file.
  local inf = io.open(SAVE_FILE_NAME, "r")
  if (nil == inf) then
    snd_switch = 1
    bgm_switch = 1
    auto_switch = 0
  else
    snd_switch, bgm_switch, auto_switch = inf:read("*number", "*number", "*number")
    inf:close()
  end
  if (1 == bgm_switch) then
    bgm_id = Sound.PlaySound(68)
  end
end

function SaveGame()
  local outf = io.open(SAVE_FILE_NAME, "w")
  outf:write(snd_switch, " ", bgm_switch, " ", auto_switch)
  outf:close()
end

function UpdateSndSwitchState()
  UpdateSwitch(snd_switch, option_snd)
end

function UpdateBgmSwitchState()
  UpdateSwitch(bgm_switch, option_bgm)
end

function UpdateAutoSwitchState()
  UpdateSwitch(auto_switch, 4)
end

function ToggleSndSwitch()
  snd_switch = ToggleSwitch(snd_switch)
  UpdateSndSwitchState()
  PlaySound(71)
  SaveGame()
end

function ToggleBgmSwitch()
  bgm_switch = ToggleSwitch(bgm_switch)
  UpdateBgmSwitchState()
  if (1 == bgm_switch) then
    bgm_id = Sound.PlaySound(68)
  else
    Sound.KillSound(bgm_id)
    bgm_id = -1
  end
  PlaySound(71)
  SaveGame()
end

function ToggleAutoSwitch(x, y)
  local ox, oy = Good.GetPos(4)
  local l,t,w,h = Good.GetDim(4)
  if (PtInRect(x, y, ox - 20, oy - 20, ox + w + 20, oy + h + 20)) then
    auto_switch = ToggleSwitch(auto_switch)
    if (1 == auto_switch) then
      auto_counter = 0
    end
    UpdateAutoSwitchState()
    PlaySound(71)
    SaveGame()
    return true
  end
  return false
end

function UpdateAutoGainCounter()
  auto_counter = auto_counter + 1
  if (60 == auto_counter) then
    if (1 == auto_switch) then
      Good.FireUserIntEvent(CMD_PUT_COIN_EATER)
    end
    auto_counter = 0
  end
end

function InitGame()
  local mask_color = 0xc0000000
  local W,H = Good.GetWindowSize()
  local x,y = Good.GetPos(25)
  local l,t,w,h = Good.GetDim(25)
  init_game_mask = Good.GenDummy(-1)
  local up = GenColorObj(init_game_mask, W, y, mask_color)
  local left = GenColorObj(init_game_mask, x, h, mask_color)
  Good.SetPos(left, 0, y)
  local right = GenColorObj(init_game_mask, x + w, h, mask_color)
  Good.SetPos(right, x + w, y)
  local down = GenColorObj(init_game_mask, W, H - (y + h), mask_color)
  Good.SetPos(down, 0, y + h)
  local msg = Good.GenTextObj(init_game_mask, 'Please select a start point on the map', 22)
  Good.SetPos(msg, 32, y + h + 32)
  if (1 == bgm_switch) then
    if (-1 == bgm_id) then
      bgm_id = Sound.PlaySound(68)
    end
  else
    if (-1 ~= bgm_id) then
      Sound.KillSound(bgm_id)
      bgm_id = -1
    end
  end
  Level.OnStep = OnStepInit
end

function ContinueGame(start_pos)
  local col = start_pos % 14
  local row = math.floor(start_pos / 14)
  local portal = Good.GenObj(25, 11)
  Good.SetPos(portal, 32 * col, 32 * row)
  Level.OnStep = OnStepGame
end

function InitTrain()
  if (nil ~= init_game_mask) then
    Good.KillObj(init_game_mask)
    init_game_mask = nil
  end
  init_train_dialog = GenDialogObj(DIALOG_W, DIALOG_H)
  local msg = Good.GenTextObj(init_train_dialog, 'Please wait', 22)
  SetTextObjColor(msg, DIALOG_TEXT_COLOR)
  Good.SetPos(msg, 16, 24)
  init_train_progress = GenColorObj(init_train_dialog, DIALOG_W - 20, 10, 0xff000000)
  Good.SetPos(init_train_progress, (DIALOG_W - (DIALOG_W - 20))/2, 60)
end

function InitTrainStep(step, total)
  if (step == total) then
    Good.KillObj(init_train_dialog)
    init_train_dialog = nil
    Level.OnStep = OnStepGame
  else
    Good.KillObj(init_train_progress)
    init_train_progress = GenColorObj(init_train_dialog, DIALOG_W - 20, 10, 0xff000000)
    Good.SetPos(init_train_progress, (DIALOG_W - (DIALOG_W - 20))/2, 60)
    local progress = math.floor((DIALOG_W - 20) * step / total)
    GenColorObj(init_train_progress, progress, 10, 0xffff0000)
  end
end

function ShowConfirmDialog(s)
  local dlg = GenDialogObj(DIALOG_W, DIALOG_H)
  local msg = Good.GenTextObj(dlg, s, 22)
  SetTextObjColor(msg, DIALOG_TEXT_COLOR)
  Good.SetPos(msg, 16, 24)
  local cancel = Good.GenObj(dlg, 61)
  Good.SetPos(cancel, DIALOG_W - 24, - 16)
  local yes = Good.GenObj(dlg, 62)
  SetTextObjColor(yes, DIALOG_TEXT_COLOR)
  Good.SetPos(yes, (DIALOG_W - 100)/2, 74)
  return dlg
end

function ShowQuitConfirmDialog()
  quit_game_dialog = ShowConfirmDialog('Are you sure to quit?')
  Level.OnStep = OnQueryQuitGame
end

function CloseQuitConfirmDialog()
  Good.KillObj(quit_game_dialog)
  quit_game_dialog = nil
  Level.OnStep = OnStepGame
end

function ShowOptionDialog()
  option_dialog = GenDialogObj(OPTION_DIALOG_W, OPTION_DIALOG_H)
  local cancel = Good.GenObj(option_dialog, 61)
  Good.SetPos(cancel, OPTION_DIALOG_W - 24, - 16)

  local offset_x = 28
  option_snd = Good.GenObj(option_dialog, 65)
  Good.SetPos(option_snd, offset_x, 30)
  local snd_msg = Good.GenTextObj(option_dialog, 'Sound', DIALOG_FONT_SIZE)
  SetTextObjColor(snd_msg, 0xff000000)
  Good.SetPos(snd_msg, offset_x + 50, 42)
  UpdateSndSwitchState()

  option_bgm = Good.GenObj(option_dialog, 65)
  Good.SetPos(option_bgm, offset_x, 90)
  local bgm_msg = Good.GenTextObj(option_dialog, 'BGM', DIALOG_FONT_SIZE)
  SetTextObjColor(bgm_msg, 0xff000000)
  Good.SetPos(bgm_msg, offset_x + 50, 102)
  UpdateBgmSwitchState()

  option_reset = Good.GenObj(option_dialog, 66)
  Good.SetPos(option_reset, offset_x - 10, 160)
  local reset_msg_1 = Good.GenTextObj(option_reset, 'Keep coin eater collection and', DIALOG_FONT_SIZE)
  SetTextObjColor(reset_msg_1, 0xff000000)
  Good.SetPos(reset_msg_1, 0, 40)
  local reset_msg_2 = Good.GenTextObj(option_reset, 'play new game', DIALOG_FONT_SIZE)
  SetTextObjColor(reset_msg_2, 0xff000000)
  Good.SetPos(reset_msg_2, 0, 55)

  option_info = Good.GenObj(option_dialog, 73)
  Good.SetPos(option_info, offset_x + 150, 95)

  Level.OnStep = OnStepOption
end

function CloseOptionDialog()
  Good.KillObj(option_dialog)
  option_dialog = nil
  Level.OnStep = OnStepGame
end

function ShowResetConfirmDialog()
  reset_game_dialog = ShowConfirmDialog('Are you sure to reset?')
  Level.OnStep = OnQueryResetGame
end

function CloseResetConfirmDialog()
  Good.KillObj(reset_game_dialog)
  reset_game_dialog = nil
  Level.OnStep = OnStepGame
end

function GetPlayTimeStr(play_time)
  local sec = play_time % 60
  local minute = math.floor(play_time / 60) % 60
  local hour = math.floor(play_time / 3600) % 24
  local day = math.floor(play_time / 86400)
  if (0 < day) then
    return string.format('play time:%d:%d:%d:%d', day, hour, minute, sec)
  else
    return string.format('play time:%d:%d:%d', hour, minute, sec)
  end
end

function GenInfoMsg(gained_coin, spent_coin, sent_coin_eater, chest_level, replay_count, play_time, max_iq)
  if (nil == info_dialog) then
    return
  end
  if (nil ~= info_msg_root) then
    Good.KillObj(info_msg_root)
    info_msg_root = nil
  end
  info_msg_root = Good.GenDummy(info_dialog)

  local function offset_y_closure() local n = 0 return function() local y = 60 + n * (8 + DIALOG_FONT_SIZE) n = n + 1 return y end end
  local offset_x, offset_y = 40, offset_y_closure()

  local labels = {
    string.format('gained coin:%d', gained_coin),
    string.format('spent coin:%d', spent_coin),
    string.format('sent coin eater:%d', sent_coin_eater),
    string.format('chest level:%d', chest_level),
    string.format('play count:%d', replay_count),
    GetPlayTimeStr(play_time),
    string.format('max iq:%d', max_iq)
  }

  for i = 1, #labels do
    local s = labels[i]
    local o = Good.GenTextObj(info_msg_root, s, DIALOG_FONT_SIZE)
    SetTextObjColor(o, 0xff000000)
    Good.SetPos(o, offset_x, offset_y())
  end
end

function UpdateInfoMsg()
  Good.FireUserIntEvent(CMD_GEN_INFO_MSG)
end

function ShowInfoDialog()
  info_dialog = GenDialogObj(OPTION_DIALOG_W, OPTION_DIALOG_H)
  local cancel = Good.GenObj(info_dialog, 61)
  Good.SetPos(cancel, OPTION_DIALOG_W - 24, - 16)

  local o = Good.GenObj(info_dialog, 73)
  Good.SetPos(o, 15, 15)

  UpdateInfoMsg()

  Level.OnStep = OnStepInfo
end

function CloseInfoDialog()
  Good.KillObj(info_dialog)
  info_dialog = nil
  info_msg_root = nil
  Level.OnStep = OnStepGame
end

function PlaySound(id)
  if (1 == snd_switch) then
    Sound.PlaySound(id)
  end
end

Level = {}

Level.OnCreate = function(param)
  Good.FireUserIntEvent(CMD_INIT_GAME)
  UpdateAutoSwitchState()
end

function OnStepInit(param)
  if (not Input.IsKeyPushed(Input.LBUTTON)) then
    return
  end

  local x, y = Input.GetMousePos()
  local map = Good.PickObj(x, y)
  if ('map' == Good.GetName(map)) then
    local mapx, mapy = Good.GetPos(map)
    local tilex, tiley = Resource.GetTileSize(24)
    local col = math.floor((x - mapx) / tilex)
    local row = math.floor((y - mapy) / tiley)
    if (2 <= col and 12 > col and 2 <= row and 12 > row) then
      local portal = Good.GenObj(map, 11)
      Good.SetPos(portal, tilex * col, tiley * row)
      Good.FireUserIntEvent(CMD_SET_START_POINT + 256 * col + 65536 * row)
      InitTrain()
      Level.OnStep = OnStepInitTrain
    else
      local o = GenColorObj(map, 10 * tilex, 10 * tiley, 0xffff0000, 'AnimInvalidPortalPos')
      Good.SetPos(o, 2 * tilex, 2 * tiley)
    end
    return
  end

  ToggleAutoSwitch(x, y)
end

function OnStepInitTrain(param)
  -- NOP.
end

function OnStepGame(param)
  UpdateAutoGainCounter()

  if (Input.IsKeyPushed(Input.ESCAPE)) then
    ShowQuitConfirmDialog()
    PlaySound(71)
    return
  end

  if (not Input.IsKeyPushed(Input.LBUTTON)) then
    return
  end

  local x, y = Input.GetMousePos()
  if (ToggleAutoSwitch(x, y)) then
    return
  end

  local x1,y1 = Good.GetPos(28)         -- Add coin eater.
  if (PtInRect(x, y, x1 - 8, y1 - 8, x1 + 40, y1 + 40)) then
    Good.FireUserIntEvent(CMD_INC_MAX_COIN_EATER)
    PlaySound(71)
    return
  end

  local x2,y2 = Good.GetPos(29)         -- Add stability.
  if (PtInRect(x, y, x2 - 8, y2 - 8, x2 + 40, y2 + 40)) then
    Good.FireUserIntEvent(CMD_INC_STABILITY)
    PlaySound(71)
    return
  end

  local x3,y3 = Good.GetPos(64)         -- Option.
  if (PtInRect(x, y, x3 - 8, y3 - 8, x3 + 40, y3 + 40)) then
    ShowOptionDialog()
    PlaySound(71)
    return
  end

  Good.FireUserIntEvent(CMD_PUT_COIN_EATER)
end

function OnQueryQuitGame(param)
  UpdateAutoGainCounter()

  if (Input.IsKeyPushed(Input.ESCAPE)) then
    CloseQuitConfirmDialog()
    PlaySound(71)
    return
  end

  if (not Input.IsKeyPushed(Input.LBUTTON)) then
    return
  end

  local mx, my = Input.GetMousePos()
  local x, y = Good.GetPos(quit_game_dialog) -- Close button.
  if (PtInRect(mx, my, x + DIALOG_W - 32, y - 32, x + DIALOG_W + 32, y + 32)) then
    CloseQuitConfirmDialog()
    PlaySound(71)
    return
  end

  local bx, by = x + (DIALOG_W - 100)/2, y + 74 -- Yes button.
  if (PtInRect(mx, my, bx, by, bx + 100, by + 32)) then
    Good.FireUserIntEvent(CMD_QUIT_GAME)
    Good.Exit()
    PlaySound(71)
    return
  end
end

function OnStepOption(param)
  UpdateAutoGainCounter()

  if (Input.IsKeyPushed(Input.ESCAPE)) then
    CloseOptionDialog()
    PlaySound(71)
    return
  end

  if (not Input.IsKeyPushed(Input.LBUTTON)) then
    return
  end

  local mx, my = Input.GetMousePos()
  local x, y = Good.GetPos(option_dialog) -- Close button.
  if (PtInRect(mx, my, x + OPTION_DIALOG_W - 32, y - 32, x + OPTION_DIALOG_W + 32, y + 32)) then
    CloseOptionDialog()
    PlaySound(71)
    return
  end

  local x1,y1 = Good.GetPos(option_snd) -- Toggle snd button.
  if (PtInRect(mx, my, x + x1, y + y1, x + x1 + 140, y + y1 + 40)) then
    ToggleSndSwitch()
    return
  end

  local x2,y2 = Good.GetPos(option_bgm) -- Toggle bgm button.
  if (PtInRect(mx, my, x + x2, y + y2, x + x2 + 140, y + y2 + 40)) then
    ToggleBgmSwitch()
    return
  end

  local x3,y3 = Good.GetPos(option_reset) -- Reset button.
  if (PtInRect(mx, my, x + x3, y + y3, x + x3 + 100, y + y3 + 34)) then
    CloseOptionDialog()
    ShowResetConfirmDialog()
    PlaySound(71)
    return
  end

  local x4,y4 = Good.GetPos(option_info) -- Info button.
  if (PtInRect(mx, my, x + x4 - 8, y + y4 - 8, x + x4 + 40, y + y4 + 40)) then
    CloseOptionDialog()
    ShowInfoDialog()
    PlaySound(71)
    return
  end
end

function OnStepInfo(param)
  UpdateAutoGainCounter()

  if (Input.IsKeyPushed(Input.ESCAPE)) then
    CloseInfoDialog()
    PlaySound(71)
    return
  end

  if (not Input.IsKeyPushed(Input.LBUTTON)) then
    return
  end

  local mx, my = Input.GetMousePos()
  local x, y = Good.GetPos(info_dialog) -- Close button.
  if (PtInRect(mx, my, x + OPTION_DIALOG_W - 32, y - 32, x + OPTION_DIALOG_W + 32, y + 32)) then
    CloseInfoDialog()
    PlaySound(71)
    return
  end
end

function OnQueryResetGame(param)
  UpdateAutoGainCounter()

  if (Input.IsKeyPushed(Input.ESCAPE)) then
    CloseResetConfirmDialog()
    PlaySound(71)
    return
  end

  if (not Input.IsKeyPushed(Input.LBUTTON)) then
    return
  end

  local mx, my = Input.GetMousePos()    -- Close button.
  local x, y = Good.GetPos(reset_game_dialog)
  if (PtInRect(mx, my, x + DIALOG_W - 32, y - 32, x + DIALOG_W + 32, y + 32)) then
    CloseResetConfirmDialog()
    PlaySound(71)
    return
  end

  local bx, by = x + (DIALOG_W - 100)/2, y + 74 -- Yes button.
  if (PtInRect(mx, my, bx, by, bx + 100, by + 32)) then
    Good.FireUserIntEvent(CMD_RESET_GAME)
    PlaySound(71)
    ResetGlobal()
    Good.GenObj(-1, 0)
    return
  end
end

Level.OnStep = OnStepInit
