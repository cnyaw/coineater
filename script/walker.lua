local CMD_SEL_COIN_EATER = 6

local W,H = Good.GetWindowSize()
local WALKER_SPEED = 12
local walker_root = nil

function InitWalker()
  walker_root = nil
end

function AnimDropDone(param)
  local x,y = Good.GetPos(32)
  local l,t,w,h = Good.GetDim(32)
  local loop1 = ArAddLoop()
  local to_x = x + math.random(w)
  local to_y = y + math.random(h)
  local cur_x, cur_y = Good.GetPos(param._id)
  local dist = math.sqrt((to_x - cur_x) * (to_x - cur_x) + (to_y - cur_y) * (to_y - cur_y))
  ArAddMoveTo(loop1, 'Pos', dist / WALKER_SPEED, to_x, to_y)
  ArAddCall(loop1, 'AnimDropDone', 5 + math.random(30))
  param.k = ArAddAnimator({loop1})
end

function InitWalkerAnim(param, init)
  local loop1 = ArAddLoop()
  if (nil == init) then
    local cur_x, cur_y = Good.GetPos(param._id)
    local x,y = Good.GetPos(32)
    local l,t,w,h = Good.GetDim(32)
    ArAddMoveTo(loop1, 'Pos', 1, cur_x, y + math.random(h)).ease = ArEaseOutBounce
    ArAddCall(loop1, 'AnimDropDone', 3)
  else
    ArAddCall(loop1, 'AnimDropDone', 3 + math.random(10))
  end
  param.k = ArAddAnimator({loop1})
end

function AnimArrange(param, x, y)
  local loop1 = ArAddLoop()
  ArAddMoveTo(loop1, 'Pos', 0.5, x, y).ease = ArEaseOutBounce
  ArAddCall(loop1, 'AnimDropDone', 5 + math.random(20))
  param.k = ArAddAnimator({loop1})
end

function AttentionGather()
  -- 13  7 1 | 2 8 14
  -- 15  9 3 | 4 10 16
  -- 17 11 5 | 6 12 18
  local x,y = Good.GetPos(32)
  x = W/2
  local offset_x, offset_y
  local idx = 1
  for i = 0, Good.GetChildCount(walker_root) - 1 do
    local o = Good.GetChild(walker_root, i)
    if (1 == (idx % 2)) then
      offset_x = -40 * (math.floor(idx / 6) + 1)
      offset_y = 32 * math.floor(((idx % 6) - 1) / 2)
    else
      offset_x = 40 * math.floor((idx - 1) / 6)
      offset_y = 32 * math.floor((((idx - 1) % 6) - 1) / 2)
    end
    AnimArrange(Good.GetParam(o), x + offset_x, y + offset_y)
    idx = idx + 1
  end
end

Walker = {}

Walker.OnStep = function(param)
  local id = param._id

  if (Input.IsKeyDown(Input.LBUTTON)) then
    if (not param.MouseDown) then
      local mx, my = Input.GetMousePos()
      local x, y = Good.GetPos(id)
      if (PtInRect(mx, my, x, y, x + 32, y + 32)) then
        local o = Good.GenObj(id, 16, 'StateIcon')
        Good.SetPos(o, 0, -16)
        param.MouseDown = true
        param.lx, param.ly = Good.GetPos(id)
        param.mx, param.my = mx, my
        Good.FireUserIntEvent(CMD_SEL_COIN_EATER + 256 * Good.GetTexId(id))
        if (51 == Good.GetTexId(id)) then -- Click on the whistle.
          AttentionGather()
          PlaySound(69)
        end
      end
    else
      local mx, my = Input.GetMousePos()
      local x = param.lx + (mx - param.mx)
      local y = param.ly + (my - param.my)
      Good.SetPos(id, x, y)
      return
    end
  else
    if (param.MouseDown) then
      local mx, my = Input.GetMousePos()
      if (32 < math.abs(mx - param.mx) or 32 < math.abs(my - param.my)) then
        InitWalkerAnim(param)
      end
      param.MouseDown = false
      return
    end
  end

  if (nil ~= param.k) then
    ArStepAnimator(param, param.k)
  end
end

function AddCoinEaterWalker(resid, next_resid)
  if (nil == walker_root) then
    walker_root = Good.GenDummy(-1)
  end
  local o = Good.GenObj(walker_root, resid, 'Walker')
  Good.SetPos(o, (W - 32)/2, -32)
  InitWalkerAnim(Good.GetParam(o))
  if (-1 ~= next_resid) then
    Good.SetTexId(49, next_resid)
    Good.SetVisible(49, Good.VISIBLE)
  else
    Good.SetVisible(49, Good.INVISIBLE)
  end
  PlaySound(72)
end

function InitCoinEaterWalker(resid, next_resid)
  if (nil == walker_root) then
    walker_root = Good.GenDummy(-1)
  end
  local x,y = Good.GetPos(32)
  local l,t,w,h = Good.GetDim(32)
  for i = 1, #resid do
    local o = Good.GenObj(walker_root, resid[i], 'Walker')
    Good.SetPos(o, math.random(W), y + math.random(h))
    InitWalkerAnim(Good.GetParam(o), true)
  end
  if (-1 ~= next_resid) then
    Good.SetTexId(49, next_resid)
    Good.SetVisible(49, Good.VISIBLE)
  else
    Good.SetVisible(49, Good.INVISIBLE)
  end
end
