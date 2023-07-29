local PROGRESS_BAR_W = 80
local PROGRESS_BAR_H = 3
local MSG_OFFSET_X = 42
local MSG_FONT_SIZE = 22
local NEXT_COST_FONT_SIZE = 16
local NEXT_COST_OFFSET_X = 90
local NEXT_COST_OFFSET_Y = 25
local MAX_COL, MAX_ROW = 14, 14
local INIT_PARAM_VALUE = -99999

local TotalCoinObjId = nil
local CoinEaterCountObjId = nil
local ScoreObjId = nil
local StabilityObjId = nil
local UnlockMapObjId = nil
local NextWalkerObjId = nil

local UpdateTotalCoinMsg_p1
local UpdateTotalCoinMsg_p2
local UpdateTotalCoinMsg_p3

local UpdateCoinEaterMsg_p1
local UpdateCoinEaterMsg_p2
local UpdateCoinEaterMsg_p3

local UpdateScoreMsg_p1
local UpdateScoreMsg_p2

local UpdateStabilityMsg_p1
local UpdateStabilityMsg_p2

local UpdateUnlockNextMapMsg_p1
local UpdateNextWalkerMsg_p1

local idMapTex = nil

function InitHelper()
  TotalCoinObjId = nil
  CoinEaterCountObjId = nil
  ScoreObjId = nil
  StabilityObjId = nil
  UnlockMapObjId = nil
  NextWalkerObjId = nil
  UpdateTotalCoinMsg_p1 = INIT_PARAM_VALUE
  UpdateTotalCoinMsg_p2 = INIT_PARAM_VALUE
  UpdateTotalCoinMsg_p3 = INIT_PARAM_VALUE
  UpdateCoinEaterMsg_p1 = INIT_PARAM_VALUE
  UpdateCoinEaterMsg_p2 = INIT_PARAM_VALUE
  UpdateCoinEaterMsg_p3 = INIT_PARAM_VALUE
  UpdateScoreMsg_p1 = INIT_PARAM_VALUE
  UpdateScoreMsg_p2 = INIT_PARAM_VALUE
  UpdateStabilityMsg_p1 = INIT_PARAM_VALUE
  UpdateStabilityMsg_p2 = INIT_PARAM_VALUE
  UpdateUnlockNextMapMsg_p1 = INIT_PARAM_VALUE
  UpdateNextWalkerMsg_p1 = INIT_PARAM_VALUE
end

function UpdateTotalCoinMsg(total, auto_gain_counter, counter)
  if (UpdateTotalCoinMsg_p1 == total and UpdateTotalCoinMsg_p2 == auto_gain_counter and
      UpdateTotalCoinMsg_p3 == counter) then
    return
  end
  UpdateTotalCoinMsg_p1 = total
  UpdateTotalCoinMsg_p2 = auto_gain_counter
  UpdateTotalCoinMsg_p3 = counter
  if (nil ~= TotalCoinObjId) then
    Good.KillObj(TotalCoinObjId)
    TotalCoinObjId = nil
  end
  local msg = string.format('%d', total)
  TotalCoinObjId = Good.GenTextObj(8, msg, MSG_FONT_SIZE)
  Good.SetPos(TotalCoinObjId, MSG_OFFSET_X, (32 - MSG_FONT_SIZE)/2)
  if (0 ~= auto_gain_counter) then
    local bk = GenColorObj(TotalCoinObjId, PROGRESS_BAR_W, PROGRESS_BAR_H, 0xff000000)
    Good.SetPos(bk, 0, 32)
    local progress = GenColorObj(TotalCoinObjId, PROGRESS_BAR_W * (counter / auto_gain_counter), PROGRESS_BAR_H, 0xffff0000)
    Good.SetPos(progress, 0, 32)
  end
  UpdateInfoMsg()
end

function UpdateCoinEaterMsg(count, max_count, next_cost, max_max_count)
  if (UpdateCoinEaterMsg_p1 == counter and UpdateCoinEaterMsg_p2 == sent and UpdateCoinEaterMsg_p3 == next_cost) then
    return
  end
  UpdateCoinEaterMsg_p1 = count
  UpdateCoinEaterMsg_p2 = max_count
  UpdateCoinEaterMsg_p3 = next_cost
  if (nil ~= CoinEaterCountObjId) then
    Good.KillObj(CoinEaterCountObjId)
    CoinEaterCountObjId = nil
  end
  local msg = string.format('%d/%d', count, max_count)
  CoinEaterCountObjId = Good.GenTextObj(9, msg, MSG_FONT_SIZE)
  Good.SetPos(CoinEaterCountObjId, MSG_OFFSET_X, (32 - MSG_FONT_SIZE)/2)
  local next_msg
  if (max_max_count ~= max_count) then
    next_msg = string.format('next %d', next_cost)
  else
    next_msg = 'max'
    Good.SetVisible(28, Good.INVISIBLE)
  end
  local next_msg_id = Good.GenTextObj(CoinEaterCountObjId, next_msg, NEXT_COST_FONT_SIZE)
  Good.SetPos(next_msg_id, NEXT_COST_OFFSET_X, NEXT_COST_OFFSET_Y)
  UpdateInfoMsg()
end

function UpdateScoreMsg(avg, best)
  if (UpdateScoreMsg_p1 == avg and UpdateScoreMsg_p2 == best) then
    return
  end
  UpdateScoreMsg_p1 = avg
  UpdateScoreMsg_p2 = best
  if (nil ~= ScoreObjId) then
    Good.KillObj(ScoreObjId)
    ScoreObjId = nil
  end
  local msg = string.format('%d/%d', avg, best)
  ScoreObjId = Good.GenTextObj(17, msg, MSG_FONT_SIZE)
  Good.SetPos(ScoreObjId, MSG_OFFSET_X, (32 - MSG_FONT_SIZE)/2)
end

function UpdateStabilityMsg(val, next_cost, max_val)
  if (UpdateStabilityMsg_p1 == val and UpdateStabilityMsg_p2 == next_cost) then
    return
  end
  UpdateStabilityMsg_p1 = val
  UpdateStabilityMsg_p2 = next_cost
  if (nil ~= StabilityObjId) then
    Good.KillObj(StabilityObjId)
    StabilityObjId = nil
  end
  local msg = string.format('%d%%', val)
  StabilityObjId = Good.GenTextObj(2, msg, MSG_FONT_SIZE)
  Good.SetPos(StabilityObjId, MSG_OFFSET_X, (32 - MSG_FONT_SIZE)/2)
  local next_msg
  if (max_val ~= val) then
    next_msg = string.format('next %d', next_cost)
  else
    next_msg = 'max'
    Good.SetVisible(29, Good.INVISIBLE)
  end
  local next_msg_id = Good.GenTextObj(StabilityObjId, next_msg, NEXT_COST_FONT_SIZE)
  Good.SetPos(next_msg_id, NEXT_COST_OFFSET_X, NEXT_COST_OFFSET_Y)
end

function UpdateUnlockNextMapMsg(next_cost)
  if (UpdateUnlockNextMapMsg_p1 == next_cost) then
    return
  end
  UpdateUnlockNextMapMsg_p1 = next_cost
  if (nil ~= UnlockMapObjId) then
    Good.KillObj(UnlockMapObjId)
    UnlockMapObjId = nil
  end
  if (0 == next_cost) then
    return
  end
  local msg = string.format('unlock %d', next_cost)
  UnlockMapObjId = Good.GenTextObj(25, msg, NEXT_COST_FONT_SIZE)
  Good.SetPos(UnlockMapObjId, (32 - NEXT_COST_FONT_SIZE)/2, (32 - NEXT_COST_FONT_SIZE)/2)
end

function UpdateNextWalkerMsg(val)
  if (UpdateNextWalkerMsg_p1 == val) then
    return
  end
  UpdateNextWalkerMsg_p1 = val
  if (nil ~= NextWalkerObjId) then
    Good.KillObj(NextWalkerObjId)
    NextWalkerObjId = nil
  end
  if (-1 == val) then
    return
  end
  local msg = string.format('next %d', val)
  NextWalkerObjId = Good.GenTextObj(49, msg, NEXT_COST_FONT_SIZE)
  Good.SetPos(NextWalkerObjId, MSG_OFFSET_X, (32 - NEXT_COST_FONT_SIZE)/2)
end

function KillAnimObj(param)
  Good.KillObj(param._id)
end

GainCoin = {}

GainCoin.OnStep = function(param)
  if (nil == param.k) then
    local x, y = Good.GetPos(8)
    local mx, my = Good.GetPos(25)
    local loop1 = ArAddLoop()
    ArAddMoveTo(loop1, 'Pos', 0.5 + math.random() / 2, x - mx, y - my).ease = ArEaseOut
    ArAddCall(loop1, 'KillAnimObj', 0)
    param.k = ArAddAnimator({loop1})
  else
    ArStepAnimator(param, param.k)
  end
end

GainChestCoin = {}

GainChestCoin.OnStep = function(param)
  if (nil == param.k) then
    local x, y = Good.GetPos(8)
    local mx, my = Good.GetPos(25)
    local loop1 = ArAddLoop()
    ArAddMoveBy(loop1, 'Pos', math.random() / 3, math.random(80) - 40, math.random(80) - 40).ease = ArEaseIn
    ArAddMoveTo(loop1, 'Pos', 0.5 + math.random() / 2, x - mx, y - my).ease = ArEaseOut
    ArAddCall(loop1, 'KillAnimObj', 0)
    param.k = ArAddAnimator({loop1})
  else
    ArStepAnimator(param, param.k)
  end
end

StateIcon = {}

StateIcon.OnStep = function(param)
  if (nil == param.k) then
    local loop1 = ArAddLoop()
    ArAddCall(loop1, 'KillAnimObj', 0.15)
    param.k = ArAddAnimator({loop1})
  else
    ArStepAnimator(param, param.k)
  end
end

UnlockMap = {}

UnlockMap.OnStep = function(param)
  if (nil == param.k) then
    local duration = 1.5
    local loop1 = ArAddLoop()
    ArAddCall(loop1, 'KillAnimObj', duration)
    local loop2 = ArAddLoop()
    ArAddMoveTo(loop2, 'BgColor', duration, 0x00ffffff)
    param.k = ArAddAnimator({loop1, loop2})
  else
    ArStepAnimator(param, param.k)
  end
end

AnimInvalidPortalPos = {}

AnimInvalidPortalPos.OnStep = function(param)
  if (nil == param.k) then
    local loop1 = ArAddLoop()
    ArAddMoveTo(loop1, 'Alpha', 0.4, 0)
    ArAddCall(loop1, 'KillAnimObj', 0)
    param.k = ArAddAnimator({loop1})
  else
    ArStepAnimator(param, param.k)
  end
end

function GenLockTexObj(col, row)
  local o = GenTexObj(25, 21, 32, 32, 32, 0, 'UnlockMap')
  Good.SetPos(o, 32 * col, 32 * row)
end

function UnlockMap12()
  for col = 1, MAX_COL - 2 do
    GenLockTexObj(col, 1)
  end
  for row = 2, MAX_ROW - 2 do
    GenLockTexObj(1, row)
    GenLockTexObj(MAX_COL - 2, row)
  end
  for col = 1, MAX_COL - 2 do
    GenLockTexObj(col, MAX_ROW - 2)
  end
end

function UnlockMap14()
  for col = 0, MAX_COL - 1 do
    GenLockTexObj(col, 0)
  end
  for row = 1, MAX_ROW - 1 do
    GenLockTexObj(0, row)
    GenLockTexObj(MAX_COL - 1, row)
  end
  for col = 1, MAX_COL - 1 do
    GenLockTexObj(col, MAX_ROW - 1)
  end
end

function GenDialogObj(w, h)
  local W, H = Good.GetWindowSize()
  local dummy = Good.GenDummy(-1)
  Good.SetPos(dummy, (W - w)/2, (H - h)/2)
  local bg = GenColorObj(dummy, w, h, 0xff808080)
  local fg = GenColorObj(dummy, w - 2, h - 2, 0xffffeed0)
  Good.SetPos(fg, 1, 1)
  return dummy
end

function ResetGlobal()
  InitWalker()
  InitHelper()
end

function SetMapTex(id, idMap)
  local cx, cy = Resource.GetMapSize(idMap)
  local tx, ty = Resource.GetTileSize(idMap)
  local w, h = cx * tx, cy * ty
  local canvas = Graphics.GenCanvas(w, h)
  Graphics.DrawMap(canvas, 0, 0, idMap)
  if (nil == idMapTex) then
    idMapTex = Resource.GenTex(canvas, 'maptex')
  else
    Resource.UpdateTex(idMapTex, 0, 0, canvas, 0, 0, w, h)
  end
  Graphics.KillCanvas(canvas)
  Good.SetTexId(id, idMapTex)
end

function UpdateSwitch(IsOn, o)
  if (1 == IsOn) then
    Good.SetBgColor(o, 0xffffffff)
  else
    Good.SetBgColor(o, 0x80808080)
  end
end

function ToggleSwitch(IsOn)
  if (1 == IsOn) then
    return 0
  else
    return 1
  end
end
