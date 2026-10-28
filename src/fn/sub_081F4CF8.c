#include "global.h"
#include "ram.h"
#include "engine_globals.h"

#ifdef NONMATCHING
// fuzzy: 90.1%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: register allocation, pointer lifetimes and shared block placement.
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Actor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[2];
    struct Vec position, velocity;
    u8 pad64[0x2c];
    u32 timer;
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[2];
    u8 mode, padA3;
    u32 modeTime;
    u8 facing, facing8, padAA, animationId, state, nextState, active, finished, padB0, changed,
        padB2, locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x33];
    void (*update)(void);
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x67];
    u32 phase;
    u8 pad180[0x1ac];
    u8 animation[0x20];
    u8 animationData[0x20];
    struct Vec anchor;
    u8 pad374[0x58];
    struct Vec home;
    u8 pad3D4[0xa78];
    s16 selectedPoint, selectedTarget;
    u16 padE50;
    u16 scaleX, scaleZ, padE56, scaleStep;
    u8 padE5A[0x14];
    u16 motion;
    u8 motionFlag, padE71, targetAngle, phaseTimer;
    s16 hitTimer, terrainTimer;
};
#define H(o) (*(s16 *)((u8 *)p2 + (o)))
#define U(o) (*(u16 *)((u8 *)p2 + (o)))
#define B(o) (*(u8 *)((u8 *)p2 + (o)))
#define W(o) (*(u32 *)((u8 *)p2 + (o)))
extern const u16 gUnk_086141BC[];
extern const s16 gUnk_086149C4[];
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneActors_StartAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
u8 Callback_InvokeActor(void *, void *);
s32 Math_Atan2(s32, s32);
static inline u32 randomA(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
static inline void facing(struct Actor *p, s32 angle)
{
    u32 v;
    s32 shift;
    p->angle = angle;
    shift = (p->angle + 0x60) >> 6;
    v = 3;
    v &= shift;
    p->facing = v;
    shift = (p->angle + 0x70) >> 5;
    v = 7;
    v &= shift;
    p->facing8 = v;
}
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct Vec *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}
static inline void sum(struct Vec *d, struct Vec *a, struct Vec *b)
{
    d->x = a->x + b->x;
    d->y = a->y + b->y;
    d->z = a->z + b->z;
}
void Ratatosk_InitAnimatedParticleParams(void *);
void Ratatosk_ConfigureAnimatedParticle(void *, s32, s32, u8, s32, s32, s32);
void sub_081FC26C(void *, struct Vec *);
// confidence=low: defeat role inferred from event flag 0x51 and persistent counters.
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[0x1c];
    u16 rows[1];
};
extern u8 *gGameState;
void *TerrainModifiers_FindCellOverride(s32, s32);
void Task_Kill(void *);
void SceneActors_RunFirstEventScript(void *, void *);
void SceneActors_RunSecondEventScript(void *, void *);
void PlayerClone_BeginActiveReactions(void);
void SunOrb_FadeOutAll(void *);
void sub_08159164(u16);
void GameState_SetEventFlag(s32);
s32 Mod(s32, s32);
static inline s32 cellAt(struct Vec *v)
{
    s32 x = v->x >> 8, z = v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct Map *)gTerrainMap)->rows[z] + x;
}
static inline u16 cellFlags(s32 index)
{
    u8 *v = TerrainModifiers_FindCellOverride(index, 1);
    if (v)
        v += 4;
    else
        v = ((struct Map *)gTerrainMap)->cells + (index * 4 + 12);
    return *(u16 *)(v + 2);
}
struct Anim {
    u32 pad[2];
    u32 flags;
};
static inline void delta(struct Vec *a, struct Vec *b, s32 *x, s32 *z)
{
    *x = a->x - b->x;
    *z = a->z - b->z;
}
static inline void clearFade(struct Anim *a)
{
    a->flags &= ~0x200;
}
static inline void increment(u16 *v)
{
    u16 a = *v;
    if ((s16)*v < 9999)
        *v = a + 1;
    else
        *v = 9999;
}
static inline u32 Arg1(u32 k_, struct Actor *p)
{
    return p->flags | k_;
}

void Ratatosk_UpdateDefeatSequence(struct Actor *p, s32 tick)
{
    u8 params[0x1c];
    struct Vec spawn;
    struct Actor *p2 = p;
    if (p->changed) {
        p->changed = 0;
        if (p->locked)
            facing(p, p->savedAngle);
        else {
            Task_Kill(*((void **)(((u8 *)p2) + 0xf20)));
            SceneActors_RunFirstEventScript(p, &p2->anchor);
        }
        clearFade((void *)p2->animationData);
        SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 26, 1, 1, 4);
        Sound_PlaySong(0x560);
        p2->scaleX = 0;
        p2->scaleZ = 0;
        p2->scaleStep = 0;
        PlayerClone_BeginActiveReactions();
        SunOrb_FadeOutAll(*((void **)(((u8 *)p2) + 0xf2c)));
        Sound_StopSong(0x56b);
        Sound_StopSong(0x569);
        Sound_StopSong(0x570);
    }
    if (cellFlags(cellAt(&p->position)) & 1)
        p2->terrainTimer = 8;
    if (p2->terrainTimer > 0) {
        p2->terrainTimer--;
        {
            struct Vec *home = &p2->home;
            struct Vec *pos = &p->position;
            s32 x = home->x - pos->x;
            s32 z = home->z - pos->z;
            polar(&p->velocity, Math_Atan2(x, z), 20);
        }
    }
    if (tick == 140) {
        Sound_PlaySong(0x33a);
        SceneActors_StartAnimation(p, p2->animationData, p2->animation, 28, 0, 4);
    }
    if (((Mod(tick, 16) == 0) || (tick > 136)) && (tick <= 150)) {
        s32 i = 1;
        do {
            u16 speed = randomA() & 15;
            u16 variant = randomA() & 1;
            s8 angle = randomA();
            u16 drift = randomA() & 15;
            struct Vec *position = &spawn;
            spawn = p->position;
            position->x -= 128;
            position->y += 64;
            drift <<= 12;
            variant <<= 12;
            Ratatosk_InitAnimatedParticleParams(params);
            {
                u8 k1 = 31;
                Ratatosk_ConfigureAnimatedParticle(params, (randomA() & k1) + 292, speed, angle, 1,
                                                   variant, drift);
            }
            sub_081FC26C(params, position);
        } while ((--i) >= 0);
    }
    if (tick == 440) {
        if (!p->locked) {
            s32 value;
            SceneActors_RunSecondEventScript(p, &p2->anchor);
            sub_08159164(*((u16 *)(((u8 *)p2) + 0xed2)));
            GameState_SetEventFlag(0x51);
            increment((u16 *)(gGameState + 0x87c));
            increment((u16 *)(gGameState + 0x538));
            increment((u16 *)(gGameState + 0x55e));
            value = (*((s16 *)(gGameState + 0x592))) + 1;
            if (value > 9999)
                value = 9999;
            else if (value < 0)
                value = 0;
            *((u16 *)(gGameState + 0x592)) = value;
        }
        p->finished = 1;
    }
    {
        u32 value = Arg1(15, p);
        p->flags = value & (~16);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Ratatosk_UpdateDefeatSequence);
#endif
