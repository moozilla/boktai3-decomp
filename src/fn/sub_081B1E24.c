// MATCHING_CONCESSION: register-pinning
#include "global.h"

// confidence=low: periodically retargets the part beside the player for 64 ticks.
#include "contact_swarm.h"
#include "scene_vec.h"

struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    u8 motion[0x38];
    struct SceneVec *base;
    u8 unknown94[8];
    struct SceneVec origin;
    struct SceneVec target;
    s16 heading, distance;
    u8 unknownB0[0xac];
    u32 variant, flags, state, timer;
    u8 unknown16C[2];
    u8 reset, enter, finished;
};
void MotionTrack_Initialize(void *, const void *, const void *, s32, u32);
void OrbitMotion_BeginDistanceWave(void *, const void *, const void *, s32, s32);
void MotionTrack_Update(void *);
void Vanargand_EmitPartTrail(void *);
void Sound_PlaySong(u32);
void AnimatedParticles_EmitBurst(void *, s32, s32, s32, s32, s32, s32, s32);
struct SceneVec *Player_GetEffectivePosition(u32);
s32 Math_Atan2(s32, s32);
extern const s16 gUnk_086149C4[];
static inline void Difference(struct SceneVec *dst, struct SceneVec *a, struct SceneVec *b)
{
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    register u32 so asm("r3") = 0x164;
    register u32 *sp asm("r0");
    register u32 *tp asm("r1");
    asm("" : "+r"(so));
    sp = (u32 *)((u8 *)p + so);
    *sp = state;
    {
        register u32 to asm("r0") = 0x168;
        asm("" : "+r"(to));
        tp = (u32 *)((u8 *)p + to);
        *tp = 0;
    }
}
static inline void Offset(struct SceneVec *v, s32 x, s32 z)
{
    register s16 *component asm("r1") = &v->x;
    register s32 first asm("r2");
    register s32 second asm("r3");
    register s32 value asm("r0");
    asm("" : "+r"(component));
    first = x;
    asm("" : "+r"(first));
    value = first;
    asm("" : "+r"(value));
    v->x = value + (u16)*component;
    second = z;
    asm("" : "+r"(second));
    value = second;
    asm("" : "+r"(value));
    v->z = value + (u16)v->z;
}
void Vanargand_UpdatePartTracking(u8 *boss, struct VanargandPart *p)
{
    struct SceneVec delta;
    u32 mask = 8;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
        Anim_SetSequence(&p->sequence, *(void **)(boss + 0x4d0), 8, (u8)p->variant, 0);
    }
    switch (p->state) {
        case 0:
            if (!(p->timer & 15)) {
                Difference(&delta, Player_GetEffectivePosition(0), p->base);
                if (p->variant == 1)
                    Offset(&delta, 512, -512);
                else
                    Offset(&delta, -512, 512);
                MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 4);
            }
            MotionTrack_Update(p->motion);
            {
                register u32 off asm("r2") = 0x168;
                u32 *t, v;
                asm("" : "+r"(off));
                t = (u32 *)((u8 *)p + off);
                v = ++*t;
                asm("" : : "r"(off));
                if (v & 64) {
                    SetState(p, 1);
                    p->enter = 1;
                }
            }
            break;
        case 1:
            p->finished = 1;
            break;
    }
}
