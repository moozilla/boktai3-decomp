#include "global.h"
#include "render.h"

// confidence=low: inferred from player cue phases and animation consumers.

// confidence=low: tentative cue role from callback phases.
// confidence=low: caller selects this update for player state five.
struct Sequence {
    const u16 *frames;
    u8 flags, count, duration, ticks;
    u16 index, unused, speed, timer;
};
struct CueSprite {
    u32 flags;
    u8 pad4[12];
    u16 tile;
};
struct CuePlayer {
    u8 pad0[0x30];
    u64 position;
    u8 pad38[0xAC - 0x38];
    u64 displayPosition;
    u8 padB4[0xC6 - 0xB4];
    u16 cueResource;
    u8 padC8[0xD4 - 0xC8];
    const void *palette;
    u8 padD8[0x14C - 0xD8];
    struct CueSprite sprite;
    u8 pad160[0x3FC - 0x160];
    struct Sequence sequence;
    u8 pad40C[8];
    const void *alternateResource;
    u8 pad418[0x444 - 0x418];
    u32 events;
    u32 extraEvents;
    u8 pad44C[0x45A - 0x44C];
    u16 timer;
    u8 pad45C[8];
    u8 facingReady, facingSide;
    u8 pad466[5];
    u8 alternateSequence;
    u8 pad46C[6];
    u8 playerIndex;
    u8 pad473[0x530 - 0x473];
    u16 cueFlag;
    u8 pad532[0x5A3 - 0x532];
    u8 completed;
};
void Sound_PlaySong(u32);
void Player_AddControlFlags(void *, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Anim_SetSequence(void *, void *, s32, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
extern u8 *gGameState;
static inline u32 Event(struct CuePlayer *p, u32 mask)
{
    if (!p)
        return 0;
    {
        u32 *events = &p->events;
        u32 result = mask;
        result &= *events;
        return result;
    }
}
static inline u32 Extra(struct CuePlayer *p, u32 mask)
{
    if (!p)
        return 0;
    {
        u32 *events = &p->extraEvents;
        u32 result = mask;
        result &= *events;
        return result;
    }
}
static inline u32 Arg1(u32 k_, const u16 *frame)
{
    return (((*frame) & 48) >> 4) & k_;
}

static inline u32 Arg2(u32 k_, const u16 *frame)
{
    return (((*frame) & 48) >> 4) & k_;
}

static inline s32 SignEvent(struct CuePlayer *actor)
{
    if (!actor)
        return 0;
    return (s32)actor->events < 0;
}
void Player_UpdateStateMotionCue(struct CuePlayer *p)
{
    if (!(p->timer)) {
        Sound_PlaySong(490);
        if (p->cueFlag)
            p->cueResource = 45;
        else if ((*((s32 *)((gGameState + ((p->playerIndex) * 4)) + 0x628))) > 0)
            p->cueResource = 46;
        else if (Event(p, 0x10000000))
            p->cueResource = 0x179;
        else if (SignEvent(p))
            p->cueResource = 0x175;
        else if (Extra(p, 1))
            p->cueResource = 0x177;
        else if (Event(p, 0x40000000))
            p->cueResource = 0x178;
        else if (Event(p, 0x20000000))
            p->cueResource = 0x176;
        else
            p->cueResource = 51;
    }
    Player_AddControlFlags(p, 64);
    Player_SetAnimation(p, 154, 64, 1);
    p->displayPosition = p->position;
    p->palette = (const void *)(gPaletteBankData + ((p->cueResource) * 32));
    if ((p->timer) <= 9) {
        struct Sequence *s = &p->sequence;
        struct CueSprite *draw;
        const u16 *frame;
        Anim_SetSequence(s, (void *)(p->alternateResource), (p->alternateSequence) + 9, 0, 0);
        draw = &p->sprite;
        frame = s->frames + s->index;
        draw->tile = (*frame) >> 6;
        if ((s->flags & 1) != Arg1(1, frame))
            draw->flags |= 4;
        else
            draw->flags &= ~4;
        if (((u8)(s->flags & 2)) != Arg2(2, frame))
            draw->flags |= 8;
        else
            draw->flags &= ~8;
        if ((++s->timer) >= s->ticks) {
            s->timer = 0;
            if (s->flags & 4) {
                if (!s->index) {
                    if (s->flags & 8) {
                        s->timer = s->ticks;
                    } else {
                        s->index = s->count - 1;
                    }
                } else
                    s->index--;
            } else if ((++s->index) >= s->count) {
                if ((u8)(s->flags & 8)) {
                    s->timer = s->ticks;
                    s->index = s->count - 1;
                } else {
                    s->index = 0;
                }
            }
            frame = s->frames + s->index;

            s->duration = *(const volatile u16 *)frame & 15;
            s->ticks = (s->duration * s->speed) >> 6;
            if (!s->ticks)
                s->ticks = 1;
        }
    } else if (((p->timer) >> 2) & 1)
        Player_AddControlFlags(p, 1);
    if ((++(p->timer)) > 39) {
        p->facingReady = 1;
        p->facingSide = 1;
        PlayerCue_EnterStandUpdate(p);
        p->completed = 1;
    }
}
