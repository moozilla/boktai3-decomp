-- The game side of the Studio's live-edit demo (showcase v3, part 2).
--
--   LIVE_STATE=state.ss LIVE_OUT=dir tools/mgba/headless.sh tools/showcase/v3/live_demo.lua ROM
--
-- Runs Centaur (linked to the studio, which forwards text saves and palette
-- edits as settext/setpool) and records frames as PNGs in LIVE_OUT:
--   a_NNNNN.png  Django reads the hint panel (the original text)
--   c_NNNNN.png  after the studio's text edit arrives: he reads it again
--   dK_NNNNN.png after palette edit K arrives: a second of the recolored Django
-- The game waits (unrecorded) for each edit; the edit lines the pieces up
-- with the studio's recording (tour.py). Stops after four palette edits or
-- MAX_WAIT frames of waiting.
local SRC = (debug.getinfo(1, "S").source or ""):gsub("^@", "")
local ROOT = SRC:match("^(.*)/tools/showcase/v3/[^/]*$") or "."
CENTAUR_HEADLESS = true
dofile(ROOT .. "/tools/mgba/centaur.lua")

local OUT = os.getenv("LIVE_OUT")
local f = io.open(os.getenv("LIVE_STATE"), "rb")
local data = f:read("a"); f:close()
emu:loadStateBuffer(data)
emu:write16(emu:read32(0x02000710) + 0x12, 0)    -- the fastest message speed (GameState+0x12)

local MAX_WAIT = 60 * 60 * 60 * 24   -- the game runs unthrottled while it waits
local PAGE = 150                    -- frames between A presses while reading
local phase, n, waited, pools = "a", 0, 0, 0
local press_until = -1
local function log(s) console:log("live_demo: " .. s) end

callbacks:add("keysRead", function()   -- setKeys persists: release A between presses
  if phase == "a" or phase == "c" then emu:setKeys(n < press_until and 1 or 0) end
end)

local text_seen, pool_seen = LIVE_NEXT, 0
local orig_setpool = setpool
setpool = function(p, c) orig_setpool(p, c); pool_seen = pool_seen + 1 end
COMMANDS.setpool = function(c) setpool(c.pool, c.colors) end

local function read_panel(prefix)
  -- tap A at 10 and every PAGE frames, five times: the message's pages and its close
  if n % PAGE == 10 and n < PAGE * 5 then press_until = n + 3 end
  emu:screenshot(string.format("%s/%s_%05d.png", OUT, prefix, n))
  n = n + 1
  return n >= PAGE * 5 + 20
end

callbacks:add("frame", function()
  if phase == "a" then
    if read_panel("a") then phase, n = "wait_text", 0; log("read once; waiting for the text edit") end
  elseif phase == "wait_text" then
    waited = waited + 1
    if LIVE_NEXT ~= text_seen then phase, n = "c", 0; log("text edit arrived") end
  elseif phase == "c" then
    if read_panel("c") then phase, n = "wait_pool", 0; log("read again; waiting for palette edits") end
  elseif phase == "wait_pool" then
    waited = waited + 1
    if pool_seen > pools then pools = pools + 1; phase, n = "d", 0; log("palette edit " .. pools) end
  elseif phase == "d" then
    emu:screenshot(string.format("%s/d%d_%05d.png", OUT, pools, n))
    n = n + 1
    if n >= 60 then
      if pools >= 4 then log("done"); os.exit(0) end
      phase = "wait_pool"
    end
  end
  if waited > MAX_WAIT then log("gave up waiting"); os.exit(1) end
end)
