// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 98.8%
// Remaining difference: the zero index register in the second sine lookup (r2 vs r3).

struct SunDiscVec {
    u16 x, y, z, pad;
};
struct SunDiscDelta {
    s16 x, y, z, pad;
};
struct SunDiscSprite {
    u32 flags;
    u8 active, unknown5, unknown6, priority;
    u8 pad8[0x14];
    struct SunDiscVec position;
    u8 pad24[8];
    u8 anim[0x58];
    struct SunDiscVec target;
    s16 velX, velY, velZ, pad92;
};

extern u8 *gUnk_0200017C;
extern const s16 gUnk_086149C4[];
u8 *SunDiscPool_GetOrCreate(void);
u32 *SunDiscPool_AllocSprite(u8 *);
void Sprite_Init(void *, void *, s32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, s32);
s32 Math_Atan2(s32, s32);
void sub_08129854(void *, s32, s32, s32, s32);

static inline s32 TruncateShift(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -(-value >> shift);
}

void SunDiscPool_SpawnDisc(const struct SunDiscVec *fromArg, const struct SunDiscVec *toArg,
                           s32 aArg, s32 bArg, s32 c, s32 d, u32 e, s32 speed)
{
    register s32 a asm("sl") = aArg;
    const struct SunDiscVec *to = toArg;
    s32 b = bArg;
    register u32 zero asm("r6");
    register struct SunDiscSprite *sprite asm("r8");
    register u8 *pool asm("r5") = gUnk_0200017C;
    register struct SunDiscSprite *s asm("r4");
    struct SunDiscDelta delta;
    s32 angle;
    if (!pool) {
        pool = SunDiscPool_GetOrCreate();
        if (!pool)
            return;
    }
    zero = *(u32 *)(pool + 0xC0C);
    if (zero != 0)
        return;
    s = (struct SunDiscSprite *)SunDiscPool_AllocSprite(pool);
    if (!s)
        return;
    *((u8 *)s + 0xF2) = 1;
    *(u16 *)((u8 *)s + 0xF0) = zero;
    *((u8 *)s + 0xF9) = e;
    {
        struct SunDiscVec *t = &s->target;
        *t = *to;
    }
    *((u8 *)s + 0xF8) = 1;
    sprite = s;
    Sprite_Init(s, pool + 0x18, 0);
    Anim_SetSequence((u8 *)s + 0x2C, *(void **)(pool + 0x34), 0, 0, zero);
    AnimData_SelectPaletteBank(*(void **)((u8 *)s + 0xC), 1);
    {
        register const struct SunDiscVec *src asm("r2") = fromArg;
        s->position = *src;
    }
    {
        struct SunDiscDelta *out = &delta;
        struct SunDiscVec *own = &s->position;
        out->x = to->x - s->position.x;
        out->y = to->y - own->y;
        out->z = to->z - own->z;
        angle = Math_Atan2(out->x, out->z);
    }
    {
        s32 sine = gUnk_086149C4[(angle + 0x40) & 0xFF];
        register s32 spd asm("r2") = speed;
        asm("" : "+r"(spd) : "r"(sine));
        s->velX = ({
            register s32 vx asm("r1") = TruncateShift(spd * sine, 12);
            vx;
        });
    }
    {
        s32 sine = gUnk_086149C4[angle & 0xFF];
        s->velZ = TruncateShift(sine * ({
                                    register s32 t asm("r1") = speed;
                                    asm("" : "+r"(t) : "r"(sine));
                                    t;
                                }),
                                12);
    }
    *(u16 *)((u8 *)s + 0x8E) = 0;
    {
        struct SunDiscSprite *t = sprite;
        t->flags |= 1;
        t->priority = 2;
    }
    sub_08129854(s, a, b, c, d);
}
#else
INCLUDE_ASM("asm/nonmatching", SunDiscPool_SpawnDisc);
#endif
