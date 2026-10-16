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
--     (Text_LookupString, 0821ADD4) is shown with the fan translation, just
--     under the place where the game draws it (the BG0 cells its glyphs go to,
--     sub_08218FB4 / sub_08218F2C), for as long as those glyphs stay on screen.
--     Run `python3 tools/mgba/gen_centaur_data.py` first (writes build/centaur/data.lua).
--   * GCL debug prints: the scripts' `print` messages (dropped by this ROM),
--     with their English, in a "GCL print" console pane and centaur.log.
--   * What is loading: resource lookups (ScriptProc_Start, 0821A520) and stage
--     tilesets (sub_08215D40), logged and shown in the HUD.
--   * Bookmarks: named savestates with screenshots (bm, go, bms) - a warp list.
--   * Studio link: connects to `python3 tools/studio/serve.py` (port 8643, or
--     $CENTAUR_LINK_PORT for a studio started with --link-port) to
--     stream text/resources and take commands (capture, bookmarks, keys, ...).
--     Open http://localhost:8642/live.
--   * Input recorder for the headless harness (rec_start / rec_stop).
--
-- Console commands (type in the scripting console):
--   help()                  this list
--   (in game) SELECT+L cycles subtitles, SELECT+R saves a quick bookmark
--   subs("en"|"both"|"jp"|"off")   subtitle mode (default "en")
--   substyle("shadow"|"box"), subsize(1.0), suboffset(px)   look, size, shift down (GBA px)
--   subpos("top"|"bottom"), subid(true) subtitle position; show string IDs
--   hud(true|false)         status overlay
--   bm("name") / go("name") / bms()   save / load / list bookmarks
--   god(true|false)         keep life at maximum (GameState +0x28/+0x2A)
--   warp(scene[, entrance]), scene()   go to a scene the way a door does (player placed)
--   callproc(id, {args})    run a script procedure (Script_Call) at the next frame
--   capture()               send a scene capture to the studio
--   rec_start("name"), rec_stop()     record inputs from here (savestate + inputs;
--                           replayable in mGBA and the harness); rec_start("name", "reset"): from power-on
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
  GlyphAt16 = 0x08218FB4,       -- (glyph, x, y, ...) 16x16 glyph into the BG0 map at tile x, y
  GlyphAt8 = 0x08218F2C,        -- (glyph, x, y, ...) 8x16 glyph
  Script_Print = 0x0821B858,    -- GCL `print` handler (r0 = its operands); prints nothing in this ROM
}

local S = {
  mode = "en", hud = true, god = false, pos = "bottom", showid = false,
  style = "shadow",      -- subtitle look: "shadow" (outlined text) or "box" (dark box)
  size = 1.0,            -- subtitle text size, 1 = 6 GBA pixels
  offset = 0,            -- move subtitles down (+) or up (-), in GBA pixels
  text = nil,            -- { id, frame }: the last string fetched that has text
  texts = {},            -- recent { id, frame }
  items = {},            -- strings on screen: { id, frame, cells = {[y*32+x] = true}, n, x0, y0, x1, y1, last }
  res = {},              -- recent { type, name, frame }
  stage = {},            -- map, tileset, pal
  keyq = {},             -- queued remote input { keys, frames }
  dirty = true,
}

-- subtitle settings, kept between sessions in build/centaur/settings.json
local SETTINGS = { "mode", "style", "size", "offset", "pos", "showid", "hud" }
local function save_settings()
  local f = io.open(OUT .. "/settings.json", "w")
  if not f then return end
  local parts = {}
  for _, k in ipairs(SETTINGS) do
    local v = S[k]
    parts[#parts + 1] = string.format('"%s":%s', k, type(v) == "string" and ('"' .. v .. '"') or tostring(v))
  end
  f:write("{" .. table.concat(parts, ",") .. "}\n"); f:close()
end

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

do
  local f = io.open(OUT .. "/settings.json", "r")
  if f then
    local t = unjson(f:read("a")); f:close()
    if type(t) == "table" then for _, k in ipairs(SETTINGS) do if t[k] ~= nil then S[k] = t[k] end end end
  end
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
    if s._s:connect("127.0.0.1", tonumber(os.getenv("CENTAUR_LINK_PORT") or "") or 8643) ~= 0 then s._s:close(); s = nil end
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
  local e = { id = id, frame = f, scene = scene() }
  S.texts[#S.texts + 1] = e
  if #S.texts > 40 then table.remove(S.texts, 1) end
  if D.jp[id] or D.en[id] then S.text = e; S.dirty = true end
  send({ t = "text", id = id, frame = f, scene = scene(), map = S.stage.map })
end

-- GCL `print`: the handler drops its text in this ROM, so log it here. The
-- first operand is an inline string (type 7: length byte, EUC-JP bytes);
-- D.prints maps those bytes to the text and its English (gcl/prints.csv).
-- Lines are queued and written from the frame callback.
local printq, printbuf = {}, nil
local function on_print()
  local p = emu:readRegister("r0")
  if emu:read8(p) ~= 7 then return end
  local t = {}
  for i = 0, emu:read8(p + 1) - 1 do
    local c = emu:read8(p + 2 + i)
    if c == 0 then break end
    t[#t + 1] = string.char(c)
  end
  local raw = table.concat(t)
  local e = D.prints and D.prints[raw]
  if #printq < 200 then printq[#printq + 1] = e or { jp = (not raw:find("[\128-\255]")) and raw or nil, raw = raw } end
end

local function print_flush()
  if not printq[1] then return end
  if printbuf == nil then  -- no panes headless (creating one there crashes)
    local ok, b = false, nil
    if not CENTAUR_HEADLESS and not os.getenv("CENTAUR_HEADLESS") then
      ok, b = pcall(function() return console:createBuffer("GCL print") end)
    end
    printbuf = ok and b or false
  end
  local f = emu:currentFrame()
  for _, e in ipairs(printq) do
    local jp = e.jp or ("<" .. e.raw:gsub(".", function(c) return string.format("%02X", c:byte()) end) .. ">")
    local line = string.format("[%d] %s", f, jp) .. (e.en and e.en ~= "" and ("\n        " .. e.en) or "")
    if printbuf then printbuf:print(line .. "\n") else console:log("print: " .. line) end
    if LOGF then LOGF:write(os.date("%H:%M:%S "), "print: ", line, "\n"); LOGF:flush() end
    send({ t = "print", frame = f, jp = e.jp, en = e.en, scene = scene() })
  end
  printq = {}
end

-- A glyph drawn at BG0 tile (x, y), w x 2 tiles: it belongs to the last string
-- fetched. Cells it covers are taken from any older string there.
local function on_glyph(w)
  local e = S.text
  if not e then return end
  local x, y = emu:readRegister("r1") & 31, emu:readRegister("r2") & 31
  local f = emu:currentFrame()
  local it = S.items[#S.items]
  if not it or it.key ~= e then
    it = { key = e, id = e.id, frame = f, cells = {}, n = 0 }
    S.items[#S.items + 1] = it
  end
  for dy = 0, 1 do
    for dx = 0, w - 1 do
      local c = ((y + dy) & 31) * 32 + ((x + dx) & 31)
      for _, o in ipairs(S.items) do
        if o ~= it and o.cells[c] then o.cells[c] = nil; o.n = o.n - 1 end
      end
      if not it.cells[c] then it.cells[c] = true; it.n = it.n + 1 end
    end
  end
  it.last = f
  it.x0 = math.min(it.x0 or x, x); it.x1 = math.max(it.x1 or 0, x + w - 1)
  it.y0 = math.min(it.y0 or y, y); it.y1 = math.max(it.y1 or 0, y + 1)
  S.dirty = true
end

-- Drop strings whose glyphs are gone from the BG0 map in VRAM (text glyph
-- entries are palette 15, tiles 0x100-0x1FF: the glyph cache).
local function check_items()
  if #S.items == 0 then return end
  local base = 0x06000000 + ((emu:read16(0x04000008) >> 8) & 31) * 0x800
  local f, keep = emu:currentFrame(), {}
  for _, it in ipairs(S.items) do
    if f - it.last > 8 then
      for c in pairs(it.cells) do
        local v = emu:read16(base + c * 2)
        if v >> 12 ~= 15 or (v & 0x3FF) < 0x100 or (v & 0x3FF) > 0x1FF then it.cells[c] = nil; it.n = it.n - 1 end
      end
    end
    if it.n > 0 then keep[#keep + 1] = it else S.dirty = true end
  end
  S.items = keep
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
  elseif label == "hp" then S.stage.hp = hex4(name)
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
    emu:setBreakpoint(function() on_glyph(2) end, ADDR.GlyphAt16)
    emu:setBreakpoint(function() on_glyph(1) end, ADDR.GlyphAt8)
    emu:setBreakpoint(on_print, ADDR.Script_Print)
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
  ov = { w = w, h = h, sw = sw, sh = sh, x0 = (w - sw) // 2, y0 = (h - sh) // 2, k = sw / 240 }
  ov.sub = canvas:newLayer(sw, sh)
  ov.sub:setPosition(ov.x0, ov.y0)
  ov.subp = image.newPainter(ov.sub.image)
  ov.hudl = canvas:newLayer(sw, math.max(12, sh // 14))
  ov.hudl:setPosition(ov.x0, ov.y0)
  ov.hudp = image.newPainter(ov.hudl.image)
  local fp = font_path()
  if fp then ov.subp:loadFont(fp); ov.hudp:loadFont(fp) end
  ov.fs = 0
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

local function subtitle_text(e)
  e = e or S.text
  if not e or S.mode == "off" then return nil end
  local en, jp = D.en[e.id], D.jp[e.id]
  if S.mode == "jp" then return jp end
  if S.mode == "both" then return (jp or "") .. (en and ("\n" .. en) or "") end
  return en or jp
end

-- The string fetched last, when its glyphs were never drawn (graphics text,
-- sprites, other renderers): shown at the screen edge for a few seconds, and
-- only when it is more than a substituted name or a control string.
local function unplaced()
  local e = S.text
  if not e or emu:currentFrame() - e.frame > 300 or emu:currentFrame() - e.frame < 20 then return nil end
  for _, it in ipairs(S.items) do if it.key == e then return nil end end
  if e.scene ~= scene() then return nil end
  local t = subtitle_text(e)
  if not t or utf8.len(t) == nil or utf8.len(t) < 6 then return nil end
  return t
end

-- Draw wrapped text, w wide, at (x, y) (y = bottom edge when up): white on a
-- dark box, or outlined white text with no box (S.style).
local function draw_box(p, txt, x, y, w, up, taken)
  local lines = wrap(p, txt, w - 6)
  local lh = math.ceil(p:textRunMetrics("Ägy"):height()) + 1
  local n = math.min(#lines, 6)
  local h = n * lh + 3
  local top = up and (y - h) or y
  for _, r in ipairs(taken) do  -- step past boxes already drawn there
    if top < r[2] + r[3] and top + h > r[2] and x < r[1] + r[4] and x + w > r[1] then top = up and (r[2] - h) or (r[2] + r[3]) end
  end
  top = math.max(0, math.min(top, ov.sh - h))
  if S.style == "box" then
    p:setFillColor(0xC0000000); p:drawRectangle(x, top, w, h)
  else
    local d = math.max(1, math.floor(ov.k / 2 + 0.5))
    p:setFillColor(0xE0000000)
    for k = 1, n do
      for _, o in ipairs({ { -d, 0 }, { d, 0 }, { 0, -d }, { 0, d }, { d, d } }) do p:drawText(lines[k], x + 3 + o[1], top + 1 + (k - 1) * lh + o[2]) end
    end
  end
  p:setFillColor(0xFFFFFFFF)
  for k = 1, n do p:drawText(lines[k], x + 3, top + 1 + (k - 1) * lh) end
  taken[#taken + 1] = { x, top, h, w }
  return top, h
end

local function draw()
  if not S.dirty then return end
  S.dirty = false
  local o = overlay()
  local txt = subtitle_text()
  if o then
    clear(o.subp, o.sw, o.sh)
    local fs = math.max(5, 6 * o.k * (tonumber(S.size) or 1))
    if fs ~= o.fs then o.fs = fs; o.subp:setFontSize(fs) end
    local dy = math.floor((tonumber(S.offset) or 0) * o.k)
    local bg0 = emu:read16(0x04000000) & 0x100 ~= 0
    local taken = {}
    if S.mode ~= "off" and bg0 then
      local list = {}
      for _, it in ipairs(S.items) do list[#list + 1] = it end
      table.sort(list, function(a, b) return a.y0 < b.y0 or (a.y0 == b.y0 and a.x0 < b.x0) end)
      for _, it in ipairs(list) do
        local t = subtitle_text(it)
        if t and t ~= "" then
          if S.showid then t = "#" .. it.id .. " " .. t end
          local k = o.k
          local x = math.floor(it.x0 * 8 * k)
          local w = math.max(math.floor((it.x1 - it.x0 + 1) * 8 * k), math.floor(96 * k))
          if x + w > o.sw then x = math.max(0, o.sw - w) end
          local below = math.floor((it.y1 + 1) * 8 * k) + dy
          local need = (math.ceil(o.subp:textRunMetrics("Ägy"):height()) + 1) * math.min(6, #wrap(o.subp, t, w - 6)) + 3
          if below + need <= o.sh then draw_box(o.subp, t, x, below, w, false, taken)
          else draw_box(o.subp, t, x, math.floor(it.y0 * 8 * k) - dy, w, true, taken) end
        end
      end
    end
    local un = S.mode ~= "off" and unplaced()
    if un then
      if S.showid then un = "#" .. S.text.id .. " " .. un end
      draw_box(o.subp, un, 0, S.pos == "top" and 0 or o.sh, o.sw, S.pos ~= "top", taken)
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
-- From here (default): the current savestate is kept as NAME.ss next to the
-- inputs, and the script starts with "loadstate NAME.ss", which replay() in
-- mGBA and tools/emu/harness (built against mGBA 0.11) both understand.
-- from == "reset": record from power-on instead.
function rec_start(name, from)
  if rec then rec_stop() end
  name = (name or "playthrough"):gsub("[^%w_%-]", "_")
  local path = OUT .. "/" .. name .. ".txt"
  local f = io.open(path, "w")
  if not f then console:error("cannot open " .. path); return end
  if from == "reset" then
    f:write("# recorded with tools/mgba/centaur.lua on " .. os.date() .. " from power-on; replay: tools/emu/harness ROM " .. path .. " OUTDIR\n")
    emu:reset()
  else
    local ss = io.open(OUT .. "/" .. name .. ".ss", "wb"); ss:write(emu:saveStateBuffer()); ss:close()
    f:write("# recorded with tools/mgba/centaur.lua on " .. os.date() .. string.format(" from frame %d, map %s, scene %s; replay: replay(\"build/centaur/%s.txt\") in mGBA, or tools/emu/harness ROM build/centaur/%s.txt OUTDIR\n", emu:currentFrame(), S.stage.map or "?", tostring(scene()), name, name))
    f:write("loadstate " .. name .. ".ss\n")
  end
  rec = { file = f, keys = 0, count = 0, frames = 0, path = path, name = name }
  LOG("recording to " .. path .. (from == "reset" and " (game reset)" or " (from here)"))
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
  if rec then flush(); rec.file:write("loadstate bookmarks/" .. name .. ".ss\n") end
  emu:loadStateBuffer(data)
  S.items = {}; S.text = nil; S.dirty = true
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
  local onscreen = {}
  for _, it in ipairs(S.items) do onscreen[#onscreen + 1] = { id = it.id, x0 = it.x0, y0 = it.y0, x1 = it.x1, y1 = it.y1 } end
  local meta = io.open(dir .. "/meta.json", "w")
  meta:write(json({ frame = emu:currentFrame(), scene = scene(), map = S.stage.map, tileset = S.stage.tileset, pal = S.stage.pal,
                    text = S.text and S.text.id or nil, onscreen = onscreen, time = os.date("%Y-%m-%d %H:%M:%S") }))
  meta:close()
  console:log("capture saved: " .. dir); toast("captured")
  send({ t = "capture", name = name, dir = dir })
end

-- ---------------------------------------------------------------------------
-- debug helpers
function god(on) S.god = on ~= false; S.dirty = true; console:log("god mode " .. (S.god and "on" or "off")) end
function subs(mode) S.mode = mode or "en"; S.dirty = true; save_settings() end
function substyle(st) S.style = st == "box" and "box" or "shadow"; S.dirty = true; save_settings() end
function subsize(n) S.size = math.max(0.5, math.min(3, tonumber(n) or 1)); S.dirty = true; save_settings() end
function suboffset(n) S.offset = math.floor(tonumber(n) or 0); S.dirty = true; save_settings() end
function subpos(where)
  S.pos = where == "top" and "top" or "bottom"
  S.dirty = true; save_settings()
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
  substyle("shadow"|"box")  subsize(1.0)  suboffset(0)   (saved in build/centaur/settings.json)
  hud(true|false)  god(true|false)
  bm("name")  go("name")  bms()      capture("tag")
  rec_start("name")  rec_stop()      where()
  peek8/16/32(a)  poke8/16/32(a,v)  watch(a)  unwatch()
in game: SELECT+L cycles subtitles, SELECT+R saves a quick bookmark
studio: python3 tools/studio/serve.py, then http://localhost:8642/live]])
end

-- Warp to scene N, arriving at entrance E, the way a door does: proc_945(scene,
-- entrance, back) runs proc_953 (r708[0x70] = entrance, the number the scene's
-- entrance procedures test to place the player) and proc_952 (the transition
-- actor that loads the scene). Without E, the scene's default entrance from
-- build/centaur/scenes.lua (tools/mgba/scenes.py) is used; entrances per scene
-- are in build/assets/stages.json (tools/assets/stages.py, docs/ASSETS.md).
-- Verified headlessly from the start bookmark: 2705/57, 2712/8 and the late
-- field 3243/126 all load with the player placed and playable.
local SCENE_ENTRANCE = nil
local function default_entrance(scene)
  if not SCENE_ENTRANCE then
    SCENE_ENTRANCE = {}
    local ok, list = pcall(dofile, OUT .. "/scenes.lua")
    if ok and type(list) == "table" then
      for _, s in ipairs(list) do SCENE_ENTRANCE[s.scene] = s.entrance end
    end
  end
  return SCENE_ENTRANCE[scene]
end
-- The older warp, for scenes without a known entrance: what "continue" does
-- (sub_081E132C at 081E14E0). It loads the stage but does not place the player.
local function warp_continue(scene)
  local gs = emu:read32(ADDR.GameState)
  local cont = emu:read32(0x020003A8)
  if gs >= 0x02000000 and gs < 0x02040000 then emu:write32(gs + 0x5A4, scene) end
  emu:write32(0x0300546C, 0)
  local function finish()
    emu:write32(0x0300523C, 0)
    emu:write32(0x030053E8, 0x11)
    emu:write16(0x030039F0, scene)
  end
  if HAS_BP and cont > 0 and cont < 0x10000 then callproc(cont, {}, finish) else finish() end
  return cont
end
function warp(scene, entrance)
  entrance = entrance or default_entrance(scene)
  if HAS_BP and entrance then
    callproc(945, { scene, entrance, 0 })
    LOG(string.format("warp to scene %d entrance %d", scene, entrance)); toast(string.format("warp %d.%d", scene, entrance))
  else
    local cont = warp_continue(scene)
    LOG(string.format("warp to scene %d (no entrance; continue proc %d)", scene, cont)); toast("warp " .. scene)
  end
  S.stage = {}; S.dirty = true
end
function scene()
  local gs = emu:read32(ADDR.GameState)
  return (gs >= 0x02000000 and gs < 0x02040000) and emu:read32(gs + 0x5A4) or nil
end
-- The player's world position (GameState+0x30/+0x32/+0x34, s16 x, y, z) and
-- the BG2 camera: the game keeps each BG's full scroll in a 0x30-byte record at
-- 0x03004C30 + 0x30 * bg (+0x20 x, +0x22 y, map pixels); Bg_UpdateScrollShadow masks
-- them into the shadow at 0x03004C20 that is copied to BGxHOFS/VOFS.
local function s16(v) return v >= 0x8000 and v - 0x10000 or v end
function where_player()
  local gs = emu:read32(ADDR.GameState)
  local m = { t = "player", frame = emu:currentFrame(), scene = scene(), map = S.stage.map, hp = S.stage.hp }
  if gs >= 0x02000000 and gs < 0x02040000 then
    m.x, m.y, m.z = s16(emu:read16(gs + 0x30)), s16(emu:read16(gs + 0x32)), s16(emu:read16(gs + 0x34))
  end
  m.cx, m.cy = emu:read16(0x03004CB0), emu:read16(0x03004CB2)
  return m
end
-- Story state: script variables as gcx.py prints them, region[offset.bit]:type.
-- Regions are pointers: r700 at 0x02000700 (scratch), r708 at 0x02000708 (the
-- script work area, where progress r708[0x3c] and most scene flags live), gs =
-- GameState (0x02000710). Types (sub_0821B20C/Script_SetVar): 1/6 16-bit, 2/3
-- byte, 4 one bit, 8 24-bit, 9 word. The page chooses what to watch ("vars").
local VAR_BASE = { r700 = 0x02000700, r708 = 0x02000708, gs = 0x02000710 }
local STORY_VARS = {}
local function var_addr(v)
  local p = VAR_BASE[v.r] and emu:read32(VAR_BASE[v.r])
  if not p or p < 0x02000000 or p >= 0x02040000 then return nil end
  return p + v.o
end
local function read_var(v)
  local a = var_addr(v)
  if not a then return nil end
  if v.t == 4 then return (emu:read8(a + ((v.b or 0) >> 3)) >> ((v.b or 0) & 7)) & 1
  elseif v.t == 2 or v.t == 3 then return emu:read8(a)
  elseif v.t == 8 then return emu:read32(a) & 0xFFFFFF
  elseif v.t == 9 then return emu:read32(a)
  else local x = emu:read16(a); return v.t == 1 and x >= 0x8000 and x - 0x10000 or x end
end
local function write_var(v, value)
  local a = var_addr(v)
  if not a then return end
  if v.t == 4 then
    local q, m = a + ((v.b or 0) >> 3), 1 << ((v.b or 0) & 7)
    emu:write8(q, value ~= 0 and (emu:read8(q) | m) or (emu:read8(q) & ~m))
  elseif v.t == 2 or v.t == 3 then emu:write8(a, value & 0xFF)
  elseif v.t == 8 then emu:write32(a, (emu:read32(a) & 0xFF000000) | (value & 0xFFFFFF))
  elseif v.t == 9 then emu:write32(a, value)
  else emu:write16(a, value & 0xFFFF) end
end
function story()
  local m = { t = "story", progress = read_var({ r = "r708", o = 0x3C, t = 1 }),
              entrance = read_var({ r = "r708", o = 0x70, t = 1 }), scene = scene(), flags = {}, vars = {} }
  for k = 0, 4 do m.flags[#m.flags + 1] = read_var({ r = "gs", o = 0x720 + 4 * k, t = 9 }) or false end
  for i, v in ipairs(STORY_VARS) do m.vars[i] = read_var(v) or false end
  return m
end
function setvar(r, o, t, b, value)
  write_var({ r = r, o = o, t = t, b = b }, value)
  LOG(string.format("set %s[0x%x%s]:t%d = %d", r, o, t == 4 and ("." .. (b or 0)) or "", t, value))
end
function centaur_state() return { map = S.stage.map, tileset = S.stage.tileset, pal = S.stage.pal, scene = scene(), items = S.items, text = S.text } end

-- Run a script procedure: callproc(id, {args...}). At the next entry of the
-- per-frame TaskSystem_RunFrame (0821A054, no arguments) the CPU is diverted
-- into Script_Call(id, args) (0821AD08), which returns to RunFrame's entry; the
-- second hit restores the original return address. Arguments are built in a
-- scratch area at the top of EWRAM (unused in captures).
local CALL = { queue = {}, saved = nil }
local SCRATCH = 0x0203FF00
function callproc(pid, args, after)
  CALL.queue[#CALL.queue + 1] = { pid = pid, args = args or {}, after = after }
  LOG(string.format("callproc %d (%d args) queued", pid, #(args or {})))
end
local function on_runframe()
  if CALL.saved then
    emu:writeRegister("lr", CALL.saved)
    CALL.saved = nil
    if CALL.after then local f = CALL.after; CALL.after = nil; f() end
    return
  end
  local c = table.remove(CALL.queue, 1)
  if not c then return end
  local argp = 0
  if #c.args > 0 then
    for k, v in ipairs(c.args) do emu:write32(SCRATCH + 8 + (k - 1) * 4, v) end
    emu:write32(SCRATCH, #c.args)
    emu:write32(SCRATCH + 4, SCRATCH + 8)
    argp = SCRATCH
  end
  CALL.saved = emu:readRegister("lr")
  CALL.after = c.after
  emu:writeRegister("r0", c.pid)
  emu:writeRegister("r1", argp)
  emu:writeRegister("lr", 0x0821A054 | 1)
  emu:writeRegister("pc", 0x0821AD08)
end
if HAS_BP then emu:setBreakpoint(on_runframe, 0x0821A054) end

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
  warp = function(c) warp(c.scene, c.entrance) end,
  vars = function(c) STORY_VARS = c.list or {}; send(story()) end,
  setvar = function(c) setvar(c.r, c.o, c.t, c.b, c.value); send(story()) end,
  callproc = function(c) callproc(c.id, c.args) end,
  subs = function(c)
    if c.mode then subs(c.mode) end
    if c.pos then subpos(c.pos) end
    if c.style then substyle(c.style) end
    if c.size then subsize(c.size) end
    if c.offset then suboffset(c.offset) end
  end,
  god = function(c) god(c.on) end,
  rec_start = function(c) rec_start(c.name, c.from) end,
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
  if f % 4 == 0 then check_items() end
  if f % 15 == 0 and S.text then S.dirty = true end  -- unplaced subtitles expire
  if f % 2 == 0 then link_poll() end
  print_flush()
  link_flush()
  if f % 6 == 0 then send(where_player()) end
  if f % 20 == 10 then send(story()) end
  if f % 30 == 0 then
    local gs = emu:read32(ADDR.GameState)
    local life = (gs >= 0x02000000 and gs < 0x02040000) and emu:read16(gs + 0x28) or nil
    local onscreen = {}
    for _, it in ipairs(S.items) do onscreen[#onscreen + 1] = { id = it.id, x0 = it.x0, y0 = it.y0, x1 = it.x1, y1 = it.y1 } end
    send({ t = "state", frame = f, rec = rec ~= nil, pos = S.pos, onscreen = onscreen, scene = scene(), map = S.stage.map, tileset = S.stage.tileset, hp = S.stage.hp, pal = S.stage.pal, life = life, mode = S.mode, style = S.style, size = S.size, offset = S.offset, god = S.god })
  end
  draw()
end)

callbacks:add("reset", function() S.text = nil; S.items = {}; S.stage = {}; S.dirty = true end)

LOG(string.format("centaur.lua loaded (breakpoints %s, overlay %s). help() for commands.",
  HAS_BP and "on" or "unavailable: use an mGBA 0.11 build", HAS_CANVAS and "on" or "unavailable"))
