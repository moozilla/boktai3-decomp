#include "global.h"
#include "render.h"
// MATCHING_CONCESSION: register-pinning
struct VanargandPart {
    u8 pad0[0x10];
    u16 state;
};
struct Vanargand {
    u8 pad0[0x54];
    u8 motionData[0x90 - 0x54];
    u32 actionTimer;
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[0xAF - 0xA8];
    u8 finished, entered, pendingAnimation, ready;
    u8 padB3[0x18C - 0xB3];
    u8 animation[0x60];
    u8 drawable[0x3FC - 0x1EC];
    struct VanargandPart part;
    u8 pad410[0x500 - 0x410];
    u16 partActive;
    u8 pad502[0x534 - 0x502];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x56C - 0x55E];
    void *attachment;
    u8 pad570[0x584 - 0x570];
    u16 pairedMotion;
    u8 pad586[0x59E - 0x586];
    u16 thresholds[3];
    u32 hitFlags;
    s16 damage;
    u8 pad5AA[2];
    s16 accumulatedDamage;
    u16 flashTimer;
    u16 delay;
    u8 pad5B2[0x5B8 - 0x5B2];
    u16 field5B8, field5BA;
    u8 pad5BC[0x718 - 0x5BC];
    u8 palette[0x918 - 0x718];
    u8 fadeA[0xB18 - 0x918];
    u8 fadeB[0xD18 - 0xB18];
    u8 fadeC[0xF24 - 0xD18];
    u32 variant;
    u8 padF28[0xF34 - 0xF28];
    s32 transition;
    u8 padF38[0xF44 - 0xF38];
    void *afterimages;
};
void Sound_PlaySong(u32);
void Vanargand_ResetPaletteTransition(struct Vanargand *);
void CpuSet(const void *, void *, u32);
extern u16 gUnk_03004FC0[];

#define LERP(dst, t, from, to, tag)                                                                \
    {                                                                                              \
        if ((t) > 0)                                                                               \
            goto tag##Upper;                                                                       \
        dst = (from);                                                                              \
        goto tag##Store;                                                                           \
        tag##Upper:                                                                                \
        {                                                                                          \
            s32 limit = 3;                                                                         \
            if ((t) < limit)                                                                       \
                goto tag##Interp;                                                                  \
        }                                                                                          \
        dst = (to);                                                                                \
        goto tag##Store;                                                                           \
        tag##Interp:                                                                               \
        {                                                                                          \
            s32 k = ((to) - (from)) << 8;                                                          \
            s32 v = (t) * k;                                                                       \
            s32 d = 3;                                                                             \
            s32 result;                                                                            \
            v = v / d;                                                                             \
            if (v >= 0)                                                                            \
                result = v >> 8;                                                                   \
            else                                                                                   \
                result = -(-v >> 8);                                                               \
            dst = result;                                                                          \
            dst += (from);                                                                         \
        }                                                                                          \
        tag##Store :;                                                                              \
    }

#define FIELD(type, p, offset) (*(type *)((u8 *)(p) + (offset)))
void Vanargand_StopActionSequence(void *);
static inline void restorePalette(struct Vanargand *p, struct Vanargand *self)
{
    s32 kind = 0x6F;
    if (!p->variant)
        kind = 0x7F;
    switch (kind) {
        case 0x6F: {
            s32 t;
            s32 r, g, b;

            CpuSet(gPaletteBankData + 0xDE0, p->palette, 0x04000020);
            CpuSet(p->fadeA, gUnk_03004FC0, 0x04000068);
            t = p->transition;
            LERP(r, t, 7, 0x13, r1)
            LERP(g, t, 0, 0x12, g1)
            LERP(b, t, 0, 0x13, b1)
            r |= g << 5;
            r |= b << 10;
            {
                u16 color = r;
                u16 *pal = gUnk_03004FC0;
                pal[0x56 / 2] = color;
                pal[0x10C / 2] = color;
            }
            LERP(r, t, 0x15, 0x18, r2)
            LERP(g, t, 0, 0x1D, g2)
            LERP(b, t, 0, 0x1F, b2)
            r |= g << 5;
            r |= b << 10;
            {
                u16 *pal;
                u32 color = (u16)r;
                // Complete packing before materializing the second palette address.
                asm("" : : "r"(color));
                pal = gUnk_03004FC0;
                pal[0x5A / 2] = color;
                pal[0x10E / 2] = color;
            }
            break;
        }
        case 0x82:
            CpuSet(gPaletteBankData + 0x1040, p->palette, 0x04000020);
            CpuSet(p->fadeB, gUnk_03004FC0, 0x04000068);
            break;
        case 0x7F:
            CpuSet(gPaletteBankData + 0xFE0, p->palette, 0x04000018);
            CpuSet(p->fadeC, gUnk_03004FC0, 0x04000068);
            break;
        default:
            CpuSet(gPaletteBankData + 0x3240, self->palette, 0x04000008);
            break;
    }
    Vanargand_ResetPaletteTransition(self);
    Vanargand_StopActionSequence((u8 *)self + 0x21C);
}
static inline void changeState(struct Vanargand *p, s32 state, void *callback)
{
    struct Vanargand *self = p;
    // Keep the transition owner distinct from the callback installation owner.
    if (FIELD(u8, p, 0xAB) == 3)
        restorePalette(p, self);
    {
        u32 zero = 0;
        FIELD(u8, p, 0xAB) = state;
        FIELD(void *, p, 0x100) = callback;
        FIELD(u32, p, 0x90) = zero;
        p->pendingAnimation = 1;
        p->phase = zero;
        p->timer = zero;
    }
}

static inline void setFlags(struct Vanargand *p, u32 mask)
{
    p->flags |= mask;
}
static inline void setPosition(s16 *dst, s32 x, s32 y, s32 z)
{
    s16 *first = dst;
    *first = x;
    dst[1] = y;
    dst[2] = z;
}
static inline u32 partFlags(struct Vanargand *p)
{
    return FIELD(u16, p, 0x4FA);
}
// confidence=low: distinguishes accumulated body damage from paired-part contact damage.
void DamageNumberDisplay_ShowWithDefaultScale(void *, s32, s32, void *, s32);
s32 ScenePair_CountPositiveCounters(void *);
void Vanargand_UpdatePaletteLeap(void *, s32);
s32 Vanargand_ApplyPendingDamage(struct Vanargand *p)
{
    struct Vanargand *self = p;
    void *display = (u8 *)p + 0x6C4;
    s32 damage = p->damage;
    u32 special = (p->hitFlags >> 6) & 1;
    register s32 partDamage asm("sl");
    u32 partSpecial;
    p->damage = 0;
    p->hitFlags = 0;
    if (FIELD(u32, p, 0x4FC) & 6) {
        partDamage = FIELD(u16, p, 0x4F8);
        partSpecial = (partFlags(p) >> 6) & 1;
    } else {
        partDamage = 0;
        partSpecial = 0;
    }
    asm("" : "+r"(partDamage));
    if (damage > 0 || special) {
        p->health -= damage;
        {
            s16 *accumulated = &self->accumulatedDamage;
            asm("" : "+r"(accumulated));
            *accumulated += damage;
        }
        DamageNumberDisplay_ShowWithDefaultScale(display, damage, 0, (u8 *)self + 0x1AC, 1);
        self->flashTimer = 15;
        CpuSet(gPaletteBankData + 0x3240, self->palette, 0x04000008);
        Vanargand_ResetPaletteTransition(self);
        {
            u32 song = 0x13E;
            if (special)
                song += 0xD3;
            Sound_PlaySong(song);
        }
        if (p->health <= 0) {
            p->health = 0;
            setFlags(p, 2);
            return 1;
        }
        {
            s32 accumulated = self->accumulatedDamage;
            s32 transition = self->transition;
            if (transition > 2)
                transition = 2;
            if (accumulated >= self->thresholds[transition]) {
                s32 count = ScenePair_CountPositiveCounters(self->attachment);
                if (count == 0) {
                    {
                        register u32 mask asm("r2") = 1;
                        setFlags(p, mask);
                    }
                    self->accumulatedDamage = count;
                    return 1;
                } else {
                    s32 transition = self->transition;
                    if (transition > 2)
                        transition = 2;
                    self->accumulatedDamage = self->thresholds[transition] - 1;
                }
            } else {
                self->delay += 6;
            }
        }
    }
    if (partDamage > 0 || partSpecial) {
        s16 pos[3];
        p->health -= partDamage;
        setPosition(pos, FIELD(s16, self, 0x418) + 64, FIELD(s16, self, 0x41A),
                    FIELD(s16, self, 0x41C) + 64);
        DamageNumberDisplay_ShowWithDefaultScale((u8 *)self + 0x4A4, partDamage, 0, pos, 1);
        if (p->health <= 0) {
            setFlags(p, 2);
        } else if (FIELD(u32, self, 0x4FC) & 4) {
            changeState(p, 20, Vanargand_UpdatePaletteLeap);
            setFlags(p, 0x10000);
            return 1;
        }
    }
    return 0;
}
