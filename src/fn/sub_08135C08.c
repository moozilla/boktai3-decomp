#define SceneActorPlacement_BeginDestinationJump sub_08135C08
#include "global.h"
#include "scene_actor_placement.h"
s32 Math_Atan2(s32, s32);
void sub_0812E5A4(struct SceneActorPlacementView *);
static inline void Difference(struct ContactPosition *out, const struct ContactPosition *b, const struct ContactPosition *a)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
s32 sub_08135C08(struct SceneActorPlacementView *actor)
{
    struct ContactPosition delta;
    if (actor->category == 3 && SceneActorPlacement_ReadFlags16(actor, 0x100) && (actor->flags2D8 & 0x400)) {
        Difference(&delta, &actor->position, &actor->destination);
        actor->velocity.x = Div(delta.x, 36);
        actor->velocity.y = 20;
        actor->velocity.z = Div(delta.z, 36);
        actor->direction = Math_Atan2(delta.x, delta.z);
        sub_0812E5A4(actor);
        SceneActorPlacement_PlayVariantEventSound(actor, 6);
        return 1;
    }
    return 0;
}
