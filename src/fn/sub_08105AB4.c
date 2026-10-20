#include "global.h"
#ifdef NONMATCHING
// fuzzy: 81.2%
// Pointer lifetimes, instruction ordering and register allocation still differ.
// MATCHING_CONCESSION: register-pinning
struct Frames {
    u16 *data;
    u8 flags, count, multiplier, duration;
    u16 index, pada, rate, timer;
};
struct Render {
    u32 flags;
    u8 pad[12];
    u16 tile;
};
struct Context {
    u8 pad[0x18];
    void *resource;
};
extern struct Context *gUnk_020001B4;
void Anim_SetSequence(struct Frames *, void *, u16, int, int);
void Anim_RestartSequence(struct Frames *, void *, u16, int, int);
s32 Div(s32, s32);
static inline u8 Test(u8 *p, u32 mask)
{
    if (*(u32 *)(p + 0x108) & mask)
        return 1;
    return 0;
}
static inline void Step(struct Render *owner, struct Frames *state)
{
    register struct Render *r asm("r5") = owner;
    register struct Frames *f asm("r4") = state;
    const volatile u16 *word;
    asm("" : "+r"(f), "+r"(r));
    {
        u32 offset = f->index << 1;
        const volatile u16 *table = f->data;
        word = (const volatile u16 *)((u32)table + offset);
    }
    r->tile = *word >> 6;
    {
        int settings = f->flags;
        register int mask asm("r3") = 1;
        register int mine asm("r1");
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
        register int mask asm("r3") = 2;
        register int mine asm("r1");
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
        register u32 mask asm("r6");
        register int duration asm("r2");
        register int savedDuration asm("r3");
        u32 timer = f->timer + 1;
        f->timer = timer;
        {
            register u32 initialMask asm("r1") = 65535;
            asm("" : "+r"(initialMask));
            mask = initialMask;
            asm("" : "+&r"(mask) : "r"(initialMask));
        }
        duration = f->duration;
        timer = (u16)timer;
        savedDuration = duration;
        if (timer >= (u32)duration) {
            register u32 flags asm("r1");
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
                register int count asm("r3");
                index = f->index + 1;
                asm("" : "+r"(index));
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
        update: {
            u32 offset = f->index << 1;
            const volatile u16 *table = f->data;
            word = (const volatile u16 *)((u32)table + offset);
        }
            {
                u32 value = *word;
                u32 low = 15;
                low &= value;
                f->multiplier = low;
            }
            {
                s32 speed = (f->multiplier * f->rate) >> 6;
                f->duration = speed;
                if (!(u8)speed)
                    f->duration = 1;
            }
        }
    }
}
// confidence=low: health-derived animation and recovery; actor identity remains tentative.
void ProximityActor_UpdateDamageAnimation(u8 *input)
{
    register u8 *p asm("r5") = input;
    register struct Context *ctx asm("r8") = gUnk_020001B4;
    u8 *hit;
    u32 *flags;
    if (!ctx)
        return;
    hit = p + 0xEB;
    if ((*hit & 1) || Test(p, 4)) {
        if (*(s16 *)(p + 0xFC) <= 0) {
            struct Frames *f;
            *(u32 *)(p + 0x108) = 0x28;
            f = (struct Frames *)(p + 0x90);
            Anim_SetSequence(f, ctx->resource, 1, 0, 0);
            Step((struct Render *)p, f);
            *(u16 *)(p + 0x106) = 3;
            p[0x10C] = 1;
            *hit = 0;
            asm("" : "+r"(p));
            return;
        } else {
            u32 test = Test(p, 1);
            if (test) {
                u16 *index = (u16 *)(p + 0x98);
                if (*index != 3) {
                    s32 frame = *index;
                    if (frame > 3) {
                        frame = __builtin_abs(frame - 6);
                    }
                    Anim_RestartSequence((struct Frames *)(p + 0x90), ctx->resource, 0, 0, 0);
                    *index = frame;
                }
                *(u32 *)(p + 0x108) = 0x22;
                asm("" : "+r"(p));
                return;
            } else {
                s32 frame;
                struct Frames *f;
                *(u32 *)(p + 0x108) = 0x24;
                frame = Div(*(s16 *)(p + 0xFC), *(s32 *)(p + 0x100));
                asm("" : "+r"(frame));
                frame = 3 - frame;
                f = (struct Frames *)(p + 0x90);
                Anim_SetSequence(f, ctx->resource, 1, 0, test);
                if (*(u16 *)(p + 0x98) < frame)
                    Step((struct Render *)p, f);
            }
        }
    }
    if (!(*hit & 1)) {
        s16 *health = (s16 *)(p + 0xFC);
        s16 *maximum = (s16 *)(p + 0xFE);
        if (*health < *maximum)
            *health += 10;
        else if (Test(p, 4)) {
            Anim_SetSequence((struct Frames *)(p + 0x90), ctx->resource, 1, 0, 4);
            if (Test(p, 4))
                *(u16 *)(p + 0x98) = 3 - Div(*health, *(s32 *)(p + 0x100));
            *health = *maximum;
            *(u32 *)(p + 0x108) = 0x30;
        }
        p[0x10D] = 0;
    }
    *hit = 0;
    asm("" : "+r"(p));
}

#else
INCLUDE_ASM("asm/nonmatching", ProximityActor_UpdateDamageAnimation);
#endif
