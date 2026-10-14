-- Replay a harness input script (tools/emu/harness.c format) inside mGBA.
--
--   replay("tests/save_menus_field.txt")          from the repo root
--   replay(path, function() capture("x") end)     run a function when it ends
--
-- Understood: wait N, hold KEYS N, tap KEYS, shot NAME (screenshot to
-- build/centaur/), lux/mark/trace/probe/dump/watch (ignored). The game is not
-- reset: start it the way the recording did (same battery save, fresh boot).
-- Loaded by centaur.lua; also usable alone.

local SRC = (debug.getinfo(1, "S").source or ""):gsub("^@", "")
local ROOT = SRC:match("^(.*)/tools/mgba/[^/]*$") or "."
local KEYS = { A = 0, B = 1, SELECT = 2, START = 3, RIGHT = 4, LEFT = 5, UP = 6, DOWN = 7, R = 8, L = 9 }

local R = nil

local function mask(spec)
  local m = 0
  for k in spec:gmatch("[^+]+") do if KEYS[k:upper()] then m = m | (1 << KEYS[k:upper()]) end end
  return m
end

local function parse(path)
  local steps = {}
  local f = io.open(path, "r") or io.open(ROOT .. "/" .. path, "r")
  if not f then error("cannot open " .. path) end
  for raw in f:lines() do
    local ln = raw:gsub("#.*", "")
    local cmd, a, b = ln:match("^%s*(%S+)%s*(%S*)%s*(%S*)")
    if cmd == "wait" then steps[#steps + 1] = { keys = 0, n = tonumber(a) }
    elseif cmd == "hold" then steps[#steps + 1] = { keys = mask(a), n = tonumber(b) }
    elseif cmd == "tap" then steps[#steps + 1] = { keys = mask(a), n = 2 }; steps[#steps + 1] = { keys = 0, n = 8 }
    elseif cmd == "shot" then steps[#steps + 1] = { shot = a, n = 0 } end
  end
  f:close()
  return steps
end

function replay(path, done)
  R = { steps = parse(path), i = 1, left = nil, done = done }
  console:log(string.format("replaying %s (%d steps)", path, #R.steps))
end

function replay_stop() R = nil end

callbacks:add("keysRead", function()
  if not R then return end
  local s = R.steps[R.i]
  if s then emu:setKeys(s.keys or 0) end
end)

callbacks:add("frame", function()
  if not R then return end
  local s = R.steps[R.i]
  while s and s.n == 0 do
    if s.shot then pcall(function() emu:screenshot(ROOT .. "/build/centaur/" .. s.shot .. ".png") end) end
    R.i = R.i + 1; s = R.steps[R.i]
  end
  if not s then
    local done = R.done
    R = nil
    emu:setKeys(0)
    console:log("replay finished")
    if done then done() end
    return
  end
  R.left = (R.left or s.n) - 1
  if R.left <= 0 then R.i = R.i + 1; R.left = nil end
end)
