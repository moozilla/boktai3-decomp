#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: inferred from player cue phases and animation consumers.

// confidence=low: tentative cue role from callback phases.
// Remaining event mask ordering, resource selection stores and playback differ.
// confidence=low: caller selects this update for player state five.
#define H(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
#define B(o) (*(u8 *)(p + (o)))
struct Sequence {
    const u16 *frames;
    u8 flags, count, duration, ticks;
    u16 index, unused, speed, timer;
};
void Sound_PlaySong(u32);
void Player_AddControlFlags(void *, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Anim_SetSequence(void *, void *, s32, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
extern u8 *gUnk_02000710;
extern u8 *gUnk_030042E4;
static inline u32 FrameValue(const u16 *p)
{
    return *p;
}
static inline u32 Event(u8 *p, u32 mask)
{
    if (!p)
        return 0;
    return *(u32 *)(p + 0x444) & mask;
}
static inline u32 Extra(u8 *p, u32 mask)
{
    if (!p)
        return 0;
    return *(u32 *)(p + 0x448) & mask;
}
static inline u32 Arg1(u32 k_, const u16 *frame)
{
    return (((*frame) & 48) >> 4) & k_;
}

static inline u32 Arg2(u32 k_, const u16 *frame)
{
    return (((*frame) & 48) >> 4) & k_;
}

void Player_UpdateStateMotionCue(u8 *p)
{
    u32 t1;
    if (!(*((u16 *)(p + 0x45A)))) {
        Sound_PlaySong(490);
        if (*((u16 *)(p + 0x530)))
            *((u16 *)(p + 0xC6)) = 45;
        else if ((*((s32 *)((gUnk_02000710 + ((*((u8 *)(p + 0x472))) * 4)) + 0x628))) > 0)
            *((u16 *)(p + 0xC6)) = 46;
        else if (Event(p, 0x10000000))
            *((u16 *)(p + 0xC6)) = (t1 = 0x179);
        else if (p && ((*((s32 *)(p + 0x444))) < 0))
            *((u16 *)(p + 0xC6)) = 0x175;
        else if (Extra(p, 1))
            *((u16 *)(p + 0xC6)) = 0x177;
        else if (Event(p, 0x40000000))
            *((u16 *)(p + 0xC6)) = 0x178;
        else if (Event(p, 0x20000000))
            *((u16 *)(p + 0xC6)) = t1 - 3;
        else
            *((u16 *)(p + 0xC6)) = 51;
    }
    Player_AddControlFlags(p, 64);
    Player_SetAnimation(p, 154, 64, 1);
    *((u64 *)(p + 0xAC)) = *((u64 *)(p + 0x30));
    *((u32 *)(p + 0xD4)) = (u32)(gUnk_030042E4 + ((*((u16 *)(p + 0xC6))) * 32));
    if ((*((u16 *)(p + 0x45A))) <= 9) {
        struct Sequence *s = (void *)(p + 0x3FC);
        struct {
            u32 flags;
            u8 pad[12];
            u16 tile;
        } *draw;
        const u16 *frame;
        Anim_SetSequence(s, (void *)(*((u32 *)(p + 0x414))), (*((u8 *)(p + 0x46B))) + 9, 0, 0);
        draw = (void *)(p + 0x14C);
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
                        goto frame_update;
                    }
                    s->index = s->count - 1;
                } else
                    s->index--;
            } else if ((++s->index) >= s->count) {
                u32 index = 0;
                if ((u8)(s->flags & 8)) {
                    s->timer = s->ticks;
                    index = s->count - 1;
                }
                s->index = index;
            }
        frame_update:
            frame = s->frames + s->index;

            s->duration = FrameValue(frame) & 15;
            s->ticks = (s->duration * s->speed) >> 6;
            if (!s->ticks)
                s->ticks = 1;
        }
    } else if (((*((u16 *)(p + 0x45A))) >> 2) & 1)
        Player_AddControlFlags(p, 1);
    if ((++(*((u16 *)(p + 0x45A)))) > 39) {
        *((u8 *)(p + 0x464)) = 1;
        *((u8 *)(p + 0x465)) = 1;
        PlayerCue_EnterStandUpdate(p);
        *((u8 *)(p + 0x5A3)) = 1;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Player_UpdateStateMotionCue);
#endif
