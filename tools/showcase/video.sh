#!/bin/sh
# Build the showcase ROM and record its cutscene with sound.
#
#   tools/showcase/video.sh [OUT.mp4]      (default build/showcase/showcase.mp4)
#
# 1. tools/showcase/showcase.py writes build/showcase/showcase.gba;
# 2. mGBA headless (tools/mgba/scenario.lua) loads the test save, warps into
#    the new scene and writes a savestate once it is up;
# 3. the harness (tools/emu/harness, built with FFmpeg) loads that state and
#    records H.264/AAC at 4x while pressing A to turn the dialogue pages.
# Needs baserom.gba, ../boktai3trans (the translation patch) and an mGBA
# built with FFmpeg (tools/mgba/build_mgba.sh). Everything stays under build/.
set -e
REPO=$(cd "$(dirname "$0")/../.." && pwd)
OUT=${1:-$REPO/build/showcase/showcase.mp4}
TAP=${TAP:-220}
cd "$REPO"
INFO=$(python3 tools/showcase/showcase.py)
echo "$INFO"
SCENE=$(echo "$INFO" | sed 's/.*scene \([0-9]*\) entrance \([0-9]*\).*/\1/')
ENTRANCE=$(echo "$INFO" | sed 's/.*scene \([0-9]*\) entrance \([0-9]*\).*/\2/')
mkdir -p build/showcase
printf "from field\nwarp %s %s\nwait 90\nstate start\n" "$SCENE" "$ENTRANCE" > build/showcase/start.txt
SCENARIO=build/showcase/start.txt tools/mgba/headless.sh tools/mgba/scenario.lua "$REPO/build/showcase/showcase.gba" > /dev/null
STATE=build/centaur/scenarios/start/start.ss
{
  echo "trace off"
  echo "loadstate $REPO/$STATE"
  echo "record $OUT"
  i=0
  while [ $i -lt 40 ]; do echo "wait $TAP"; echo "tap A"; i=$((i + 1)); done
  echo "stoprecord"
} > build/showcase/record.txt
mkdir -p build/showcase/run
tools/emu/harness build/showcase/showcase.gba build/showcase/record.txt build/showcase/run > /dev/null 2>&1
echo "$OUT: $(ffprobe -v error -show_entries format=duration -of csv=p=0 "$OUT") s"
# trim the white screen after the closing fade: cut 3 s after the last frame
# that is not (nearly) all white
LAST=$(ffmpeg -v error -i "$OUT" -vf "fps=4,scale=16:16,format=gray" -f rawvideo - | python3 -c "
import sys
d = sys.stdin.buffer.read(); n = len(d) // 256
last = max((i for i in range(n) if sum(d[i * 256:(i + 1) * 256]) / 256 < 240), default=n - 1)
print(last / 4 + 3)")
ffmpeg -v error -y -i "$OUT" -t "$LAST" -c copy "${OUT%.mp4}.trim.mp4" && mv "${OUT%.mp4}.trim.mp4" "$OUT"
echo "$OUT: trimmed to $LAST s"
