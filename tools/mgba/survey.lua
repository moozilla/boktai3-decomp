-- Stage survey (headless): warp to every scene in build/centaur/scenes.lua from
-- the field of the test save, let it load, and capture it for the studio.
--
--   python3 tools/mgba/scenes.py
--   tools/mgba/headless.sh tools/mgba/survey.lua
--
-- Each scene gets a capture named scene_NNNN (palettes, VRAM, OAM, screenshot),
-- analysed by the studio (stage palettes -> data/stage_palettes.csv, sprite
-- palettes -> build/centaur/sprite_palettes.json). Results also go to
-- build/centaur/survey.tsv. A warp loads the stage but does not spawn the player
-- (see docs/CENTAUR.md), which is enough for palettes and scenery.
local SRC = (debug.getinfo(1, "S").source or ""):gsub("^@", "")
local ROOT = SRC:match("^(.*)/tools/mgba/[^/]*$") or "."
dofile(ROOT .. "/tools/mgba/centaur.lua")
local SCENES = dofile(ROOT .. "/build/centaur/scenes.lua")
local ONLY = tonumber(os.getenv("SURVEY_LIMIT") or "") or #SCENES
local WAIT = 240

local base, i, t0, phase = nil, 0, nil, nil
local out = io.open(ROOT .. "/build/centaur/survey.tsv", "a")

-- loadStateBuffer takes effect at the next frame, so restore, wait, then warp.
local function done(scene)
  local f = io.open(string.format("%s/build/centaur/captures/scene_%04d/meta.json", ROOT, scene), "r")
  if f then f:close(); return true end
end
local function start_next()
  i = i + 1
  while i <= math.min(#SCENES, ONLY) and done(SCENES[i].scene) do i = i + 1 end
  if i > math.min(#SCENES, ONLY) then out:close(); console:log("survey done"); os.exit(0) end
  if os.getenv("SURVEY_RESTORE") then emu:loadStateBuffer(base) end
  phase, t0 = "restore", nil
end

replay(ROOT .. "/tests/save_menus_field.txt", function()
  base = emu:saveStateBuffer()
  start_next()
end)

local wait_restore = 0
callbacks:add("frame", function()
  if phase == "restore" then
    wait_restore = wait_restore + 1
    if wait_restore >= 3 then
      wait_restore = 0
      warp(SCENES[i].scene)
      phase, t0 = "load", emu:currentFrame()
    end
  elseif phase == "load" and emu:currentFrame() - t0 >= WAIT then
    local s = SCENES[i]
    local st = centaur_state()
    capture(string.format("scene_%04d", s.scene))
    out:write(string.format("%d\t%s\t%s\t%s\t%s\n", s.scene, s.map, st.map or "", st.tileset or "", st.pal or ""))
    out:flush()
    if i % 25 == 0 then console:log(string.format("survey %d/%d", i, math.min(#SCENES, ONLY))) end
    start_next()
  end
end)
