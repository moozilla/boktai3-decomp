#!/bin/sh
# Build the mGBA 0.11 development version used by Centaur (scripting overlays and
# breakpoints), plus a headless runner patched for our tests.
#
#   tools/mgba/build_mgba.sh [DIR]     default DIR: ~/git/mgba
#
# macOS (Homebrew): brew install cmake qt@5 lua sdl2 ffmpeg libzip libepoxy pkgconf
# The patch (tools/mgba/mgba-headless.patch): sockets no longer raise SIGPIPE
# (a stopped studio must not kill the emulator), and the headless runner gets a
# debugger for scripts (MGBA_SCRIPT_DEBUGGER=1), a video buffer, the harness's
# fixed clock (BOKTAI3_RTC) and solar reading, and battery-save autoloading.
set -e
REPO=$(cd "$(dirname "$0")/../.." && pwd)
DIR=${1:-$HOME/git/mgba}
COMMIT=143adf5aa2a60c6036810b0cd72445e13d74b0e6
if [ ! -d "$DIR/.git" ]; then
  git clone https://github.com/mgba-emu/mgba.git "$DIR"
fi
cd "$DIR"
git fetch -q origin "$COMMIT" 2>/dev/null || true
if ! git diff --quiet; then
  echo "$DIR has local changes; leaving them (expected: our patch)"
else
  git checkout -q "$COMMIT"
  git apply "$REPO/tools/mgba/mgba-headless.patch"
fi
mkdir -p build && cd build
PREFIX=""
command -v brew >/dev/null && PREFIX="$(brew --prefix qt@5);$(brew --prefix)"
cmake .. -DCMAKE_PREFIX_PATH="$PREFIX" -DBUILD_QT=ON -DBUILD_SDL=OFF -DBUILD_HEADLESS=ON \
  -DUSE_LUA=ON -DUSE_DISCORD_RPC=OFF -DCMAKE_BUILD_TYPE=Release
make -j"$(sysctl -n hw.ncpu 2>/dev/null || nproc)"
echo "built: $DIR/build/qt/mGBA.app (GUI), $DIR/build/mgba-headless"
