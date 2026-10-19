-- Debug scenarios: set up a situation, trigger script code, take screenshots.
--
--   SCENARIO=tests/scenarios/NAME.txt tools/mgba/headless.sh tools/mgba/scenario.lua
--
-- A scenario is a text file, one step per line (# comments, numbers decimal
-- or 0x hex). Output goes to build/centaur/scenarios/NAME/ (screenshots,
-- captures, log.txt). Steps:
--
--   from field            start in the field of the test save (default:
--                         replays tests/save_menus_field.txt first)
--   from none             start wherever the save boots (no replay)
--   from bookmark NAME    load a Centaur bookmark (build/centaur/bookmarks)
--   warp SCENE [ENTRANCE] go to a scene the way a door does (player placed)
--   wait N                run N frames
--   call PROC [ARGS...]   run a script procedure (Script_Call) and wait for it
--   set REGION OFF TYPE[.BIT] VALUE   write a script variable (region r700,
--                         r708 or gs; TYPE as gcx.py prints it: 1 s16, 2 byte,
--                         4 one bit, 9 word), e.g. `set r708 0x3c 1 120`
--   goto X Z [R] [MAX]    walk the player toward world (X, Z) until within R
--                         (default 96) or MAX frames (default 600) pass
--   swing N               N sword swings (B), turning a quarter each time
--   until REGION OFF TYPE VALUE [MAX] [TAP]   wait (at most MAX frames, default
--                         3600) until a script variable equals VALUE; with TAP,
--                         press A every TAP frames meanwhile (to page through dialogue)
--   fight REGION OFF TYPE X Z [X Z ...]   until the variable is 0: walk to each
--                         point in turn and swing (a scripted player's battle)
--   keylog PATH           from now on, write the keys held each frame to
--                         PATH (OUTDIR-relative) as harness `hold` lines, so
--                         the run replays (and records with sound) in the
--                         harness from a `state` taken just before
--   get REGION OFF TYPE[.BIT]   log a script variable's value (as `set` names it)
--   pos X Y Z             move the player (world units, 256 per cell)
--   keys KEYS N           hold keys for N frames (A+UP, START, ...), then release
--   shot NAME             screenshot -> NAME.png
--   state NAME            write an mGBA savestate -> NAME.ss (for tools/emu/harness
--                         `loadstate`, e.g. to record video with audio)
--   record N PREFIX [TAP] a screenshot every frame for N frames ->
--                         PREFIX_00000.png ... (for a video: ffmpeg -framerate
--                         59.7275 -i PREFIX_%05d.png); with TAP, press A for 2
--                         frames every TAP frames (to advance dialogue)
--   capture NAME          a full Centaur capture (palettes, VRAM, OAM, screen)
--   log TEXT              write TEXT to the log
--   quit                  stop (also at the end of the file)
--
-- PALETTE_LOG=1 also logs every large-sprite palette selection to
-- OUTDIR/sprite_pools.tsv (scene, hash, pool entry, caller) and every sprite
-- load to OUTDIR/sprite_loads.tsv (scene, hash, caller).
--
-- Spawning an actor is a `call` of its spawner procedure with arguments
-- (gcl/names/proc.csv names them: spawn_slime_by_color, place_bat, ...).
local SRC = (debug.getinfo(1, "S").source or ""):gsub("^@", "")
local ROOT = SRC:match("^(.*)/tools/mgba/[^/]*$") or "."
dofile(ROOT .. "/tools/mgba/centaur.lua")

local path = os.getenv("SCENARIO") or ""
if path:sub(1, 1) ~= "/" then path = ROOT .. "/" .. path end
local name = path:match("([^/]+)%.%w+$") or "scenario"
local OUTDIR = ROOT .. "/build/centaur/scenarios/" .. name
os.execute('mkdir -p "' .. OUTDIR .. '"')
local logf = io.open(OUTDIR .. "/log.txt", "w")
local function say(s) console:log("scenario: " .. s); if logf then logf:write(s, "\n"); logf:flush() end end

-- PALETTE_LOG=1: log every large-sprite palette selection (scene, hash, pool
-- entry, caller) to OUTDIR/sprite_pools.tsv, as tools/mgba/palette_survey.lua
-- does; tools/assets/bigpalettes.py folds such logs into data/sprite_palettes.csv
if os.getenv("PALETTE_LOG") then
  local pf = io.open(OUTDIR .. "/sprite_pools.tsv", "w")
  pf:write("scene\thash\tpool\tcaller\n")
  local anim_hash, seen = {}, {}
  local lf, lseen = io.open(OUTDIR .. "/sprite_loads.tsv", "w"), {}
  lf:write("scene\thash\tcaller\n")
  emu:setBreakpoint(function()
    local h = emu:readRegister("r1") & 0xFFFF
    anim_hash[emu:readRegister("r0")] = h
    local k = string.format("%d\t%04X\t%08X", scene() or -1, h, (emu:readRegister("lr") - 1) & ~1)
    if not lseen[k] then lseen[k] = true; lf:write(k, "\n"); lf:flush() end
  end, 0x082151E4)
  emu:setBreakpoint(function()
    local a, p, lr = emu:readRegister("r0"), emu:readRegister("r1") & 0xFFFF, emu:readRegister("lr")
    local h = anim_hash[a]
    local k = string.format("%d\t%s\t%d\t%08X", scene() or -1, h and string.format("%04X", h) or "?", p, (lr - 1) & ~1)
    if not seen[k] then seen[k] = true; pf:write(k, "\n"); pf:flush() end
  end, 0x08215284)
end

local steps = {}
local f = io.open(path, "r")
if not f then say("no scenario file " .. path); os.exit(1) end
for raw in f:lines() do
  local ln = raw:gsub("#.*$", ""):gsub("^%s+", ""):gsub("%s+$", "")
  if ln ~= "" then
    local w = {}
    for x in ln:gmatch("%S+") do w[#w + 1] = x end
    steps[#steps + 1] = { op = w[1], args = { table.unpack(w, 2) }, line = ln }
  end
end
f:close()

local function num(x) return x and (x:match("^0x") and tonumber(x:sub(3), 16) or tonumber(x)) end
local KEY = { A = 0, B = 1, SELECT = 2, START = 3, RIGHT = 4, LEFT = 5, UP = 6, DOWN = 7, R = 8, L = 9 }
local function keymask(s)
  local m = 0
  for k in s:gmatch("[^+]+") do if KEY[k:upper()] then m = m | (1 << KEY[k:upper()]) end end
  return m
end

local i, wait_until, busy, rec = 0, 0, false, nil
local held, keylog, steer = 0, nil, nil
local setKeys = function(m) held = m; emu:setKeys(m) end
local function player_xz()
  local gs = emu:read32(0x02000710)
  if gs < 0x02000000 or gs >= 0x02040000 then return nil end
  local function s16(a) local v = emu:read16(a); return v >= 0x8000 and v - 0x10000 or v end
  return s16(gs + 0x30), s16(gs + 0x34)
end
local KEYNAME = { [0] = "A", "B", "SELECT", "START", "RIGHT", "LEFT", "UP", "DOWN", "R", "L" }
local function keytext(m)
  local t = {}
  for b = 0, 9 do if m & (1 << b) ~= 0 then t[#t + 1] = KEYNAME[b] end end
  return #t > 0 and table.concat(t, "+") or nil
end
local logrun = { m = -1, n = 0 }
local function log_frame()
  if not keylog then return end
  if held == logrun.m then logrun.n = logrun.n + 1; return end
  if logrun.n > 0 then
    local k = keytext(logrun.m)
    keylog:write(k and string.format("hold %s %d\n", k, logrun.n) or string.format("wait %d\n", logrun.n))
  end
  logrun.m, logrun.n = held, 1
end
local function done() say("done"); if logf then logf:close() end; if keylog then held = -2; log_frame(); keylog:close() end; os.exit(0) end
local function next_step()
  i = i + 1
  local s = steps[i]
  if not s then return done() end
  local a, op = s.args, s.op
  say(string.format("[%d] %s", emu:currentFrame(), s.line))
  if op == "warp" then warp(num(a[1]), num(a[2]))
  elseif op == "wait" then wait_until = emu:currentFrame() + (num(a[1]) or 1)
  elseif op == "call" then
    local args = {}
    for k = 2, #a do args[#args + 1] = num(a[k]) end
    busy = true
    callproc(num(a[1]), args, function() busy = false end)
  elseif op == "set" then
    local t, b = a[3]:match("^(%d+)%.?(%d*)$")
    setvar(a[1], num(a[2]), tonumber(t), b ~= "" and tonumber(b) or nil, num(a[4]))
  elseif op == "goto" then
    steer = { x = num(a[1]), z = num(a[2]), r = num(a[3]) or 96, until_ = emu:currentFrame() + (num(a[4]) or 600) }
  elseif op == "swing" then
    local dirs, seq = { "LEFT", "UP", "RIGHT", "DOWN" }, {}
    for k = 1, (num(a[1]) or 4) do
      seq[#seq + 1] = { keymask(dirs[(k - 1) % 4 + 1]), 3 }
      seq[#seq + 1] = { keymask("B"), 6 }
      seq[#seq + 1] = { 0, 12 }
    end
    steer = { seq = seq, k = 1, t = 0 }
  elseif op == "until" then
    local t, b = a[3]:match("^(%d+)%.?(%d*)$")
    steer = { var = { a[1], num(a[2]), tonumber(t), b ~= "" and tonumber(b) or nil }, value = num(a[4]),
              until_ = emu:currentFrame() + (num(a[5]) or 3600), tap = num(a[6]), k = 0 }
  elseif op == "fight" then
    local t, b = a[3]:match("^(%d+)%.?(%d*)$")
    local pts = {}
    for k = 4, #a - 1, 2 do pts[#pts + 1] = { num(a[k]), num(a[k + 1]) } end
    steer = { fight = pts, var = { a[1], num(a[2]), tonumber(t), b ~= "" and tonumber(b) or nil }, value = 0, k = 0,
              until_ = emu:currentFrame() + 7200 }
  elseif op == "keylog" then
    keylog = io.open(OUTDIR .. "/" .. a[1], "w")
  elseif op == "get" then
    local t, b = a[3]:match("^(%d+)%.?(%d*)$")
    say(string.format("get %s %s %s = %s", a[1], a[2], a[3], tostring(getvar(a[1], num(a[2]), tonumber(t), b ~= "" and tonumber(b) or nil))))
  elseif op == "pos" then
    -- the player object (0x02000580) keeps its position at +0x30 and copies
    -- at +0xAC and +0x10C (s16 x, y, z); GameState +0x30 mirrors it
    local x, y, z = num(a[1]) & 0xFFFF, num(a[2]) & 0xFFFF, num(a[3]) & 0xFFFF
    local p = emu:read32(0x02000580)
    if p >= 0x02000000 and p < 0x02040000 then
      for _, o in ipairs({ 0x30, 0xAC, 0x10C }) do emu:write16(p + o, x); emu:write16(p + o + 2, y); emu:write16(p + o + 4, z) end
    else say("no player object") end
    local gs = emu:read32(0x02000710)
    if gs >= 0x02000000 and gs < 0x02040000 then emu:write16(gs + 0x30, x); emu:write16(gs + 0x32, y); emu:write16(gs + 0x34, z) end
  elseif op == "keys" then
    setKeys(keymask(a[1])); wait_until = emu:currentFrame() + (num(a[2]) or 2); s.release = true
  elseif op == "shot" then pcall(function() emu:screenshot(OUTDIR .. "/" .. a[1] .. ".png") end)
  elseif op == "state" then
    local ok, err = pcall(function() return emu:saveStateFile(OUTDIR .. "/" .. a[1] .. ".ss", 0xFF) end)
    if not ok then say("state failed: " .. tostring(err)) end
  elseif op == "record" then
    rec = { n = num(a[1]), prefix = a[2], tap = num(a[3]), k = 0 }
  elseif op == "capture" then capture(name .. "_" .. a[1])
  elseif op == "log" then say(table.concat(a, " "))
  elseif op == "quit" then return done()
  elseif op == "from" then -- handled before the first frame
  else say("unknown step: " .. s.line) end
end

local started = false
local function start()
  started = true
  callbacks:add("frame", function()
    local s = steps[i]
    if rec then
      pcall(function() emu:screenshot(string.format("%s/%s_%05d.png", OUTDIR, rec.prefix, rec.k)) end)
      if rec.tap then setKeys((rec.k % rec.tap) < 2 and 1 or 0) end
      rec.k = rec.k + 1
      if rec.k >= rec.n then setKeys(0); rec = nil end
      return
    end
    log_frame()
    -- a fight stops the moment its variable reaches 0, even mid-swing: the
    -- next dialogue opens a few frames later and B would page through it
    if steer and steer.var and steer.sub and getvar(table.unpack(steer.var)) == steer.value then steer.sub = nil end
    if steer and steer.var and not steer.sub then
      local v = getvar(table.unpack(steer.var))
      if v == steer.value or emu:currentFrame() >= steer.until_ then
        say(string.format("[%d] %s -> %s", emu:currentFrame(), steer.fight and "fight" or "until", tostring(v)))
        steer = nil; setKeys(0)
      elseif steer.fight then
        steer.k = steer.k % #steer.fight + 1
        local p = steer.fight[steer.k]
        local dirs, seq = { "LEFT", "UP", "RIGHT", "DOWN" }, {}
        for k = 1, 4 do
          seq[#seq + 1] = { keymask(dirs[k]), 3 }; seq[#seq + 1] = { keymask("B"), 6 }; seq[#seq + 1] = { 0, 12 }
        end
        steer.sub = { x = p[1], z = p[2], r = 140, until_ = emu:currentFrame() + 240, then_ = seq }
      else
        steer.k = steer.k + 1
        setKeys((steer.tap and steer.k % steer.tap < 4) and keymask("A") or 0)
        return
      end
    end
    if steer and steer.sub then
      -- a fight's leg: walk, then the swings, then back to the fight
      local parent, sub = steer, steer.sub
      if sub.then_ and not sub.walked then
        local px, pz = player_xz()
        local dx, dz = sub.x - (px or sub.x), sub.z - (pz or sub.z)
        if not px or dx * dx + dz * dz <= sub.r * sub.r or emu:currentFrame() >= sub.until_ then
          sub.walked = true; sub.k, sub.t = 1, 0
        else
          local sx, sy, m = dx - dz, dx + dz, 0
          if sx > 40 then m = m | keymask("RIGHT") elseif sx < -40 then m = m | keymask("LEFT") end
          if sy > 40 then m = m | keymask("DOWN") elseif sy < -40 then m = m | keymask("UP") end
          setKeys(m)
          return
        end
      end
      local e = sub.then_[sub.k]
      if not e then parent.sub = nil; setKeys(0); return end
      setKeys(e[1]); sub.t = sub.t + 1
      if sub.t >= e[2] then sub.k = sub.k + 1; sub.t = 0 end
      return
    end
    if steer then
      if steer.seq then
        local e = steer.seq[steer.k]
        if not e then steer = nil; setKeys(0)
        else
          setKeys(e[1]); steer.t = steer.t + 1
          if steer.t >= e[2] then steer.k = steer.k + 1; steer.t = 0 end
        end
      else
        local px, pz = player_xz()
        local dx, dz = steer.x - (px or steer.x), steer.z - (pz or steer.z)
        if not px or dx * dx + dz * dz <= steer.r * steer.r or emu:currentFrame() >= steer.until_ then
          steer = nil; setKeys(0)
        else
          -- screen axes: right = +x -z, down = +x +z
          local sx, sy, m = dx - dz, dx + dz, 0
          if sx > 40 then m = m | keymask("RIGHT") elseif sx < -40 then m = m | keymask("LEFT") end
          if sy > 40 then m = m | keymask("DOWN") elseif sy < -40 then m = m | keymask("UP") end
          setKeys(m)
        end
      end
      if steer then return end
    end
    if busy or emu:currentFrame() < wait_until then return end
    if s and s.release then setKeys(0); s.release = nil end
    next_step()
  end)
end

local first = steps[1]
if first and first.op == "from" then
  i = 1
  if first.args[1] == "none" then start()
  elseif first.args[1] == "bookmark" then go(first.args[2]); wait_until = emu:currentFrame() + 3; start()
  else replay(ROOT .. "/tests/save_menus_field.txt", start) end
else
  replay(ROOT .. "/tests/save_menus_field.txt", start)
end
