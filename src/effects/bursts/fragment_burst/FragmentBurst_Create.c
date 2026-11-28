#include "effects/bursts/fragment_burst.h"
#include "scene/motion/scene_actor_terrain.h"

struct FragmentBurstPool *FragmentBurst_EnsurePool(void);
struct FragmentBurst *FragmentBurst_AllocateSpriteGroup(struct FragmentBurstPool *);
void sub_081281D0(struct FragmentBurst *, u32, u32, u32, u32);

void FragmentBurst_Create(struct ContactPosition *position, u32 amount, u32 kind, u32 parameter,
                          u32 flags, u32 unused, u32 completionScript)
{
    struct ContactPosition screen;
    struct FragmentBurstPool *pool = gSpriteGroupPool;
    struct FragmentBurst *burst;
    s32 i;
    const u16 *table;

    if (!pool) {
        pool = FragmentBurst_EnsurePool();
        if (!pool)
            return;
    }
    burst = FragmentBurst_AllocateSpriteGroup(pool);
    if (!burst)
        return;
    burst->state = 1;
    burst->age = 0;
    burst->timer = 0;
    burst->enter = 1;
    burst->origin = *position;
    burst->completionScript = completionScript;
    burst->position = *position;
    burst->position.y =
        SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(position), position);
    FragmentBurst_ProjectToScreen(&screen, position);
    for (i = 0; i < 4; i++) {
        struct DisplaySequenceSprite *sprite = &burst->sprites[i];

        Sprite_Init(sprite, pool->graphics, 0);
        sprite->flags |= 0x11;
        sprite->priority = 1;
    }
    for (i = 0; i < 3; i++) {
        struct DisplaySequenceSprite *sprite = &burst->sprites[i];
        struct DisplaySequenceState *sequence = &burst->sequences[i];

        Anim_SetSequence(sequence, pool->sequenceResource, 15, 0, 0);
        gRandomIndex = (gRandomIndex + 1) & 1023;
        table = gRandomTable;
        {
            u32 value = table[gRandomIndex];
            value &= 3;
            sequence->index = value;
        }
        AnimData_SelectPaletteBank((void *)sprite->resource, 0x13F);
        sprite->position = screen;
        sprite->position.y += i << 5;
    }
    sub_081281D0(burst, amount, kind, parameter, flags);
}
