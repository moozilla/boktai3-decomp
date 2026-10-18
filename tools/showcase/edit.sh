#!/bin/sh
# Cut the showcase: the in-game scene, a title card, the Studio tour (with the
# scene's music carried under it at a lower level), an end card.
#
#   tools/showcase/edit.sh GAME.mp4 STUDIO.mp4 OUT.mp4
set -e
GAME=$1; STUDIO=$2; OUT=$3
T=$(mktemp -d)
FONT=/System/Library/Fonts/Supplemental/Arial.ttf
BOLD=/System/Library/Fonts/Supplemental/Arial\ Bold.ttf
card() {  # card OUT.png "big" "small"
  magick -size 1280x800 xc:'#1b1916' -fill '#f2e9da' -font "$BOLD" -pointsize 64 -gravity center \
    -annotate +0-40 "$2" -fill '#d97757' -font "$FONT" -pointsize 34 -annotate +0+50 "$3" "$1"
}
card "$T/c1.png" "Behind the scenes" "The Boktai 3 decompilation Studio"
card "$T/c2.png" "Boktai 3 decompilation" "Every line of that scene was written by Claude"
seg() {  # seg IN OUT: 1280x800, 30 fps, stereo AAC (silent when the input has none)
  ffmpeg -v error -y -i "$1" -f lavfi -i anullsrc=r=48000:cl=stereo \
    -filter_complex "[0:v]scale=1280:800:force_original_aspect_ratio=decrease:flags=neighbor,pad=1280:800:(ow-iw)/2:(oh-ih)/2:color=#1b1916,fps=30,format=yuv420p[v]" \
    -map "[v]" -map 0:a? -map 1:a -shortest -c:v libx264 -crf 18 -c:a aac -ar 48000 -ac 2 "$2"
}
still() {  # still IN.png SECONDS OUT
  ffmpeg -v error -y -loop 1 -t "$2" -i "$1" -f lavfi -t "$2" -i anullsrc=r=48000:cl=stereo \
    -vf "fps=30,format=yuv420p,fade=in:0:12,fade=out:st=$(echo "$2 - 0.4" | bc):d=0.4" -c:v libx264 -crf 18 -c:a aac -ar 48000 -ac 2 -shortest "$3"
}
ffmpeg -v error -y -i "$GAME" -filter_complex "[0:v]scale=1200:800:flags=neighbor,pad=1280:800:40:0:color=#1b1916,fps=30,format=yuv420p[v];[0:a]aresample=48000[a]" \
  -map "[v]" -map "[a]" -c:v libx264 -crf 18 -c:a aac -ac 2 "$T/game.mp4"
still "$T/c1.png" 3 "$T/card1.mp4"
# the Studio tour with music from the scene's middle (its jukebox and theme), quieter, faded
SD=$(ffprobe -v error -show_entries format=duration -of csv=p=0 "$STUDIO")
ffmpeg -v error -y -i "$STUDIO" -ss 40 -i "$GAME" -filter_complex \
  "[0:v]fps=30,format=yuv420p[v];[1:a]volume=0.55,afade=t=in:d=1.5,afade=t=out:st=$(echo "$SD - 2" | bc):d=2,aresample=48000[a]" \
  -map "[v]" -map "[a]" -t "$SD" -c:v libx264 -crf 18 -c:a aac -ac 2 "$T/studio.mp4"
still "$T/c2.png" 4 "$T/card2.mp4"
printf "file '%s'\n" "$T/game.mp4" "$T/card1.mp4" "$T/studio.mp4" "$T/card2.mp4" > "$T/list.txt"
ffmpeg -v error -y -f concat -safe 0 -i "$T/list.txt" -c:v libx264 -crf 18 -c:a aac -ac 2 "$OUT"
mv "$T" ~/.Trash/"showcase-edit-$(date +%s)"
echo "$OUT: $(ffprobe -v error -show_entries format=duration -of csv=p=0 "$OUT") s"
