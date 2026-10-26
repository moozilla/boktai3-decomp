#include "global.h"
#include "ram.h"
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
int Anim_SetSequence(struct Frames *, void *, u32, int, int);
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
extern const u16 gUnk_086141BC[];
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define H(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define B(p, o) (*((u8 *)(p) + (o)))
static inline int randomValue(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
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

void ActorMotion_AdvanceTowardScreenTarget(void *, int);
// confidence=low: paired render animations and movement are observed statically.
void SceneActors_UpdateDelayedMovement(void *arg, int time)
{
    void *p = arg;
    int tick = time;
    int result = 0;
    void *base = p;
    u8 *enter;
    enter = (u8 *)p + 0xb1;
    if (*enter) {
        int zero = 0;
        struct Render *r;
        struct Render *r2;
        struct Frames *f;
        *enter = zero;
        r = (void *)((u8 *)p + 0x2c0);
        r2 = (void *)((u8 *)p + 0x308);
        W(p, 0x298) = zero;
        W(p, 0x290) = zero;
        W((u8 *)r + 0x3c, 0) = zero;
        f = (void *)((u8 *)p + 0x2ec);
        Anim_SetSequence(f, (void *)W(p, 0x390), zero, 0, zero);
        step(f, r);
        W((u8 *)r2 + 0x3c, 0) = 4;
        f = (void *)((u8 *)r2 + 0x2c);
        Anim_SetSequence(f, (void *)W(base, 0x390), 4, 0, 0);
        step(f, r2);
    }
    {
        int t = tick;
        asm("" : "+r"(t));
        if (t > 120)
            result = SceneMover_StepPastScreenX(p, W(base, 0x294), 0x6000);
    }
    W(base, 0x288) = (randomValue() & 1);
    ActorDisplay_UpdateScreenAnchor(p);
    ActorDisplay_UpdateForegroundSprite(p);
    ActorDisplay_UpdateAccentSprite(p);
    SceneActors_EmitPeriodicParticles(p, tick);
    {
        int test = result;
        if (test) {
            B(p, 0xaf) = 1;
            W(base, 0x284) = 16;
        }
    }
}
