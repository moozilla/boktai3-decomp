#!/bin/sh
# Play Boktai 3 with Centaur: subtitles, bookmarks and the studio's live view.
#
#   tools/mgba/play.sh [ROM]        default: baserom.gba (keeps recordings valid
#                                   for the matching build); or build/english.gba
#
# Uses the mGBA 0.11 development build from tools/mgba/build_mgba.sh (overlay +
# breakpoints) when present, else MGBA=/path/to/mGBA. Starts the studio
# (http://localhost:8642/live) if it is not already running.
set -e
cd "$(dirname "$0")/../.."
ROM=${1:-baserom.gba}
MGBA=${MGBA:-$HOME/git/mgba/build/qt/mGBA.app/Contents/MacOS/mGBA}
[ -x "$MGBA" ] || MGBA=$(command -v mGBA || command -v mgba-qt || true)
[ -n "$MGBA" ] || { echo "no mGBA found: run tools/mgba/build_mgba.sh or set MGBA="; exit 1; }

if [ ! -f build/centaur/data.lua ] || [ build/text/bank.tsv -nt build/centaur/data.lua ]; then
  python3 tools/mgba/gen_centaur_data.py
fi
if ! curl -s -o /dev/null http://localhost:8642/api/live; then
  echo "starting the studio: http://localhost:8642/live (log: build/studio.log)"
  nohup python3 tools/studio/serve.py > build/studio.log 2>&1 &
  sleep 2
fi
command -v open >/dev/null && open http://localhost:8642/live
exec "$MGBA" --script "$PWD/tools/mgba/centaur.lua" "$ROM"
