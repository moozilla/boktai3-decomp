#include "global.h"
#include "scene_vec.h"
struct TileTrigger {
    u32 flags;
    u8 unknown4[0x70];
    u8 animation[0x10];
    struct SceneVec position;
    u32 key;
    u8 unknown90[0x28];
    u16 sequence, delay, unknownBC;
    s16 state;
    u16 triggerFlags;
    u8 forced, frame;
};
struct Player *SceneActorQuery_FindPlayerInBounds(const struct SceneVec *, const struct SceneVec *,
                                                  s32);
u8 TileTrigger_IsTetherNearby(struct TileTrigger *);
u32 sub_0806F06C(const struct SceneVec *, const struct SceneVec *, u32 *);
void TileTrigger_TickIdle(struct TileTrigger *p)
{
    struct SceneVec radius;
    u32 found;
    s32 i;
    p->flags |= 1;
    radius.z = 128;
    radius.x = 128;
    radius.y = 16;
    for (i = 0; i <= 3; i++) {
        if (SceneActorQuery_FindPlayerInBounds(&p->position, &radius, i))
            goto activate;
    }
    if (TileTrigger_IsTetherNearby(p))
        goto activate;
    found = 0;
    if (sub_0806F06C(&p->position, &radius, &found)) {
    activate:
        p->state = 2;
        p->frame = 0;
    }
}
