#include "global.h"
#include "ram.h"
#include "display_sequence_inline.h"
struct State {
    u8 pad[0x15];
    s8 current;
};
struct Render {
    u32 flags;
    u8 pad[12];
    u16 tile;
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
    struct DisplaySequenceState frames;
    void *resource;
    void *callback;
};
void AnimData_SelectPaletteBank(void *, int);
int Anim_SetSequence(struct DisplaySequenceState *, void *, u16, int, int);
int MessageWindow_UpdateSpeakerPortrait(struct Actor *p)
{
    struct State *state = &p->state;
    if (state->current == -1) {
        p->render.flags |= 1;
    } else {
        struct DisplaySequenceState *f;
        struct DisplaySequenceState *initial;
        struct Render *r;
        const u16 *word;
        p->render.flags &= ~1;
        {
            switch ((s8)(state->current - 4)) {
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
        word = f->commands + f->index;
        r->tile = *word >> 6;
        {
            union DisplaySequenceControl control;
            u32 bit, first;
            control.raw = f->flags;
            bit = 1;
            first = (u8)(control.raw & bit);
            if (first != (((*word & 0x30) >> 4) & bit))
                r->flags |= 4;
            else
                r->flags &= ~4;
        }
        {
            union DisplaySequenceControl control;
            u32 bit, first;
            control.raw = f->flags;
            bit = 2;
            first = (u8)(control.raw & bit);
            if (first != (((*word & 0x30) >> 4) & bit))
                r->flags |= 8;
            else
                r->flags &= ~8;
        }
        if (++f->elapsed >= f->duration) {
            DisplaySequence_SelectNext(f);
            word = f->commands + f->index;
            DisplaySequence_UpdateWeight(f, word);
            if (f->duration == 0)
                f->duration = 1;
        }
    }
    p->previousMode = (u8)state->current;
}
