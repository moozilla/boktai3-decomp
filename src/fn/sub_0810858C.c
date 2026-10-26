// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "ram.h"
struct Frames {
    u16 *data;
    u8 flags, count, multiplier, duration;
    u16 index, padA, rate, timer;
};
struct AnimatedTile {
    u32 flags;
    u8 pad4[3];
    u8 field7;
    u8 pad8[4];
    void *anim;
    u16 field10;
    u8 pad12[0xA];
    s16 x, y, z;
    u8 pad22[0xA];
    struct Frames frames;
    u8 pad3C[0x1C];
    s32 name, field5C;
    u16 field60;
    u8 pad62[2];
    u8 sequence, field65;
};
struct AnimatedTileManager {
    u8 pad0[0x18];
    void *sequences;
};
struct AnimatedTileManager *AnimatedTile_CreateManager(void);
struct AnimatedTile *AnimatedTile_Allocate(struct AnimatedTileManager *);
/* Registry routine 8192: takes a slot of the animated tile manager
   (0x020001BC, created on demand) with sprite 0x3876: -p x y z, -R (2), -r
   sequence, -n, -C, -c palette bank (+0x304); then starts the sequence and
   steps it once (tentative). */
void AnimatedTile_CreateFromScript(void)
{
    struct AnimatedTileManager *manager = gNamedEntryPool;
    struct AnimatedTile *e;
    struct AnimatedTile *slot;
    s32 palette;
    u32 two;
    struct Frames *fr;
    struct Frames *f;
    const vu16 *frame;
    u32 one;
    if (!manager) {
        manager = AnimatedTile_CreateManager();
        if (!manager)
            return;
    }
    slot = AnimatedTile_Allocate(manager);
    if (!slot)
        return;
    e = slot;
    AnimData_Load(&e->frames + 1, 0x3876);
    Sprite_Init(e, &e->frames + 1, 0);
    one = e->field60 = 1;
    e->field65 = 1;
    if (Script_SeekToKeyword('p')) {
        e->x = Script_GetValue();
        e->y = Script_GetValue();
        e->z = Script_GetValue();
    }
    e->field7 = Script_GetKeywordValue('R', 2);
    e->sequence = Script_GetKeywordValue('r', 0);
    e->name = Script_GetKeywordValue('n', 0);
    e->field5C = Script_GetKeywordValue('C', 0);
    palette = Script_GetKeywordValue('c', 0);
    AnimData_SelectPaletteBank(e->anim, palette + 0x304);
    fr = &e->frames;
    Anim_SetSequence(fr, manager->sequences, 0, 0, e->sequence);
    f = fr;
    frame = &f->data[f->index];
    e->field10 = *frame >> 6;
    {
        u32 input = f->flags;
        u32 first = one;
        asm("" : "+r"(first));
        first &= input;
        if (first != (((*frame & 0x30) >> 4) & one))
            e->flags |= 4;
        else
            e->flags &= ~4;
    }
    {
        u32 flags = f->flags;
        register u32 first asm("r1");
        two = 2;
        asm("" : "+r"(two));
        first = two;
        first &= flags;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*frame & 0x30) >> 4) & two))
            e->flags |= 8;
        else
            e->flags &= ~8;
    }
    if (++f->timer >= f->duration) {
        f->timer = 0;
        if (f->flags & 4) {
            u32 index = f->index;
            if (index == 0) {
                if (f->flags & 8) {
                    f->timer = f->duration;
                    goto next;
                }
                index = f->count;
            }
            f->index = index - 1;
        } else if (++f->index >= f->count) {
            if ((u8)(f->flags & 8)) {
                f->timer = f->duration;
                f->index = f->count - 1;
            } else
                f->index = 0;
        }
    next:
        frame = &f->data[f->index];
        f->multiplier = *frame & 0xF;
        f->duration = (f->multiplier * f->rate) >> 6;
        if (!f->duration)
            f->duration = 1;
    }
}
