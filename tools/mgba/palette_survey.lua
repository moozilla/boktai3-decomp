-- Palette survey (headless): warp to every scene in build/centaur/scenes.lua and
-- log which sprite palette pool entry (9B1B) the game gives each large sprite.
--
--   python3 tools/mgba/scenes.py
--   tools/mgba/headless.sh tools/mgba/palette_survey.lua
--
-- Breakpoints on AnimData_Load(anim, hash) (082151E4) and
-- AnimData_SelectPaletteBank(anim, entry) (08215284) track the hash each anim
-- holds; every palette selection is written to build/centaur/sprite_pools.tsv
-- as scene, hash, pool entry and the caller (lr). The game picks the entry, so
-- this is the palette as the game itself sets it, for every sprite a scene
-- creates while it loads and runs for WAIT frames.
local SRC = (debug.getinfo(1, "S").source or ""):gsub("^@", "")
local ROOT = SRC:match("^(.*)/tools/mgba/[^/]*$") or "."
dofile(ROOT .. "/tools/mgba/centaur.lua")
local SCENES = dofile(ROOT .. "/build/centaur/scenes.lua")
local ONLY = tonumber(os.getenv("SURVEY_LIMIT") or "") or #SCENES
local WAIT = tonumber(os.getenv("SURVEY_WAIT") or "") or 360

local out = io.open(ROOT .. "/build/centaur/sprite_pools.tsv", "w")
out:write("scene\thash\tpool\tcaller\n")
local anim_hash, seen, scene = {}, {}, -1

emu:setBreakpoint(function()
  anim_hash[emu:readRegister("r0")] = emu:readRegister("r1") & 0xFFFF
end, 0x082151E4)
emu:setBreakpoint(function()
  local a, p, lr = emu:readRegister("r0"), emu:readRegister("r1") & 0xFFFF, emu:readRegister("lr")
  local h = anim_hash[a]
  local k = string.format("%d\t%s\t%d\t%08X", scene, h and string.format("%04X", h) or "?", p, (lr - 1) & ~1)
  if not seen[k] then seen[k] = true; out:write(k, "\n"); out:flush() end
end, 0x08215284)

local base, i, t0, phase, wait_restore = nil, 0, nil, nil, 0
local function start_next()
  i = i + 1
  if i > math.min(#SCENES, ONLY) then out:close(); console:log("palette survey done"); os.exit(0) end
  emu:loadStateBuffer(base)
  phase, t0 = "restore", nil
end

replay(ROOT .. "/tests/save_menus_field.txt", function()
  base = emu:saveStateBuffer()
  start_next()
end)

callbacks:add("frame", function()
  if phase == "restore" then
    wait_restore = wait_restore + 1
    if wait_restore >= 3 then
      wait_restore = 0
      scene = SCENES[i].scene
      warp(scene)
      phase, t0 = "load", emu:currentFrame()
    end
  elseif phase == "load" and emu:currentFrame() - t0 >= WAIT then
    if i % 25 == 0 then console:log(string.format("palette survey %d/%d", i, math.min(#SCENES, ONLY))) end
    start_next()
  end
end)
