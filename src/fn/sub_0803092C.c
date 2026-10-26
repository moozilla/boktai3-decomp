#include "global.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
struct State {
    u8 pad[0x15];
    s8 current;
};
struct Render {
    u32 flags;
    u8 pad[12];
    u16 tile;
};
struct Frames {
    u16 *data;
    u8 flags, count, multiplier, duration;
    u16 index, pada, rate, timer;
};
struct Actor {
    u8 pad0[0x18];
    struct State state;
    u8 pad30[0x152];
    u8 mode, pad183, previousMode, pad185[2], defaultMode;
    u8 pad188[0x14];
    u32 counter;
    u8 pad1a0[0xd0];
    u8 sprite[0x1c];
    struct Render render;
    u8 pad2a0[0x18];
    struct Frames frames;
    void *resource;
    void *callback;
};
void AnimData_SelectPaletteBank(void *, int);
int Anim_SetSequence(struct Frames *, void *, u16, int, int);
int MessageWindow_UpdateSpeakerPortrait(struct Actor *p)
{
    struct State *state = &p->state;
    if (state->current == -1) {
        p->render.flags |= 1;
    } else {
        struct Frames *f;
        struct Frames *initial;
        struct Render *r;
        u16 *word;
        p->render.flags &= ~1;
        {
            register struct State *read asm("r2") = state;
            asm("" : "+r"(read));
            switch ((s8)((u8)read->current - 4)) {
                case 26:
                    AnimData_SelectPaletteBank(p->sprite, 506);
                    break;
                case 16:
                case 21:
                    AnimData_SelectPaletteBank(p->sprite, 504);
                    break;
                case 0:
                case 1:
                case 9:
                    AnimData_SelectPaletteBank(p->sprite, 505);
                    break;
                case 15:
                    AnimData_SelectPaletteBank(p->sprite, 507);
                    break;
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 17:
                case 18:
                case 19:
                case 20:
                case 22:
                case 23:
                case 24:
                case 25:
                default:
                    AnimData_SelectPaletteBank(p->sprite, 503);
                    break;
            }
        }
        initial = &p->frames;
        Anim_SetSequence(initial, p->resource, (u16)state->current, 0, 0);
        r = &p->render;
        f = initial;
        word = f->data + f->index;
        r->tile = *word >> 6;
        {
            int settings = f->flags;
            int mask = 1;
            register int mine asm("r2");
            int bits;
            asm("" : "+r"(mask));
            mine = mask & settings;
            bits = (*word & 48) >> 4;
            bits &= mask;
            if (mine != bits)
                r->flags |= 4;
            else
                r->flags &= ~4;
        }
        {
            int settings = f->flags;
            int mask = 2;
            register int mine asm("r2");
            int bits;
            asm("" : "+r"(mask));
            mine = mask & settings;
            asm("" : "+r"(mine));
            mine = (u8)mine;
            bits = (*word & 48) >> 4;
            bits &= mask;
            if (mine != bits)
                r->flags |= 8;
            else
                r->flags &= ~8;
        }
        {
            u32 mask;
            int duration;
            register int savedDuration asm("r4");
            u32 timer = f->timer + 1;
            f->timer = timer;
            {
                register u32 initialMask asm("r2") = 65535;
                asm("" : "+r"(initialMask));
                mask = initialMask;
            }
            duration = f->duration;
            timer = (u16)timer;
            savedDuration = duration;
            if (timer >= (u32)duration) {
                register u32 flags asm("r2");
                int index;
                f->timer = 0;
                flags = f->flags;
                if (flags & 4) {
                    index = f->index;
                    if (!index) {
                        if (flags & 8) {
                            f->timer = savedDuration;
                            goto update;
                        }
                        index = f->count;
                    }
                    index--;
                    goto storeIndex;
                } else {
                    int count;
                    index = f->index + 1;
                    f->index = index;
                    count = f->count;
                    if ((index & mask) < count)
                        goto update;
                    index = flags & 8;
                    index = (u8)index;
                    if (index) {
                        f->timer = duration;
                        index = count - 1;
                    }
                }
            storeIndex:
                f->index = index;
            update:
                word = f->data + f->index;
                {
                    u32 value = *word;
                    u32 low = 15;
                    low &= value;
                    f->multiplier = low;
                }
                f->duration = (f->multiplier * f->rate) >> 6;
                if (!f->duration)
                    f->duration = 1;
            }
        }
    }
    p->previousMode = (u8)state->current;
}
