#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "bosses/ratatosk/ratatosk_particle.h"
#include "engine/ram.h"
#include "scene/collision/contact_position.h"
#include "engine/engine_globals.h"
#include "scene/actor/scene_actor_components.h"

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
static inline s32 trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}

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
void Player_AddExperience(u16);
void Bestiary_UnlockEntry(s32);
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
static inline u32 Ratatosk_AddFlags(const struct Ratatosk *actor, u32 flags)
{
    return actor->flags | flags;
}

void Ratatosk_UpdateDefeatSequence(struct Ratatosk *p, s32 tick)
{
    struct RatatoskParticleConfig params;
    struct ContactPosition spawn;
    struct Ratatosk *p2 = p;
    s32 i;
    s32 value;
    u8 angle;
    u16 radius;
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked) {
            angle = p->savedAngle;
            Ratatosk_SetFacing(p, (s8)angle);
        } else {
            Task_Kill(p2->particles);
            SceneActors_RunFirstEventScript(p, &p2->anchor);
        }
        clearFade((void *)p2->animationData);
        SceneActors_StartDirectionalAnimation(p, p2->animationData, p2->animation, 26, 1, 1, 4);
        Sound_PlaySong(0x560);
        p2->scaleX = 0;
        p2->scaleZ = 0;
        p2->scaleMode = 0;
        PlayerClone_BeginActiveReactions();
        SunOrb_FadeOutAll(p2->sunOrbs);
        Sound_StopSong(0x56b);
        Sound_StopSong(0x569);
        Sound_StopSong(0x570);
    }
    if (cellFlags(cellAt(&p->body.position)) & 1)
        p2->terrainTimer = 8;
    if (p2->terrainTimer > 0) {
        p2->terrainTimer--;
        {
            struct ContactPosition *home = &p2->motionPath.position;
            struct ContactPosition *pos = &p->body.position;
            s32 x = home->x - pos->x;
            s32 z = home->z - pos->z;
            s32 angle = Math_Atan2(x, z);
            struct ContactPosition *velocity = &p->body.motion;
            radius = 20;
            polar(velocity, angle, radius);
        }
    }
    if (tick == 140) {
        Sound_PlaySong(0x33a);
        SceneActors_StartAnimation(p, p2->animationData, p2->animation, 28, 0, 4);
    }
    if (((Mod(tick, 16) == 0) || (tick > 136)) && (tick <= 150)) {
        /* Each particle spawns at a pointer taken once, before the loop; the
         * per-iteration pointer only builds the offset position. */
        struct ContactPosition *spawnAt = &spawn;
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
            Ratatosk_ConfigureAnimatedParticle(&params, (randomA() & 31) + 292, value, angle, 1,
                                               variant, radius);
            Ratatosk_SpawnAnimatedParticle(&params, spawnAt);
        }
    }
    if (tick == 440) {
        if (!p->locked) {
            s32 count;
            SceneActors_RunSecondEventScript(p, &p2->anchor);
            Player_AddExperience(p2->parameterED2);
            Bestiary_UnlockEntry(0x51);
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
        p->actionFinished = 1;
    }
    value = Ratatosk_AddFlags(p, 15);
    p->flags = value & ~16;
}
