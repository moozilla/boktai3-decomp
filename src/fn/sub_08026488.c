#include "scene_actors.h"
#include "scene_graphics_actor.h"
#include "engine_globals.h"
#include "ram.h"

static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline void effect(u8 *p, int id, int mode, int duration)
{
    *(u32 *)(p + 0x390) = id;
    p[0x38e] = mode;
    p[0x38f] = duration;
    if (p[0x38e] == 2) {
        u32 value = duration == 5 ? 32 : 64;
        p[0x38c] = value;
    } else if (p[0x38e] == 1)
        p[0x38c] = 0;
}
static inline void effect2(u8 *p, int mode, int id, int duration)
{
    *(u32 *)(p + 0x390) = id;
    p[0x38e] = mode;
    p[0x38f] = duration;
    if (p[0x38e] == 2) {
        u32 value = duration == 5 ? 32 : 64;
        p[0x38c] = value;
    } else if (p[0x38e] == 1)
        p[0x38c] = 0;
}
void SceneActors_StopBackgroundFlash(void *);
void BackgroundFlash_BeginAppearance(void *);
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define U32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define U8(p, o)  (*((u8 *)(p) + (o)))
void SceneActorAnimation_UpdateFlashWindup(struct StateActor *p, int tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    if (p->actionChanged) {
        int sequence;
        p->actionChanged = 0;
        sequence = choose(actor->graphics.animationFlags, 64, 26);
        {

            SceneActors_StartDirectionalAnimation(
                p, &actor->animation, (int)&actor->animationResource, sequence, 2, 1, 4);
        }
        effect((u8 *)p, 826, 1, 5);
        U32(actor, 0x3a8) = 0;
        Sound_PlaySongChecked(851);
        Sound_PlaySongChecked(887);
    }
    if (actor->graphics.stepTableKind != 3) {
        SceneActorState_SetFlags(p, 0x100000);
    }
    if (U8(actor, 0x382)) {
        {
            int age = tick >> 6;
            U8(actor, 0x399) = age;
        }
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 index = gRandomIndex * 2;
            u16 *table;
            u32 random;
            table = ((u16 *)RANDOM_TABLE_ADDR);
            index += (u32)table;
            random = *(u16 *)index;
            U8(actor, 0x398) = random;
        }
    }
    if (actor->animation.frameIndex == 5)
        actor->animation.elapsed = 0;
    if (p->actionTryUpdate(p)) {
        effect2((u8 *)actor, 2, 826, 5);
        SceneActors_StopBackgroundFlash(actor);
    } else {
        SceneActorState_SetFlags(p, 64);
        if (!p->actionReset(p)) {
            if (tick == 10)
                BackgroundFlash_BeginAppearance(actor->graphics.backgroundFlash);
            if (actor->animation.frameIndex == 2 && !actor->animation.elapsed)
                Sound_PlaySongChecked(849);
            if (tick > S16(actor, 0x374))
                p->actionFinished = 1;
        }
    }
}
