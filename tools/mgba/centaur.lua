-- Boktai 3 "centaur" script for desktop mGBA: Tools > Scripting > File > Load script,
-- or start mGBA with --script tools/mgba/centaur.lua.
--
-- Works best with an mGBA 0.11 development build (overlay drawing and scripting
-- breakpoints; see docs/CENTAUR.md for building one). On mGBA 0.10 the recorder
-- and helpers work; subtitles fall back to a "Subtitles" text pane only when the
-- breakpoints exist, so they need 0.11.
--
-- Features
--   * English subtitles over the game: every string the game fetches
--     (Text_LookupString, 0821ADD4) is shown with the fan translation.
--     Run `python3 tools/mgba/gen_centaur_data.py` first (writes build/centaur/data.lua).
--   * What is loading: resource lookups (ScriptProc_Start, 0821A520) and stage
--     tilesets (sub_08215D40), logged and shown in the HUD.
--   * Bookmarks: named savestates with screenshots (bm, go, bms) - a warp list.
--   * Studio link: connects to `python3 tools/studio/serve.py` (port 8643) to
--     stream text/resources and take commands (capture, bookmarks, keys, ...).
--     Open http://localhost:8642/live.
--   * Input recorder for the headless harness (rec_start / rec_stop).
--
-- Console commands (type in the scripting console):
--   help()                  this list
--   (in game) SELECT+L cycles subtitles, SELECT+R saves a quick bookmark
--   subs("en"|"both"|"jp"|"off")   subtitle mode (default "en")
--   subpos("top"|"bottom"), subid(true) subtitle position; show string IDs
--   hud(true|false)         status overlay
--   bm("name") / go("name") / bms()   save / load / list bookmarks
--   god(true|false)         keep life at maximum (GameState +0x28/+0x2A)
--   warp(scene), scene()    experimental: load a scene (stage loads, player does not spawn)
--   capture()               send a scene capture to the studio
--   rec_start("name"), rec_stop()     record inputs as a harness script
--   peek8/16/32(a), poke8/16/32(a,v), watch(a), unwatch(), where()
--
-- For recordings to replay in the harness: fix the real-time clock at
-- 2005-10-01 07:00:00 UTC (harness default), keep the solar sensor at its
-- default (or note the level; harness `lux N`), and note the battery save.

-- ---------------------------------------------------------------------------
-- paths and data
local SRC = (debug.getinfo(1, "S").source or ""):gsub("^@", "")
local ROOT = SRC:match("^(.*)/tools/mgba/[^/]*$") or "."
local OUT = ROOT .. "/build/centaur"
local D = { jp = {}, en = {}, types = {} }
do
  local ok, data = pcall(dofile, OUT .. "/data.lua")
  if ok and type(data) == "table" then D = data
  else console:warn("no build/centaur/data.lua: run python3 tools/mgba/gen_centaur_data.py for subtitles") end
end
os.execute('mkdir -p "' .. OUT .. '/bookmarks" "' .. OUT .. '/captures"')
do  -- replay("tests/x.txt"): harness input scripts inside mGBA
  local ok, err = pcall(dofile, ROOT .. "/tools/mgba/replay.lua")
  if not ok then console:warn("replay.lua: " .. tostring(err)) end
end

local LOGF = io.open(OUT .. "/centaur.log", "a")
local function LOG(msg)
  console:log(msg)
  if LOGF then LOGF:write(os.date("%H:%M:%S "), msg, "\n"); LOGF:flush() end
end

local HAS_BP = emu.setBreakpoint ~= nil
local HAS_CANVAS = canvas ~= nil and canvas.newLayer ~= nil

local ADDR = {
  Text_LookupString = 0x0821ADD4,
  ScriptProc_Start = 0x0821A520,
  TilesetLoad = 0x08215D40,     -- sub_08215D40(mode, ptn, count, ids, dst)
  GameState = 0x02000710,       -- pointer; +0x28 life, +0x2A lifeMax
}

local S = {
  mode = "en", hud = true, god = false, pos = "bottom", showid = false,
  text = nil,            -- { id, frame }
  texts = {},            -- recent { id, frame }
  res = {},              -- recent { type, name, frame }
  stage = {},            -- map, tileset, pal
  keyq = {},             -- queued remote input { keys, frames }
  dirty = true,
}

local function hex4(v) return string.format("%04X", v & 0xFFFF) end
local function toast(msg) S.toast = { text = msg, frame = emu:currentFrame() }; S.dirty = true end

-- ---------------------------------------------------------------------------
-- tiny JSON (enough for the studio protocol)
local function jstr(s)
  return '"' .. tostring(s):gsub('[%c"\\]', function(c)
    local m = { ['"'] = '\\"', ['\\'] = '\\\\', ['\n'] = '\\n', ['\r'] = '\\r', ['\t'] = '\\t' }
    return m[c] or string.format("\\u%04x", c:byte())
  end) .. '"'
end
local function json(v)
  local t = type(v)
  if t == "table" then
    if #v > 0 or next(v) == nil then
      local parts = {}
      for _, x in ipairs(v) do parts[#parts + 1] = json(x) end
      return "[" .. table.concat(parts, ",") .. "]"
    end
    local parts = {}
    for k, x in pairs(v) do parts[#parts + 1] = jstr(k) .. ":" .. json(x) end
    return "{" .. table.concat(parts, ",") .. "}"
  elseif t == "string" then return jstr(v)
  elseif t == "boolean" then return v and "true" or "false"
  elseif v == nil then return "null"
  else return tostring(v) end
end
local function unjson(s)
  local i = 1
  local function ws() i = s:find("[^%s]", i) or #s + 1 end
  local val
  local function str()
    local out = {}
    i = i + 1
    while i <= #s do
      local c = s:sub(i, i)
      if c == '"' then i = i + 1; return table.concat(out) end
      if c == "\\" then
        local n = s:sub(i + 1, i + 1)
        local m = { n = "\n", t = "\t", r = "\r", b = "\b", f = "\f" }
        if n == "u" then out[#out + 1] = utf8.char(tonumber(s:sub(i + 2, i + 5), 16)); i = i + 6
        else out[#out + 1] = m[n] or n; i = i + 2 end
      else out[#out + 1] = c; i = i + 1 end
    end
  end
  function val()
    ws()
    local c = s:sub(i, i)
    if c == "{" then
      local o = {}; i = i + 1; ws()
      if s:sub(i, i) == "}" then i = i + 1; return o end
      while true do
        ws(); local k = str(); ws(); i = i + 1
        o[k] = val(); ws()
        local d = s:sub(i, i); i = i + 1
        if d == "}" then return o end
      end
    elseif c == "[" then
      local a = {}; i = i + 1; ws()
      if s:sub(i, i) == "]" then i = i + 1; return a end
      while true do
        a[#a + 1] = val(); ws()
        local d = s:sub(i, i); i = i + 1
        if d == "]" then return a end
      end
    elseif c == '"' then return str()
    elseif s:sub(i, i + 3) == "true" then i = i + 4; return true
    elseif s:sub(i, i + 4) == "false" then i = i + 5; return false
    elseif s:sub(i, i + 3) == "null" then i = i + 4; return nil
    else
      local num = s:match("^-?[%d%.eE+-]+", i); i = i + #num; return tonumber(num)
    end
  end
  local ok, r = pcall(val)
  return ok and r or nil
end

-- ---------------------------------------------------------------------------
-- studio link (TCP, newline-delimited JSON)
local link = { sock = nil, retry = 0, buf = "", out = {} }
-- Messages are queued and written from the frame callback: socket I/O inside a
-- breakpoint callback crashes mGBA.
local function send(msg)
  if not link.sock then return end
  link.out[#link.out + 1] = json(msg)
  if #link.out > 500 then table.remove(link.out, 1) end
end
local function link_flush()
  if not link.sock or #link.out == 0 then return end
  local ok = link.sock:send(table.concat(link.out, "\n") .. "\n")
  link.out = {}
  if not ok then link.sock = nil end
end
local function link_poll()
  if not socket then return end
  if not link.sock then
    if emu:currentFrame() < link.retry then return end
    link.retry = emu:currentFrame() + 300
    -- Connect through the raw socket: socket.connect() registers a frame
    -- callback, and adding callbacks from inside a callback crashes mGBA.
    local s = socket.tcp()
    if s._s:connect("127.0.0.1", 8643) ~= 0 then s._s:close(); s = nil end
    if s then
      link.sock = s
      link.out = {}
      send({ t = "hello", title = emu:getGameTitle(), frame = emu:currentFrame(), bp = HAS_BP, canvas = HAS_CANVAS })
      LOG("centaur: connected to the studio (http://localhost:8642/live)")
      S.dirty = true
    end
    return
  end
  while link.sock and link.sock:hasdata() do
    local chunk = link.sock:receive(4096)
    if not chunk or #chunk == 0 then break end
    link.buf = link.buf .. chunk
  end
  while true do
    local nl = link.buf:find("\n")
    if not nl then break end
    local line = link.buf:sub(1, nl - 1)
    link.buf = link.buf:sub(nl + 1)
    local cmd = unjson(line)
    if cmd and COMMANDS[cmd.c] then
      local ok, err = pcall(COMMANDS[cmd.c], cmd)
      if not ok then send({ t = "error", c = cmd.c, error = tostring(err) }); LOG("command " .. tostring(cmd.c) .. " failed: " .. tostring(err)) end
    end
  end
end

-- ---------------------------------------------------------------------------
-- hooks
local function on_text()
  local id = emu:readRegister("r0")
  local f = emu:currentFrame()
  local last = S.texts[#S.texts]
  if last and last.id == id and f - last.frame < 4 then return end
  local e = { id = id, frame = f }
  S.texts[#S.texts + 1] = e
  if #S.texts > 40 then table.remove(S.texts, 1) end
  if D.jp[id] or D.en[id] then S.text = e; S.dirty = true end
  send({ t = "text", id = id, frame = f })
end

local function on_resource()
  local ty, name = emu:readRegister("r0") & 0xFFFF, emu:readRegister("r1") & 0xFFFF
  local f = emu:currentFrame()
  local label = D.types[ty] or hex4(ty)
  local last = S.res[#S.res]
  if last and last.type == ty and last.name == name and f - last.frame < 2 then return end
  S.res[#S.res + 1] = { type = ty, name = name, frame = f }
  if #S.res > 60 then table.remove(S.res, 1) end
  if label == "map" then S.stage.map = hex4(name); S.dirty = true
  elseif label == "pal" then S.stage.pal = hex4(name) end
  send({ t = "res", type = hex4(ty), label = label, name = hex4(name), frame = f })
end

local function on_tileset()
  local n, list = emu:readRegister("r2"), emu:readRegister("r3")
  local ids = {}
  for k = 0, math.min(n, 8) - 1 do ids[#ids + 1] = hex4(emu:read16(list + k * 2)) end
  S.stage.tileset = table.concat(ids, ",")
  S.dirty = true
  send({ t = "tileset", ids = ids, frame = emu:currentFrame() })
end

if HAS_BP then
  local ok, id = pcall(function() return emu:setBreakpoint(on_text, ADDR.Text_LookupString) end)
  if ok and id and id >= 0 then
    emu:setBreakpoint(on_resource, ADDR.ScriptProc_Start)
    emu:setBreakpoint(on_tileset, ADDR.TilesetLoad)
  else
    HAS_BP = false  -- no debugger attached (headless: set MGBA_SCRIPT_DEBUGGER=1)
  end
end

-- ---------------------------------------------------------------------------
-- overlay: subtitles + HUD
local FONTS = {
  "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
  "/Library/Fonts/Arial Unicode.ttf",
  "/System/Library/Fonts/ヒラギノ角ゴシック W4.ttc",
  "/System/Library/Fonts/Supplemental/Arial.ttf",
  "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
  "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
  "C:/Windows/Fonts/meiryo.ttc", "C:/Windows/Fonts/arial.ttf",
}
local function font_path()
  for _, p in ipairs(FONTS) do
    local f = io.open(p, "rb")
    if f then f:close(); return p end
  end
end

local ov = nil
local subbuf = nil
local function overlay()
  if ov or not HAS_CANVAS then return ov end
  local w, h = canvas:width(), canvas:height()
  if not w or w == 0 then return nil end
  local sw, sh = canvas:screenWidth(), canvas:screenHeight()
  ov = { w = w, h = h, sw = sw, sh = sh, x0 = (w - sw) // 2, y0 = (h - sh) // 2 }
  ov.band = math.max(48, sh * 2 // 5)
  ov.sub = canvas:newLayer(sw, ov.band)
  ov.sub:setPosition(ov.x0, ov.y0 + (S.pos == "top" and 0 or sh - ov.band))
  ov.subp = image.newPainter(ov.sub.image)
  ov.hudl = canvas:newLayer(sw, math.max(12, sh // 14))
  ov.hudl:setPosition(ov.x0, ov.y0)
  ov.hudp = image.newPainter(ov.hudl.image)
  local fp = font_path()
  if fp then ov.subp:loadFont(fp); ov.hudp:loadFont(fp) end
  ov.fs = math.max(7, sh / 18)
  ov.subp:setFontSize(ov.fs)
  ov.hudp:setFontSize(math.max(6, sh / 26))
  return ov
end

local function clear(p, w, h)
  p:setBlend(false); p:setFill(true); p:setFillColor(0x00000000); p:drawRectangle(0, 0, w, h); p:setBlend(true)
end

local function wrap(p, text, width)
  local out = {}
  for para in (text .. "\n"):gmatch("(.-)\n") do
    local line = ""
    for word in para:gmatch("%S+") do
      local try = line == "" and word or (line .. " " .. word)
      if line ~= "" and p:textRunMetrics(try):width() > width then out[#out + 1] = line; line = word
      else line = try end
    end
    out[#out + 1] = line
  end
  return out
end

local function subtitle_text()
  local e = S.text
  if not e or S.mode == "off" then return nil end
  local en, jp = D.en[e.id], D.jp[e.id]
  if S.mode == "jp" then return jp end
  if S.mode == "both" then return (jp or "") .. (en and ("\n" .. en) or "") end
  return en or jp
end

local function draw()
  if not S.dirty then return end
  S.dirty = false
  local o = overlay()
  local txt = subtitle_text()
  if o then
    clear(o.subp, o.sw, o.band)
    if txt and txt ~= "" then
      local lines = wrap(o.subp, txt, o.sw - 10)
      local m = o.subp:textRunMetrics("Ägy")
      local lh = math.ceil(m:height()) + 1
      local n = math.min(#lines, (o.band - 4) // lh)
      local boxh = n * lh + 4
      local top = S.pos == "top" and 0 or (o.band - boxh)
      o.subp:setFillColor(0xB8000000); o.subp:drawRectangle(0, top, o.sw, boxh)
      o.subp:setFillColor(0xFFFFFFFF)
      for k = 1, n do o.subp:drawText(lines[#lines - n + k], 5, top + 2 + (k - 1) * lh) end
      if S.showid then
        o.subp:setFillColor(0xC09FB4FF); o.subp:drawText("#" .. S.text.id, o.sw - 3, top + boxh - lh, 0x13)
      end
    end
    o.sub:update()
    clear(o.hudp, o.sw, o.hudl.image.height)
    if S.hud or S.toast then
      local parts = {}
      if rec then parts[#parts + 1] = string.format("REC %d:%02d", rec.frames // 3600, (rec.frames // 60) % 60) end
      if S.stage.map then parts[#parts + 1] = "map " .. S.stage.map .. (S.stage.tileset and (" tiles " .. S.stage.tileset) or "") end
      if S.god then parts[#parts + 1] = "GOD" end
      if link.sock then parts[#parts + 1] = "studio" end
      if S.toast and emu:currentFrame() - S.toast.frame < 150 then
        local tw = o.hudp:textRunMetrics(S.toast.text):width() + 8
        o.hudp:setFillColor(0xC0204060); o.hudp:drawRectangle(o.sw - tw, 0, tw, o.hudl.image.height)
        o.hudp:setFillColor(0xFFFFFFFF); o.hudp:drawText(S.toast.text, o.sw - 4, 1, 0x13)
      end
      if #parts > 0 then
        local s = table.concat(parts, "  ")
        o.hudp:setFillColor(0x90000000); o.hudp:drawRectangle(0, 0, o.hudp:textRunMetrics(s):width() + 8, o.hudl.image.height)
        o.hudp:setFillColor(rec and 0xFFFF6060 or 0xFFFFFFFF); o.hudp:drawText(s, 4, 1)
      end
    end
    o.hudl:update()
  elseif txt and HAS_BP and not CENTAUR_HEADLESS and not os.getenv("CENTAUR_HEADLESS") then
    if subbuf == nil then
      local ok, b = pcall(function() return console:createBuffer("Subtitles") end)
      subbuf = ok and b or false
    end
    if subbuf then subbuf:clear(); subbuf:print(txt .. "\n") end
  end
end

-- ---------------------------------------------------------------------------
-- recorder (format of tools/emu/harness.c: "wait N" / "hold KEYS N")
local KEYS = { "A", "B", "SELECT", "START", "RIGHT", "LEFT", "UP", "DOWN", "R", "L" }
rec = nil
local function keyname(mask)
  local parts = {}
  for i, name in ipairs(KEYS) do if mask & (1 << (i - 1)) ~= 0 then parts[#parts + 1] = name end end
  return table.concat(parts, "+")
end
local function flush()
  if not rec or rec.count == 0 then return end
  if rec.keys == 0 then rec.file:write(string.format("wait %d\n", rec.count))
  else rec.file:write(string.format("hold %s %d\n", keyname(rec.keys), rec.count)) end
  rec.count = 0
end
function rec_start(name)
  if rec then rec_stop() end
  local path = OUT .. "/" .. (name or "playthrough") .. ".txt"
  local f = io.open(path, "w")
  if not f then console:error("cannot open " .. path); return end
  f:write("# recorded with tools/mgba/centaur.lua on " .. os.date() .. "; replay: tools/emu/harness ROM " .. path .. " OUTDIR\n")
  emu:reset()
  rec = { file = f, keys = 0, count = 0, frames = 0, path = path }
  console:log("recording to " .. path .. " (game reset)")
  S.dirty = true
end
function rec_stop()
  if not rec then return end
  flush(); rec.file:close()
  console:log(string.format("recording stopped after %d frames: %s", rec.frames, rec.path))
  rec = nil; S.dirty = true
end

-- ---------------------------------------------------------------------------
-- bookmarks (savestates + screenshots)
local BM = OUT .. "/bookmarks/"
local function bm_index()
  local list = {}
  local f = io.open(BM .. "index.tsv", "r")
  if f then
    for ln in f:lines() do
      local name, frame, map, when = ln:match("^([^\t]*)\t([^\t]*)\t([^\t]*)\t(.*)$")
      if name then list[#list + 1] = { name = name, frame = tonumber(frame), map = map, when = when } end
    end
    f:close()
  end
  return list
end
function bm(name)
  name = (name or os.date("%H%M%S")):gsub("[^%w_%-]", "_")
  local data = emu:saveStateBuffer()
  local f = io.open(BM .. name .. ".ss", "wb"); f:write(data); f:close()
  pcall(function() emu:screenshot(BM .. name .. ".png") end)
  local list = bm_index()
  local out = {}
  for _, b in ipairs(list) do if b.name ~= name then out[#out + 1] = b end end
  out[#out + 1] = { name = name, frame = emu:currentFrame(), map = S.stage.map or "", when = os.date("%Y-%m-%d %H:%M") }
  f = io.open(BM .. "index.tsv", "w")
  for _, b in ipairs(out) do f:write(string.format("%s\t%d\t%s\t%s\n", b.name, b.frame, b.map, b.when)) end
  f:close()
  console:log("bookmark saved: " .. name); toast("bookmark " .. name)
  send({ t = "bookmarks", list = out })
end
function go(name)
  local f = io.open(BM .. name .. ".ss", "rb")
  if not f then console:error("no bookmark " .. name); return end
  local data = f:read("a"); f:close()
  if rec then console:warn("loading a bookmark while recording: the replay will not stay in sync") end
  emu:loadStateBuffer(data)
  console:log("loaded bookmark " .. name); toast("loaded " .. name)
end
function bms()
  for _, b in ipairs(bm_index()) do console:log(string.format("  %-20s map %s  %s", b.name, b.map, b.when)) end
end

-- ---------------------------------------------------------------------------
-- capture: palettes, VRAM, OAM, IO and a screenshot, for the studio
local function tohex(s) return (s:gsub(".", function(c) return string.format("%02x", c:byte()) end)) end
function capture(tag)
  local name = (tag or ("cap" .. emu:currentFrame())):gsub("[^%w_%-]", "_")
  local dir = OUT .. "/captures/" .. name
  os.execute('mkdir -p "' .. dir .. '"')
  local regions = { pal = { 0x05000000, 0x400 }, vram = { 0x06000000, 0x18000 }, oam = { 0x07000000, 0x400 },
                    io = { 0x04000000, 0x60 }, ewram = { 0x02000000, 0x40000 }, iwram = { 0x03000000, 0x8000 } }
  for k, r in pairs(regions) do
    local f = io.open(dir .. "/" .. k .. ".bin", "wb"); f:write(emu:readRange(r[1], r[2])); f:close()
  end
  pcall(function() emu:screenshot(dir .. "/screen.png") end)
  local meta = io.open(dir .. "/meta.json", "w")
  meta:write(json({ frame = emu:currentFrame(), scene = scene(), map = S.stage.map, tileset = S.stage.tileset, pal = S.stage.pal,
                    text = S.text and S.text.id or nil, time = os.date("%Y-%m-%d %H:%M:%S") }))
  meta:close()
  console:log("capture saved: " .. dir); toast("captured")
  send({ t = "capture", name = name, dir = dir })
end

-- ---------------------------------------------------------------------------
-- debug helpers
function god(on) S.god = on ~= false; S.dirty = true; console:log("god mode " .. (S.god and "on" or "off")) end
function subs(mode) S.mode = mode or "en"; S.dirty = true end
function subpos(where)
  S.pos = where == "top" and "top" or "bottom"
  if ov then ov.sub:setPosition(ov.x0, ov.y0 + (S.pos == "top" and 0 or ov.sh - ov.band)) end
  S.dirty = true
end
function subid(on) S.showid = on ~= false; S.dirty = true end
function hud(on) S.hud = on ~= false; S.dirty = true end

function peek8(a) return emu:read8(a) end
function peek16(a) return emu:read16(a) end
function peek32(a) return emu:read32(a) end
function poke8(a, v) emu:write8(a, v) end
function poke16(a, v) emu:write16(a, v) end
function poke32(a, v) emu:write32(a, v) end
local WATCH = nil
function watch(a) WATCH = { addr = a, last = emu:read32(a) }; console:log(string.format("watching %08X", a)) end
function unwatch() WATCH = nil end
function where()
  local gs = emu:read32(ADDR.GameState)
  console:log(string.format("frame %d  map %s  tileset %s  pal %s", emu:currentFrame(), S.stage.map or "?", S.stage.tileset or "?", S.stage.pal or "?"))
  if gs >= 0x02000000 and gs < 0x02040000 then
    console:log(string.format("  GameState %08X  life %d/%d", gs, emu:read16(gs + 0x28), emu:read16(gs + 0x2A)))
  end
  if S.text then console:log(string.format("  last text #%d: %s", S.text.id, D.en[S.text.id] or D.jp[S.text.id] or "")) end
end
function help()
  console:log([[centaur commands:
  subs("en"|"both"|"jp"|"off")  subpos("top"|"bottom")  subid(true)
  hud(true|false)  god(true|false)
  bm("name")  go("name")  bms()      capture("tag")
  rec_start("name")  rec_stop()      where()
  peek8/16/32(a)  poke8/16/32(a,v)  watch(a)  unwatch()
in game: SELECT+L cycles subtitles, SELECT+R saves a quick bookmark
studio: python3 tools/studio/serve.py, then http://localhost:8642/live]])
end

-- Experimental warp: request scene N the way the script `load` command does
-- (08225508): 0x030039F0 = N, GameState+0x5A4 = N, request flags 0x030053E8.
-- Scenes are script procedures; python3 tools/mgba/scenes.py lists them. The
-- stage loads, but the player is not spawned (doors also set an entrance).
function warp(scene)
  local gs = emu:read32(ADDR.GameState)
  emu:write16(0x030039F0, scene)
  if gs >= 0x02000000 and gs < 0x02040000 then emu:write32(gs + 0x5A4, scene) end
  emu:write32(0x030053E8, 0x11)
  S.stage = {}; S.dirty = true
  LOG("warp to scene " .. scene); toast("warp " .. scene)
end
function scene()
  local gs = emu:read32(ADDR.GameState)
  return (gs >= 0x02000000 and gs < 0x02040000) and emu:read32(gs + 0x5A4) or nil
end
function centaur_state() return { map = S.stage.map, tileset = S.stage.tileset, pal = S.stage.pal, scene = scene() } end

-- save the overlay layers as PNGs (debugging the overlay without seeing the window)
function ovdump()
  local o = overlay()
  if not o then LOG("no overlay"); return end
  S.dirty = true; draw()
  o.sub.image:save(OUT .. "/overlay_sub.png", "PNG")
  o.hudl.image:save(OUT .. "/overlay_hud.png", "PNG")
  LOG(string.format("overlay dumped: canvas %dx%d screen %dx%d at %d,%d", o.w, o.h, o.sw, o.sh, o.x0, o.y0))
end

-- commands the studio can send
COMMANDS = {
  capture = function(c) capture(c.name) end,
  bookmark = function(c) bm(c.name) end,
  load = function(c) go(c.name) end,
  bookmarks = function() send({ t = "bookmarks", list = bm_index() }) end,
  warp = function(c) warp(c.scene) end,
  subs = function(c) subs(c.mode); if c.pos then subpos(c.pos) end end,
  god = function(c) god(c.on) end,
  rec_start = function(c) rec_start(c.name) end,
  rec_stop = function() rec_stop() end,
  keys = function(c) S.keyq[#S.keyq + 1] = { keys = c.keys or "", frames = c.frames or 2 } end,
  poke = function(c) local w = ({ [1] = emu.write8, [2] = emu.write16, [4] = emu.write32 })[c.size or 4]; w(emu, c.addr, c.value) end,
  eval = function(c)
    local fn, err = load(c.lua)
    if not fn then error(err) end
    local r = fn()
    send({ t = "eval", result = r ~= nil and tostring(r) or nil })
  end,
}

-- ---------------------------------------------------------------------------
-- per frame
local function keymask(spec)
  local m = 0
  for k in tostring(spec):gmatch("[^+]+") do
    for i, name in ipairs(KEYS) do if name == k:upper() then m = m | (1 << (i - 1)) end end
  end
  return m
end

local held_remote = nil
callbacks:add("keysRead", function()
  if held_remote then emu:setKeys(held_remote) end
end)

callbacks:add("frame", function()
  local f = emu:currentFrame()
  if rec then
    local k = emu:getKeys() & 0x3FF
    if k ~= rec.keys then flush(); rec.keys = k end
    rec.count = rec.count + 1; rec.frames = rec.frames + 1
    if rec.frames % 60 == 0 then S.dirty = true end
    if rec.frames % 3600 == 0 then rec.file:flush() end
  end
  if WATCH then
    local v = emu:read32(WATCH.addr)
    if v ~= WATCH.last then
      console:log(string.format("[watch] frame %d  %08X: %08X -> %08X", f, WATCH.addr, WATCH.last, v))
      WATCH.last = v
    end
  end
  if S.god then
    local gs = emu:read32(ADDR.GameState)
    if gs >= 0x02000000 and gs < 0x02040000 then emu:write16(gs + 0x28, emu:read16(gs + 0x2A)) end
  end
  -- hotkeys: SELECT+L cycles subtitles, SELECT+R saves a quick bookmark
  local k = emu:getKeys()
  local combo = (k & 0x204 == 0x204 and "L") or (k & 0x104 == 0x104 and "R") or nil
  if combo and combo ~= S.lastcombo then
    if combo == "L" then
      local order = { en = "both", both = "jp", jp = "off", off = "en" }
      subs(order[S.mode] or "en"); toast("subtitles: " .. S.mode)
    else
      bm(os.date("%m%d-%H%M%S"))
    end
  end
  S.lastcombo = combo
  -- toasts expire
  if S.toast and f - S.toast.frame == 150 then S.dirty = true end
  -- remote keys from the studio
  local q = S.keyq[1]
  if q then
    held_remote = keymask(q.keys)
    q.frames = q.frames - 1
    if q.frames <= 0 then table.remove(S.keyq, 1); if not S.keyq[1] then held_remote = nil end end
  end
  if f % 2 == 0 then link_poll() end
  link_flush()
  if f % 30 == 0 then
    local gs = emu:read32(ADDR.GameState)
    local life = (gs >= 0x02000000 and gs < 0x02040000) and emu:read16(gs + 0x28) or nil
    send({ t = "state", frame = f, rec = rec ~= nil, pos = S.pos, scene = scene(), map = S.stage.map, tileset = S.stage.tileset, pal = S.stage.pal, life = life, mode = S.mode, god = S.god })
  end
  draw()
end)

callbacks:add("reset", function() S.text = nil; S.stage = {}; S.dirty = true end)

LOG(string.format("centaur.lua loaded (breakpoints %s, overlay %s). help() for commands.",
  HAS_BP and "on" or "unavailable: use an mGBA 0.11 build", HAS_CANVAS and "on" or "unavailable"))
