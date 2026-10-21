#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Frames {
    u16 *data;
    u8 flags, count, multiplier, duration;
    u16 index, pad, rate, timer;
};
struct Render {
    u32 flags;
    u8 pad[12];
    u16 tile;
};
void AnimData_Load(void *, u32);
void *ScriptProc_Start(u32, u32);
void Sprite_Init(void *, void *, u32);
int Anim_SetSequence(struct Frames *, void *, u32, int, int);
static inline void step(struct Frames *f, struct Render *r)
{
    u16 *word = f->data + f->index;
    r->tile = *word >> 6;
    {
        int settings = f->flags;
        register int mask asm("r3") = 1;
        register int mine asm("r1");
        int bits;
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
        register int mask asm("r3") = 2;
        register int mine asm("r1");
        int bits;
        mine = mask & settings;
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
        register int savedDuration asm("r3");
        u32 timer = f->timer + 1;
        f->timer = timer;
        {
            register u32 initialMask asm("r1") = 65535;
            asm("" : "+r"(initialMask));
            mask = initialMask;
        }
        duration = f->duration;
        timer = (u16)timer;
        savedDuration = duration;
        if (timer >= (u32)duration) {
            register int flags asm("r1");
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
                index = (u8)(flags & 8);
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

// confidence=low: script body initialization installs this auxiliary sprite and animation.
struct Vec {
    s16 x, y, z;
};
void KeyframeActor_InitAuxiliarySprite(u8 *arg)
{
    u8 *p = arg;
    void *data = p + 0x198;
    void *sequence;
    struct Render *render;
    u32 zero;
    struct Frames *frames;
    AnimData_Load(data, 0xD495);
    sequence = ScriptProc_Start(0x922E, 0xAF44);
    *(void **)(p + 0x410) = sequence;
    render = (void *)(p + 0x14C);
    *(void **)(p + 0x178) = data;
    zero = 0;
    Sprite_Init(render, data, 0x1001);
    {
        u32 value = *(u16 *)(p + 0x30);
        *(s16 *)(p + 0x168) = value;
        {
            u32 y = *(u16 *)(p + 0x32);
            *(s16 *)(p + 0x16A) = y;
        }
        value = *(u16 *)(p + 0x34);
        *(s16 *)(p + 0x16C) = value;
    }
    *(u16 *)(p + 0x4E8) = zero;
    frames = (void *)(p + 0x3FC);
    Anim_SetSequence(frames, sequence, 0, p[0x3A2], p[0x3A3]);
    step(frames, render);
}
