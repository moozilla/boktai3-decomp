-- The game side of the Studio tour's Centaur stop (showcase v3, part 2).
--
--   LIVE_STATE=state.ss LIVE_OUT=dir [LIVE_FRAMES=N] tools/mgba/headless.sh tools/showcase/v3/live_tour.lua ROM
--
-- Runs Centaur (linked to the Studio, which follows the game on its Live
-- page: the state, the screen inspector, the map) with Django walking
-- around the room, at the GBA's own frame rate (the Studio's recording runs
-- in real time), and saves every frame as LIVE_OUT/g_NNNNN.png for the edit.
local SRC = (debug.getinfo(1, "S").source or ""):gsub("^@", "")
local ROOT = SRC:match("^(.*)/tools/showcase/v3/[^/]*$") or "."
CENTAUR_HEADLESS = true
dofile(ROOT .. "/tools/mgba/centaur.lua")

local OUT = os.getenv("LIVE_OUT")
local FRAMES = tonumber(os.getenv("LIVE_FRAMES") or "2400")
local f = io.open(os.getenv("LIVE_STATE"), "rb")
local data = f:read("a"); f:close()
emu:loadStateBuffer(data)

local PRE = 260          -- frames before filming (the warp and its fade-in)
local RIGHT, LEFT, UP, DOWN = 16, 32, 64, 128
local WALK = { {DOWN, 50}, {0, 40}, {LEFT, 60}, {0, 30}, {UP, 50}, {0, 40}, {RIGHT, 60}, {0, 50} }
local n = 0
local function keys_at(i)
  local t = i % 380
  for _, w in ipairs(WALK) do
    if t < w[2] then return w[1] end
    t = t - w[2]
  end
  return 0
end
callbacks:add("keysRead", function() emu:setKeys(n < PRE and 0 or keys_at(n)) end)

-- warp into the room first (unthrottled, unsaved): the stage's loading is
-- what tells Centaur (and the Studio) its map, tileset and palette
local t0 = nil
callbacks:add("frame", function()
  if n == 5 then warp(2712) end
  if n < PRE then n = n + 1; return end
  if not t0 then t0 = os.clock() end
  local i = n - PRE
  emu:screenshot(string.format("%s/g_%05d.png", OUT, i))
  n = n + 1
  if i >= FRAMES then console:log("live_tour: done"); os.exit(0) end
  local target = t0 + i / 59.7275       -- the GBA's frame rate, so the Studio sees it in real time
  while os.clock() < target do end
end)
