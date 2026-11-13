/* C replacements for the MP2K sequencer the SDK wrote in assembly (m4a_1.s,
 * ROM 0822F1B4..0822FDB4): MPlayMain, ply_note and the event handlers the
 * jump table at 0824FFDC points to. The mixer (SoundMain) is port/hw/mixer.c.
 * Written from the documented MP2K engine behavior, not from ROM bytes.
 * Handlers without a ROM symbol are bound to their addresses by
 * port/build_game.py (PORT_CODE), so ROM tables and C reach the same code. */
#include "global.h"
#include "gba/m4a_internal.h"

void port_bus_store_i8(volatile void *p, u8 value);

#define TEMPLATE_ROM 0x0824FFDC
#define SOUND_INFO (*(struct SoundInfo **)0x03007FF0)

/* MP2K note lengths in ticks (the engine's standard clock table). */
static const u8 sClockTable[49] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 28, 30, 32, 36, 40, 42, 44, 48, 52, 54, 56, 60, 64, 66, 68, 72, 76, 78,
    80, 84, 88, 90, 92, 96,
};

u32 MidiKeyToFreq(struct WaveData *wav, u8 key, u8 fineAdjust);
void ClearChain(void *x);
void ClearModM(struct MusicPlayerTrack *track);

void MPlayJumpTableCopy(MPlayFunc *table)
{
    const MPlayFunc *src = (const MPlayFunc *)TEMPLATE_ROM;
    int i;
    for (i = 0; i < 36; i++) table[i] = src[i];
}

/* Jump-table entry 35; the C Clear64byte (m4a.c) calls it through the table. */
void RealClear64byte(void *addr)
{
    u32 *p = addr;
    int i;
    for (i = 0; i < 16; i++) p[i] = 0;
}

void RealClearChain(void *x)
{
    struct SoundChannel *chan = x;
    struct MusicPlayerTrack *track = chan->track;
    struct SoundChannel *next, *prev;
    if (!track) return;
    next = chan->nextChannelPointer;
    prev = chan->prevChannelPointer;
    if (prev) prev->nextChannelPointer = next;
    else track->chan = next;
    if (next) next->prevChannelPointer = prev;
    chan->track = NULL;
}

static u8 next_byte(struct MusicPlayerTrack *track) { return *track->cmdPtr++; }

static u32 read_word(const u8 *p) { return p[0] | (p[1] << 8) | (p[2] << 16) | ((u32)p[3] << 24); }

/* 0xB1 FINE: end of track. */
void ply_fine(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    struct SoundChannel *chan;
    (void)player;
    for (chan = track->chan; chan; chan = chan->nextChannelPointer) {
        if (chan->statusFlags & SOUND_CHANNEL_SF_ON) chan->statusFlags |= SOUND_CHANNEL_SF_STOP;
        ClearChain(chan);
    }
    track->flags = 0;
}

void ply_goto(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    track->cmdPtr = (u8 *)read_word(track->cmdPtr);
}

void ply_patt(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    if (track->patternLevel >= 3) {
        ply_fine(player, track);
        return;
    }
    track->patternStack[track->patternLevel++] = track->cmdPtr + 4;
    ply_goto(player, track);
}

void ply_pend(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    if (track->patternLevel) track->cmdPtr = track->patternStack[--track->patternLevel];
}

void ply_rept(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    u8 count = *track->cmdPtr;
    if (count == 0) {
        track->cmdPtr++;
        ply_goto(player, track);
        return;
    }
    track->repN++;
    track->cmdPtr++;
    if (track->repN < count) {
        ply_goto(player, track);
    } else {
        track->repN = 0;
        track->cmdPtr += 4;
    }
}

void ply_prio(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    track->priority = next_byte(track);
}

void ply_tempo(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    player->tempoD = next_byte(track) * 2;
    player->tempoI = (player->tempoD * player->tempoU) >> 8;
}

void ply_keysh(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    track->keyShift = next_byte(track);
    track->flags |= MPT_FLG_PITCHG;
}

void ply_voice(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    const struct ToneData *tone = &player->tone[next_byte(track)];
    track->tone = *tone;
}

void ply_vol(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    track->vol = next_byte(track);
    track->flags |= MPT_FLG_VOLCHG;
}

void ply_pan(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    track->pan = next_byte(track) - C_V;
    track->flags |= MPT_FLG_VOLCHG;
}

void ply_bend(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    track->bend = next_byte(track) - C_V;
    track->flags |= MPT_FLG_PITCHG;
}

void ply_bendr(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    track->bendRange = next_byte(track);
    track->flags |= MPT_FLG_PITCHG;
}

void ply_lfos(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    track->lfoSpeed = next_byte(track);
    if (!track->lfoSpeed) ClearModM(track);
}

void ply_lfodl(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    track->lfoDelay = next_byte(track);
}

void ply_mod(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    track->mod = next_byte(track);
    if (!track->mod) ClearModM(track);
}

void ply_modt(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    u8 type = next_byte(track);
    (void)player;
    if (track->modT != type) {
        track->modT = type;
        ClearModM(track);
    }
}

void ply_tune(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    (void)player;
    track->tune = next_byte(track) - C_V;
    track->flags |= MPT_FLG_PITCHG;
}

void ply_port(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    u8 reg = next_byte(track);
    u8 value = next_byte(track);
    (void)player;
    port_bus_store_i8((vu8 *)(REG_BASE + 0x60 + reg), value);
}

/* 0xCE EOT: release the tied note (key optional). */
void ply_endtie(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    struct SoundChannel *chan;
    u8 key;
    (void)player;
    if (*track->cmdPtr < 0x80) track->key = next_byte(track);
    key = track->key;
    for (chan = track->chan; chan; chan = chan->nextChannelPointer) {
        if ((chan->statusFlags & (SOUND_CHANNEL_SF_START | SOUND_CHANNEL_SF_ENV)) &&
            !(chan->statusFlags & SOUND_CHANNEL_SF_STOP) && chan->midiKey == key) {
            chan->statusFlags |= SOUND_CHANNEL_SF_STOP;
            return;
        }
    }
}

void TrackStop(struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    struct SoundChannel *chan;
    (void)player;
    if (!(track->flags & MPT_FLG_EXIST)) return;
    for (chan = track->chan; chan; chan = chan->nextChannelPointer) {
        if (chan->statusFlags) {
            if ((chan->type & 7) && SOUND_INFO->CgbOscOff) SOUND_INFO->CgbOscOff(chan->type & 7);
            chan->statusFlags = 0;
        }
        chan->track = NULL;
    }
    track->chan = NULL;
}

static void channel_volume(struct SoundChannel *chan, struct MusicPlayerTrack *track)
{
    s32 pan = (s8)chan->rhythmPan;
    s32 right = (((0x80 + pan) * chan->velocity) * track->volMR) >> 14;
    s32 left = (((0x7f - pan) * chan->velocity) * track->volML) >> 14;
    chan->rightVolume = right > 0xff ? 0xff : right;
    chan->leftVolume = left > 0xff ? 0xff : left;
}

/* 0xCF TIE and 0xD0..0xFF notes: n = command - 0xCF. */
void m4a_Unk_0822FB18(u32 n, struct MusicPlayerInfo *player, struct MusicPlayerTrack *track)
{
    struct SoundInfo *sound = SOUND_INFO;
    struct ToneData *tone = &track->tone;
    struct SoundChannel *chan = NULL;
    u8 type, key, cgb;
    s32 rhythmPan = 0, priority, transposed;

    track->gateTime = sClockTable[n & 0xff];
    if (*track->cmdPtr < 0x80) {
        track->key = next_byte(track);
        if (*track->cmdPtr < 0x80) {
            track->velocity = next_byte(track);
            if (*track->cmdPtr < 0x80) track->gateTime += next_byte(track);
        }
    }
    key = track->key;
    type = tone->type;
    if (type & (TONEDATA_TYPE_RHY | TONEDATA_TYPE_SPL)) {
        u32 index = key;
        struct ToneData *sub;
        if (type & TONEDATA_TYPE_SPL) index = (*(u8 **)&tone->attack)[key];
        sub = (struct ToneData *)tone->wav + index;
        if (sub->type & (TONEDATA_TYPE_RHY | TONEDATA_TYPE_SPL)) return;
        if (type & TONEDATA_TYPE_RHY) {
            if (sub->pan_sweep & 0x80) rhythmPan = ((s32)(sub->pan_sweep) - 0xc0) * 2;
            key = sub->key;
        }
        tone = sub;
        type = sub->type;
    }
    priority = player->priority + track->priority;
    if (priority > 0xff) priority = 0xff;
    cgb = type & TONEDATA_TYPE_CGB;
    if (cgb) {
        struct CgbChannel *c;
        if (!sound->cgbChans) return;
        c = &sound->cgbChans[cgb - 1];
        if ((c->statusFlags & SOUND_CHANNEL_SF_ON) && !(c->statusFlags & SOUND_CHANNEL_SF_STOP)) {
            if (c->priority > priority) return;
            if (c->priority == priority && (u32)c->track < (u32)track) return;
        }
        chan = (struct SoundChannel *)c;
    } else {
        struct SoundChannel *best = NULL;
        s32 bestPriority = priority;
        struct MusicPlayerTrack *bestTrack = track;
        int stopped = 0, i;
        for (i = 0; i < sound->maxChans; i++) {
            struct SoundChannel *c = &sound->chans[i];
            if (!(c->statusFlags & SOUND_CHANNEL_SF_ON)) {
                best = c;
                break;
            }
            if (c->statusFlags & SOUND_CHANNEL_SF_STOP) {
                if (!stopped || c->priority < bestPriority ||
                    (c->priority == bestPriority && (u32)c->track > (u32)bestTrack)) {
                    stopped = 1;
                    best = c;
                    bestPriority = c->priority;
                    bestTrack = c->track;
                }
            } else if (!stopped) {
                if (c->priority < bestPriority ||
                    (c->priority == bestPriority && (u32)c->track >= (u32)bestTrack)) {
                    best = c;
                    bestPriority = c->priority;
                    bestTrack = c->track;
                }
            }
        }
        if (!best) return;
        chan = best;
    }
    ClearChain(chan);
    chan->prevChannelPointer = NULL;
    chan->nextChannelPointer = track->chan;
    if (track->chan) track->chan->prevChannelPointer = chan;
    track->chan = chan;
    chan->track = track;
    track->lfoDelayC = track->lfoDelay;
    if (track->lfoDelay) ClearModM(track);
    TrkVolPitSet(player, track);
    chan->gateTime = track->gateTime;
    chan->midiKey = track->key;
    chan->velocity = track->velocity;
    chan->priority = priority;
    chan->key = key;
    chan->rhythmPan = rhythmPan;
    chan->type = tone->type;
    chan->wav = tone->wav;
    chan->attack = tone->attack;
    chan->decay = tone->decay;
    chan->sustain = tone->sustain;
    chan->release = tone->release;
    chan->pseudoEchoVolume = track->pseudoEchoVolume;
    chan->pseudoEchoLength = track->pseudoEchoLength;
    channel_volume(chan, track);
    transposed = key + (s8)track->keyM;
    if (transposed < 0) transposed = 0;
    if (cgb) {
        struct CgbChannel *c = (struct CgbChannel *)chan;
        c->length = tone->length;
        c->sweep = ((tone->pan_sweep & 0x80) || !(tone->pan_sweep & 0x70)) ? 8 : tone->pan_sweep;
        c->frequency = sound->MidiKeyToCgbFreq(cgb, transposed, track->pitM);
    } else {
        chan->count = track->unk_3C;
        chan->frequency = MidiKeyToFreq(tone->wav, transposed, track->pitM);
    }
    chan->statusFlags = SOUND_CHANNEL_SF_START;
    track->flags &= 0xf0;
}

void MPlayMain(struct MusicPlayerInfo *player)
{
    struct SoundInfo *sound;
    u32 tempo;
    if (player->ident != ID_NUMBER) return;
    player->ident++;
    if (player->MPlayMainNext) player->MPlayMainNext(player->musicPlayerNext);
    if ((s32)player->status < 0) goto unlock;
    sound = SOUND_INFO;
    FadeOutBody(player);
    if ((s32)player->status < 0) goto unlock;
    tempo = player->tempoC + player->tempoI;
    while (tempo >= 150) {
        struct MusicPlayerTrack *track = player->tracks;
        u32 active = 0, bit = 1;
        int i;
        for (i = 0; i < player->trackCount; i++, track++, bit <<= 1) {
            struct SoundChannel *chan;
            if (!(track->flags & MPT_FLG_EXIST)) continue;
            active |= bit;
            for (chan = track->chan; chan; chan = chan->nextChannelPointer) {
                if (!(chan->statusFlags & SOUND_CHANNEL_SF_ON)) ClearChain(chan);
                else if (chan->gateTime && --chan->gateTime == 0)
                    chan->statusFlags |= SOUND_CHANNEL_SF_STOP;
            }
            if (track->flags & MPT_FLG_START) {
                Clear64byte(track);
                track->flags = MPT_FLG_EXIST;
                track->bendRange = 2;
                track->volX = 64;
                track->lfoSpeed = 22;
                track->tone.type = 1;
            }
            while (!track->wait) {
                u8 cmd = *track->cmdPtr;
                if (cmd < 0x80) {
                    cmd = track->runningStatus;
                } else {
                    track->cmdPtr++;
                    if (cmd >= 0xbd) track->runningStatus = cmd;
                }
                if (cmd >= 0xcf) {
                    sound->plynote(cmd - 0xcf, player, track);
                } else if (cmd > 0xb0) {
                    player->cmd = cmd - 0xb1;
                    sound->MPlayJumpTable[cmd - 0xb1](player, track);
                    if (!track->flags) goto next_track;
                } else {
                    track->wait = sClockTable[cmd - 0x80];
                }
            }
            track->wait--;
            if (track->lfoSpeed && track->mod) {
                if (track->lfoDelayC) {
                    track->lfoDelayC--;
                } else {
                    s32 wave;
                    s8 mod;
                    track->lfoSpeedC += track->lfoSpeed;
                    wave = (s8)(track->lfoSpeedC - 0x40) < 0 ? (s8)track->lfoSpeedC
                                                             : 0x80 - track->lfoSpeedC;
                    mod = (s8)((track->mod * wave) >> 6);
                    if (mod != track->modM) {
                        track->modM = mod;
                        track->flags |= track->modT ? MPT_FLG_VOLCHG : MPT_FLG_PITCHG;
                    }
                }
            }
        next_track:;
        }
        player->clock++;
        if (!active) {
            player->status = MUSICPLAYER_STATUS_PAUSE;
            goto unlock;
        }
        player->status = active;
        tempo -= 150;
    }
    player->tempoC = tempo;
    {
        struct MusicPlayerTrack *track = player->tracks;
        int i;
        for (i = 0; i < player->trackCount; i++, track++) {
            struct SoundChannel *chan;
            if (!(track->flags & MPT_FLG_EXIST) || !(track->flags & 0xf)) continue;
            TrkVolPitSet(player, track);
            for (chan = track->chan; chan; chan = chan->nextChannelPointer) {
                u8 cgb;
                if (!(chan->statusFlags & SOUND_CHANNEL_SF_ON)) {
                    ClearChain(chan);
                    continue;
                }
                cgb = chan->type & 7;
                if (track->flags & MPT_FLG_VOLCHG) {
                    channel_volume(chan, track);
                    if (cgb) ((struct CgbChannel *)chan)->modify |= CGB_CHANNEL_MO_VOL;
                }
                if (track->flags & MPT_FLG_PITCHG) {
                    s32 key = chan->key + (s8)track->keyM;
                    if (key < 0) key = 0;
                    if (cgb) {
                        struct CgbChannel *c = (struct CgbChannel *)chan;
                        c->frequency = sound->MidiKeyToCgbFreq(cgb, key, track->pitM);
                        c->modify |= CGB_CHANNEL_MO_PIT;
                    } else {
                        chan->frequency = MidiKeyToFreq(chan->wav, key, track->pitM);
                    }
                }
            }
            track->flags &= 0xf0;
        }
    }
unlock:
    player->ident = ID_NUMBER;
}
