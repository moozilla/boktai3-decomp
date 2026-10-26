#include "global.h"

#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
#include "engine.h"

u8 sub_0812E118(u32, u32, u32, u32);
extern const u16 gUnk_0203B400[];

static inline s32 sub0808FA40_Quarter(s32 heading)
{
    if (heading >= 0)
        return heading >> 6;
    return -(-heading >> 6);
}

#ifdef NONMATCHING
// fuzzy: 68.7%
// scope shape 111 (differences once low registers are set aside)
// confidence=low: rolls a heading, walks up to two free cells that way, and stores the chosen cell center in the actor's context
void sub_0808FA40(void *arg0, u32 arg1)
{
    struct SceneActorRegistryResetView *actor = arg0;
    struct ContactPosition center;
    void *context;
    s32 tries = 0;
    u32 cell, height, shape;
    s32 chosen, next, i;

    context = actor->context;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    actor->direction = gUnk_0203B400[gRandomIndex];
    do {
        cell = actor->terrainCell;
        height = SceneActorTerrain_GetHeight(cell, &actor->position);
        shape = *SceneActorTerrain_GetCell(cell) >> 4;
        switch (sub0808FA40_Quarter(actor->direction + 32) & 3) {
            case 0:
                next = cell + 1;
                i = 0;
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next++;
                    i++;
                } while (i != 2);
                chosen = next - 1;
                break;
            case 1:
                next = cell + SceneActorTerrain_Width();
                i = 0;
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next += SceneActorTerrain_Width();
                    i++;
                } while (i != 2);
                chosen = next - SceneActorTerrain_Width();
                break;
            case 2:
                next = cell - 1;
                i = 0;
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next--;
                    i++;
                } while (i != 2);
                chosen = next + 1;
                break;
            case 3:
                next = cell - SceneActorTerrain_Width();
                i = 0;
                do {
                    if (!sub_0812E118(cell, next, height, shape))
                        break;
                    next -= SceneActorTerrain_Width();
                    i++;
                } while (i != 2);
                chosen = next + SceneActorTerrain_Width();
                break;
            default:
                chosen = cell;
                break;
        }
        if (chosen != actor->terrainCell)
            break;
        actor->direction += 64;
        tries++;
    } while (tries <= 3);
    actor->direction = arg1;
    SceneActorTerrain_GetCenter(chosen, &center);
    *(struct ContactPosition *)((u8 *)context + 0x3EC) = center;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0808FA40);
#endif
