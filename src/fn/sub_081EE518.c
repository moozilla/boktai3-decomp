// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: hit callback slows this departure action.
#ifdef NONMATCHING
// fuzzy: 92.0%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: base preservation, second decoder allocation, and phase transitions.
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
int Anim_SetSequence(struct Frames *, void *, u16, int, int);
int Anim_RestartSequence(struct Frames *, void *, u16, int, int);
void Sound_PlaySong(int);
void SceneMover_ApproachScreenX(void *, int);
void ActorDisplay_UpdateScreenAnchor(void *);
void ActorDisplay_UpdateForegroundSprite(void *);
void ActorDisplay_UpdateAccentSprite(void *);
void SceneActors_EmitPeriodicParticles(void *, int);
int SceneMover_StepPastScreenX(void *, int, int);
int Player_GetHorizontalVelocity(void *);
void Player_GetWorldPosition(int, void *);
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define H(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define B(p, o) (*((u8 *)(p) + (o)))
static inline int randomValue(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
    return gUnk_086141BC[gUnk_03005304];
}
static inline void step(struct Frames *f, struct Render *r)
{
    u16 *word = f->data + f->index;
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
        register u32 mask asm("r5");
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
        asm("" : "+r"(savedDuration));
        if (timer >= (u32)duration) {
            int flags;
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
                asm("" : "+r"(flags));
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

// confidence=low: phase meanings inferred from state entry and motion calls.
void SceneActors_UpdateHitSensitiveDeparture(void *arg, int time)
{
    register void *p asm("r8") = arg;
    register int tick asm("sb") = time;
    register void *base asm("r7") = p;
    u8 *enter;
    asm("" : "+r"(p), "+r"(tick), "+r"(base));
    enter = ((u8 *)p) + 0xb1;
    if (*enter) {
        struct Frames *f;
        struct Render *r;
        register int zero asm("r2") = 0;
        *enter = zero;
        asm("" : "+r"(zero));
        r = (void *)(((u8 *)p) + 0x2c0);
        *((s32 *)(((u8 *)p) + 0x298)) = 0;
        *((s32 *)(((u8 *)p) + 0x290)) = 0;
        *((s32 *)(((u8 *)p) + 0x294)) = 40;
        *((s32 *)(((u8 *)((u8 *)r)) + 0x3c)) = 2;
        f = (void *)(((u8 *)p) + 0x2ec);
        Anim_SetSequence(f, (void *)(*((s32 *)(((u8 *)p) + 0x390))), 2, 0, 0);
        step(f, r);
        *((s32 *)(((u8 *)p) + 0x398)) = 0;
        Sound_PlaySong(0x586);
    }
    switch ((u32)(*((s32 *)(((u8 *)base) + 0x398)))) {
        case 0:
            if (tick > 90) {
                struct Frames *f;
                struct Render *r;
                *((s32 *)(((u8 *)base) + 0x304)) = 1;
                *((s32 *)(((u8 *)base) + 0x2fc)) = 3;
                f = (void *)(((u8 *)base) + 0x2ec);
                Anim_SetSequence(f, (void *)(*((s32 *)(((u8 *)base) + 0x390))), 3, 0, 0);
                r = (void *)(((u8 *)base) + 0x2c0);
                step(f, r);
                *((s32 *)(((u8 *)base) + 0x398)) = 1;
            }
            *((s32 *)(((u8 *)base) + 0x288)) = (randomValue() << 31) >> 31;
            goto moving;

        case 1:
            if (tick > 150) {
                *((s32 *)(((u8 *)base) + 0x304)) = 0;
                *((s32 *)(((u8 *)base) + 0x398)) = 2;
                *((s32 *)(((u8 *)base) + 0x294)) = 160;
            }
            *((s32 *)(((u8 *)base) + 0x288)) = randomValue() & 3;
        moving:
            SceneMover_ApproachScreenX(p, *((s32 *)(((u8 *)base) + 0x294)));

            ActorDisplay_UpdateScreenAnchor(p);
            ActorDisplay_UpdateForegroundSprite(p);
            ActorDisplay_UpdateAccentSprite(p);
            SceneActors_EmitPeriodicParticles(p, tick);
            break;

        case 2: {
            int velocity = Player_GetHorizontalVelocity((void *)(*((s32 *)(((u8 *)base) + 0x3a0))));
            s16 position[8];
            if (velocity < 0x6000)
                velocity = 0x6000;
            if ((*((s16 *)(((u8 *)base) + 0x5c2))) > 0)
                velocity >>= 1;
            SceneMover_StepPastScreenX(p, *((s32 *)(((u8 *)base) + 0x294)), velocity);
            *((s32 *)(((u8 *)base) + 0x288)) = randomValue() & 7;
            ActorDisplay_UpdateScreenAnchor(p);
            ActorDisplay_UpdateForegroundSprite(p);
            ActorDisplay_UpdateAccentSprite(p);
            SceneActors_EmitPeriodicParticles(p, tick);
            if (*((s32 *)(((u8 *)base) + 0x300)))
                *((s32 *)(((u8 *)base) + 0x304)) = 1;
            Player_GetWorldPosition(0, position);
            if (((*((s16 *)(((u8 *)base) + 0x278))) > 159) || (tick > 420)) {
                *((s32 *)(((u8 *)base) + 0x304)) = 0;
                *((s32 *)(((u8 *)base) + 0x2fc)) = 3;
                *(((u8 *)base) + 0x2f0) |= 4;
                Anim_RestartSequence((void *)(((u8 *)base) + 0x2ec),
                                     (void *)(*((s32 *)(((u8 *)base) + 0x390))),
                                     (u16)(*((s32 *)(((u8 *)base) + 0x2fc))), 0, 0);
                *((s32 *)(((u8 *)base) + 0x294)) = 60;
                *((s32 *)(((u8 *)base) + 0x398)) = 4;
            } else if ((*((s32 *)(((u8 *)base) + 0x590))) == 1) {
                *((s32 *)(((u8 *)base) + 0x294)) = 60;
                *((s32 *)(((u8 *)base) + 0x398)) = 3;
            }
            break;
        }

        case 3:
            SceneMover_ApproachScreenX(p, *((s32 *)(((u8 *)base) + 0x294)));
            *((s32 *)(((u8 *)base) + 0x288)) = randomValue() & 1;
            ActorDisplay_UpdateScreenAnchor(p);
            ActorDisplay_UpdateForegroundSprite(p);
            ActorDisplay_UpdateAccentSprite(p);
            SceneActors_EmitPeriodicParticles(p, tick);
            *((s32 *)(((u8 *)base) + 0x304)) = 0;
            *((s32 *)(((u8 *)base) + 0x2fc)) = 3;
            *(((u8 *)base) + 0x2f0) |= 4;
            Anim_RestartSequence((void *)(((u8 *)base) + 0x2ec),
                                 (void *)(*((s32 *)(((u8 *)base) + 0x390))),
                                 (u16)(*((s32 *)(((u8 *)base) + 0x2fc))), 0, 0);
            *((s32 *)(((u8 *)base) + 0x398)) = 4;
            *((s32 *)(((u8 *)base) + 0x294)) = 60;
            break;

        case 4:
            SceneMover_ApproachScreenX(p, *((s32 *)(((u8 *)base) + 0x294)));
            *(((u8 *)base) + 0x2f0) |= 4;
            *((s32 *)(((u8 *)base) + 0x288)) = randomValue() & 1;
            ActorDisplay_UpdateScreenAnchor(p);
            ActorDisplay_UpdateForegroundSprite(p);
            ActorDisplay_UpdateAccentSprite(p);
            SceneActors_EmitPeriodicParticles(p, tick);
            if (*((s32 *)(((u8 *)base) + 0x300))) {
                *(((u8 *)base) + 0x2f0) &= ~4;
                *(((u8 *)p) + 0xaf) = 1;
            }
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActors_UpdateHitSensitiveDeparture);
#endif
