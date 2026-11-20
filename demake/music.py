"""Reduce an MP2K song to four monophonic DMG voices and preserve its GOTO loop."""
import collections
import json
import math
import shutil
import struct
import subprocess
import sys

from art import ROOT

sys.path.insert(0, str(ROOT / "tools/studio"))
import sound


def render_score(song, max_frames=14400):
    data = sound.song_data(song)
    if data is None:
        raise ValueError(f"invalid MP2K song {song}")
    node = shutil.which("node")
    if node is None:
        raise ValueError("MP2K conversion requires Node.js to run the repository's Studio sequencer")
    proc = subprocess.run([node, ROOT / "demake/score.js", str(max_frames)], input=json.dumps(data),
                          text=True, capture_output=True, check=True)
    score = json.loads(proc.stdout)
    if not score["events"]:
        raise ValueError(f"song {song} has no note events")
    return score


def choose(active):
    """Prefer the source PSG voice and loud, recent events; keep bass out of the two melody slots."""
    pitched = [n for n in active if not n["drum"]]
    drums = [n for n in active if n["drum"]]
    def best(notes, rank):
        return max(notes, key=rank) if notes else None
    bass = best([n for n in pitched if n["kind"] == 3 or n["pitch"] <= 60],
                lambda n: (n["kind"] == 3, n["level"] - max(0, n["pitch"] - 48) * .012, n["start"]))
    voices = []
    for kind in (1, 2):
        pool = [n for n in pitched if n is not bass and not any(
            n["track"] == other["track"] or n["pitch"] == other["pitch"] for other in voices if other)]
        note = best(pool, lambda n: (.32 * (n["kind"] == kind) + n["level"] +
                                     .01 * min(18, max(0, n["pitch"] - 48)), n["start"]))
        voices.append(note)
    drum = best(drums, lambda n: (n["start"], n["level"]))
    return [*voices, bass, drum]


def registers(n, channel):
    if n is None:
        return b"\0\0\0"
    vol = max(2, min(13, round(16 * math.sqrt(n["level"]))))
    if channel == 3:
        # Noise shift/divider roughly follows percussion register and note pitch.
        poly = 0x53 if n["key"] <= 40 else 0x37 if n["key"] <= 65 else 0x16
        return bytes([poly, 0x80, vol << 4 | 1])
    hz = 440 * 2 ** ((n["pitch"] - 69) / 12)
    freq = min(2047, max(0, round(2048 - (65536 if channel == 2 else 131072) / hz)))
    if channel == 2:
        style = 0x20 if vol >= 10 else 0x40 if vol >= 6 else 0x60
    else:
        # Duty precedes volume in the high register's unused bits.
        duty = n["duty"] if n["kind"] in (1, 2) else (2 if channel == 0 else 1)
        style = vol << 4
        return bytes([freq & 255, (freq >> 8) | duty << 6, style])
    return bytes([freq & 255, freq >> 8, style])


def compile_song(song, max_frames=14400):
    score = render_score(song, max_frames)
    nframes = score["frames"]
    notes = score["events"]
    starts = collections.defaultdict(list)
    stops = collections.defaultdict(list)
    by_track = collections.Counter()
    for i, n in enumerate(notes):
        if n["start"] >= nframes:
            continue
        n["id"] = i
        starts[n["start"]].append(n)
        stops[max(n["start"] + 1, n["end"])].append(i)
        by_track[n["track"]] += 1
    loop = score["repeat"]["start"] if score["repeat"] else 0
    frames = sorted({0, loop, *(t for t in starts if t < nframes), *(t for t in stops if t < nframes)})
    active, changes = {}, []
    current = [-1] * 4
    counts = [0] * 4
    for tick in frames:
        for i in stops[tick]:
            active.pop(i, None)
        for n in starts[tick]:
            active[n["id"]] = n
        voices = choose(active.values())
        mask, params = 0, bytearray()
        for ch, note in enumerate(voices):
            num = note["id"] if note else -1
            if current[ch] != num or tick in (0, loop):
                mask |= 1 << ch
                params.extend(registers(note, ch))
                current[ch] = num
                counts[ch] += note is not None
        if mask:
            changes.append((tick, mask, params))
    stream = bytearray()
    loop_offset = 0
    for i, (tick, mask, args) in enumerate(changes):
        if tick == loop:
            loop_offset = len(stream)
        remaining = (changes[i + 1][0] if i + 1 < len(changes) else nframes) - tick
        stream.extend([min(255, remaining), mask, *args])
        remaining -= 255
        while remaining > 0:
            stream.extend([min(255, remaining), 0])
            remaining -= 255
    stream.append(0)
    wav = next((n["wave"] for n in notes if n["wave"]), None)
    if wav is None:
        wave = [*range(16), *range(15, -1, -1)]
        wav = [(a << 4) | b for a, b in zip(wave[::2], wave[1::2])]
    meta = dict(song=song, source_tracks=score["tracks"], frames=nframes, notes=sum(by_track.values()),
                channel_notes=counts, loop_frame=loop, loop_offset=loop_offset, bytes=len(stream),
                loop_source="MP2K GOTO" if score["repeat"] else "excerpt restart")
    return bytes(stream), bytes(wav), meta
