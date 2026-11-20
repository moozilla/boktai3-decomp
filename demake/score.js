// Trace the repository's MP2K sequencer in video-frame time. Only musical
// events cross this boundary; the cartridge synthesizes them with its APU.
'use strict';
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const studio = fs.readFileSync(path.join(__dirname, '../tools/studio/mp2k.js'), 'utf8');
const { Player, tone } = vm.runInNewContext(studio + '\nMP2K', { atob });
const song = JSON.parse(fs.readFileSync(0, 'utf8'));
const limit = Number(process.argv[2]);
const player = new Player(song);
const events = [];
const firstVisit = player.tracks.map(() => new Map());
let frame = 0, repeat = null;

// Reuse the running-status, pattern stack, repeat and tempo implementation.
// tick() expires the gates in this.chans, including tied notes ended by EOT.
player.play = function(track, voice, key, velocity, gate) {
    if (!voice) return;
    const rhythm = Boolean(voice.type & 0x80);
    if (voice.type & 0x40) voice = tone(this.mem, voice.wav + 12 * this.mem.u8(voice.extra + key));
    else if (rhythm) voice = tone(this.mem, voice.wav + 12 * key);
    const event = {
        start: frame, track: track.i, program: track.program, key,
        pitch: key + track.keyShift + track.bend * track.bendRange / 64 + track.tune / 64,
        level: track.vol * velocity / (127 * 127),
        drum: rhythm || (voice.type & 7) === 4,
        kind: voice.type & 7,
        duty: voice.wav & 3,
        wave: (voice.type & 7) === 3 ? Array.from({length: 16}, (_, i) => this.mem.u8(voice.wav + i)) : null,
    };
    events.push(event);
    this.chans.push({ track, key, gate, event, stop: false });
};
const step = player.step;
player.step = function(t) {
    const addr = t.ptr, cmd = this.mem.u8(addr), visits = firstVisit[t.i];
    if (!visits.has(addr)) visits.set(addr, frame);
    if (t.i === 0 && cmd === 0xB2 && repeat === null) {
        const target = this.mem.u32(addr + 1);
        if (target <= addr && visits.has(target)) repeat = { start: visits.get(target), end: frame };
    }
    step.call(this, t);
};
while (frame < limit && repeat === null && player.tracks.some(t => t.on)) {
    player.tempoC += player.tempoI;
    while (player.tempoC >= 150) {
        player.tick();
        player.tempoC -= 150;
    }
    player.chans = player.chans.filter(c => {
        if (!c.stop) return true;
        c.event.end = frame;
        return false;
    });
    frame++;
}
for (const c of player.chans) c.event.end = frame;
process.stdout.write(JSON.stringify({ frames: repeat ? repeat.end : frame, repeat, tracks: player.ntracks, events }));
