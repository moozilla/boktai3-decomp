#include "global.h"
#include "display_animation.h"
struct AnimatedTile {
    u32 flags;
    u8 pad4[0xc];
    u16 frame;
    u8 pad12[0x1a];
    struct DisplaySequenceState animation;
    u8 pad3c[0x24];
    u16 state;
    u8 pad62[2];
    u8 sequence, entered;
};
struct AnimatedTileManager {
    u8 pad0[0x18];
    void *sequences;
};
void Sound_PlaySong(u32);
static inline u32 Step(struct AnimatedTile *crate)
{
    struct DisplaySequenceState *a = &crate->animation;
    const volatile u16 *entry;
    u32 done;
    {
        u32 offset = a->index * 2;
        const u16 *table = crate->animation.commands;
        entry = (const volatile u16 *)((u32)table + offset);
    }
    crate->frame = *entry >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            crate->flags |= 4;
        else
            crate->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = a->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*entry & 0x30) >> 4) & bit))
            crate->flags |= 8;
        else
            crate->flags &= ~8;
    }
    if (++a->elapsed >= a->duration) {
        a->elapsed = 0;
        if (a->flags & 4) {
            if (a->index == 0) {
                if (a->flags & 8)
                    a->elapsed = a->duration;
                else
                    a->index = a->frameCount - 1;
                done = 1;
            } else {
                --a->index;
                done = 0;
            }
        } else {
            if (++a->index >= a->frameCount) {
                if ((u8)(a->flags & 8)) {
                    a->elapsed = a->duration;
                    a->index = a->frameCount - 1;
                } else
                    a->index = 0;
                done = 1;
            } else
                done = 0;
        }
        entry = a->commands + a->index;
        a->frameWeight = *entry & 15;
        a->duration = (a->frameWeight * a->speed) >> 6;
        if (!a->duration)
            a->duration = 1;
    } else
        done = 0;
    return done;
}

void AnimatedTile_TickAlternateAnimation(struct AnimatedTileManager *manager,
                                         struct AnimatedTile *tile)
{
    if (tile->entered) {
        Sound_PlaySong(0x10f);
        Anim_SetSequence(&tile->animation, manager->sequences, 5, 0, tile->sequence);
        tile->entered = 0;
    }
    if (Step(tile)) {
        tile->state = 3;
        tile->entered = 1;
    }
}
