#include "global.h"
#include "ram.h"
#include "contact_position.h"
#include "engine_globals.h"
#include "scene_actor_components.h"

#ifdef NONMATCHING
// fuzzy: 99.4%
struct Actor {
    u8 pad0[0x48];
    struct SceneActorBody body;
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
    struct ContactPosition anchor;
    u8 pad374[0x58];
    struct ContactPosition home;
    u8 pad3D4[0xa78];
    s16 selectedPoint, selectedTarget;
    u16 padE50;
    u16 scaleX, scaleZ, padE56, scaleStep;
    u8 padE5A[0x14];
    u16 motion;
    u8 motionFlag, padE71, targetAngle, phaseTimer;
    s16 hitTimer, terrainTimer;
};
extern const u16 gUnk_086141BC[];
extern const s16 gUnk_086149C4[];
void SceneActors_StartDirectionalAnimation(void *, void *, void *, s32, s32, s32, s32);
void SceneActors_StartAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
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
    p->body.direction = angle;
    shift = (p->body.direction + 0x60) >> 6;
    v = 3;
    v &= shift;
    p->facing = v;
    shift = (p->body.direction + 0x70) >> 5;
    v = 7;
    v &= shift;
    p->facing8 = v;
}
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    s32 zero = 0;
    v->x = x;
    v->y = zero;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}
struct AnimatedParticleParams {
    u16 kind, animation;
    u8 reserved4[2];
    u8 angle, reserved7;
    u32 speed, variant, drift;
    u32 field14, active;
};
void Ratatosk_InitAnimatedParticleParams(struct AnimatedParticleParams *);
void Ratatosk_ConfigureAnimatedParticle(struct AnimatedParticleParams *, u16, u32, u8, u32, u32,
                                        u32);
void sub_081FC26C(struct AnimatedParticleParams *, struct ContactPosition *);
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
static inline s32 cellAt(struct ContactPosition *v)
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
    struct AnimatedParticleParams params;
    struct ContactPosition spawn;
    struct Actor *p2 = p;
    s32 i;
    s32 value;
    u8 angle;
    u16 radius;
    if (p->changed) {
        p->changed = 0;
        if (p->locked) {
            angle = p->savedAngle;
            facing(p, (s8)angle);
        } else {
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
    if (cellFlags(cellAt(&p->body.position)) & 1)
        p2->terrainTimer = 8;
    if (p2->terrainTimer > 0) {
        p2->terrainTimer--;
        {
            struct ContactPosition *home = &p2->home;
            struct ContactPosition *pos = &p->body.position;
            s32 x = home->x - pos->x;
            s32 z = home->z - pos->z;
            {
                s32 angle = Math_Atan2(x, z);
                struct ContactPosition *velocity = &p->body.motion;
                radius = 20;
                polar(velocity, angle, radius);
            }
        }
    }
    if (tick == 140) {
        Sound_PlaySong(0x33a);
        SceneActors_StartAnimation(p, p2->animationData, p2->animation, 28, 0, 4);
    }
    if (((Mod(tick, 16) == 0) || (tick > 136)) && (tick <= 150)) {
        for (i = 0; i < 2; ++i) {
            struct ContactPosition *position = &spawn;
            u16 speedSample = randomA() & 15;
            u16 variant;
            value = speedSample;
            variant = randomA() & 1;
            angle = (u8)randomA();
            radius = randomA() & 15;
            spawn = p->body.position;
            position->x -= 128;
            position->y += 64;
            variant <<= 12;
            radius <<= 12;
            Ratatosk_InitAnimatedParticleParams(&params);
            {
                Ratatosk_ConfigureAnimatedParticle(&params, (randomA() & 31) + 292, value, angle, 1,
                                                   variant, radius);
            }
            sub_081FC26C(&params, position);
        }
    }
    if (tick == 440) {
        if (!p->locked) {
            s32 count;
            SceneActors_RunSecondEventScript(p, &p2->anchor);
            sub_08159164(*((u16 *)(((u8 *)p2) + 0xed2)));
            GameState_SetEventFlag(0x51);
            increment((u16 *)(gGameState + 0x87c));
            increment((u16 *)(gGameState + 0x538));
            increment((u16 *)(gGameState + 0x55e));
            count = (*((s16 *)(gGameState + 0x592))) + 1;
            if (count > 9999)
                count = 9999;
            else if (count < 0)
                count = 0;
            *((u16 *)(gGameState + 0x592)) = count;
        }
        p->finished = 1;
    }
    {
        value = Arg1(15, p);
        p->flags = value & (~16);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Ratatosk_UpdateDefeatSequence);
#endif
