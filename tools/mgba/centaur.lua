-- Boktai 3 "centaur" script for desktop mGBA (0.10+): Tools > Scripting > File > Load script.
--
-- 1. Input recorder: every frame's buttons are written as a replay script in
--    the format of tools/emu/harness.c ("wait N" / "hold KEYS N"), so a human
--    playthrough can be replayed headlessly for coverage and shift tests.
-- 2. Debug console helpers (type these in the scripting console):
--      rec_start("name")   reset the game and start recording to name.txt
--      rec_stop()          finish the file
--      peek8/16/32(addr), poke8/16/32(addr, value)
--      watch(addr)         log every frame in which the value at addr changes
--      unwatch()
--      where()             frame counter + a few known RAM values
--    Extend the HELPERS section as we learn the game (jump to scenes, toggle
--    flags, open hidden menus, ...).
--
-- For replays to stay in sync with the harness:
--   * Settings > Game Boy Advance: set the real-time clock to a fixed time of
--     2005-10-01 07:00:00 UTC (the harness default, BOKTAI3_RTC=1128150000).
--   * Leave the solar sensor at its default, or note the level you used
--     (the harness command is `lux N`).
--   * Recording starts from a reset, with the battery save that is loaded at
--     that moment. Note which save you used. Harness: BOKTAI3_SAV=path.

local KEYS = { "A", "B", "SELECT", "START", "RIGHT", "LEFT", "UP", "DOWN", "R", "L" }

local rec = nil   -- { file, keys, count, frames }

local function keyname(mask)
  local parts = {}
  for i, name in ipairs(KEYS) do
    if mask & (1 << (i - 1)) ~= 0 then parts[#parts + 1] = name end
  end
  return table.concat(parts, "+")
end

local function flush()
  if not rec or rec.count == 0 then return end
  if rec.keys == 0 then
    rec.file:write(string.format("wait %d\n", rec.count))
  else
    rec.file:write(string.format("hold %s %d\n", keyname(rec.keys), rec.count))
  end
  rec.count = 0
end

local function on_frame()
  if rec then
    local k = emu:getKeys() & 0x3FF
    if k ~= rec.keys then
      flush()
      rec.keys = k
    end
    rec.count = rec.count + 1
    rec.frames = rec.frames + 1
    if rec.frames % 3600 == 0 then rec.file:flush() end
  end
  if WATCH then
    local v = emu:read32(WATCH.addr)
    if v ~= WATCH.last then
      console:log(string.format("[watch] frame %d  %08X: %08X -> %08X",
        emu:currentFrame(), WATCH.addr, WATCH.last, v))
      WATCH.last = v
    end
  end
end

function rec_start(name)
  if rec then rec_stop() end
  local path = (name or "playthrough") .. ".txt"
  local f = io.open(path, "w")
  if not f then console:error("cannot open " .. path); return end
  f:write("# recorded with tools/mgba/centaur.lua; replay: tools/emu/harness baserom.gba " .. path .. " OUTDIR\n")
  emu:reset()
  rec = { file = f, keys = 0, count = 0, frames = 0 }
  console:log("recording to " .. path .. " (game reset)")
end

function rec_stop()
  if not rec then return end
  flush()
  rec.file:close()
  console:log(string.format("recording stopped after %d frames", rec.frames))
  rec = nil
end

-- ---- memory helpers -------------------------------------------------------
function peek8(a) return emu:read8(a) end
function peek16(a) return emu:read16(a) end
function peek32(a) return emu:read32(a) end
function poke8(a, v) emu:write8(a, v) end
function poke16(a, v) emu:write16(a, v) end
function poke32(a, v) emu:write32(a, v) end

function watch(a) WATCH = { addr = a, last = emu:read32(a) }; console:log(string.format("watching %08X", a)) end
function unwatch() WATCH = nil end

-- ---- HELPERS: known game state (extend as the decomp names things) --------
-- Addresses come from docs/ROM_MAP.md and symbols/*.csv in the decomp repo.
local KNOWN = {
  { "save data ptr", 0x030053F8 },   -- raphaelr: pointer to global save data
  { "m4a SoundInfo", 0x030054E0 },
}

function where()
  console:log(string.format("frame %d", emu:currentFrame()))
  for _, k in ipairs(KNOWN) do
    console:log(string.format("  %-16s %08X = %08X", k[1], k[2], emu:read32(k[2])))
  end
end

callbacks:add("frame", on_frame)
console:log("centaur.lua loaded: rec_start(\"name\"), rec_stop(), peek/poke, watch(addr), where()")
