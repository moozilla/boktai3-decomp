#define gSineTable gUnk_086149C4
/* Descriptive aliases and object roles are tentative (confidence: low). */
#include "global.h"
#include "scene_actor_reset.h"
struct SceneActorForwardCollider {
    u8 unknown0[0x628];
    struct ContactCollider collider;
};
static inline u8 unavailable(struct SceneActorRegistryResetView *p)
{
    if (p) {
        u32 mask = 0x40;
        if (p->flags2D4 & mask)
            return 1;
        {
            u32 mask = 0x20000;
            if (p->flags2D0 & mask)
                return 1;
        }
        if (p->health <= 0)
            return 1;
    }
    return 0;
}
static inline s32 scaled(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
void SceneActor_UpdateForwardCollider(struct SceneActorRegistryResetView *p)
{
    struct SceneActorForwardCollider *context = p->context;
    union SceneActorRegistryPackedPosition pos;
    union SceneActorRegistryPackedPosition bounds;
    if (!unavailable(p) && p->field2B5 == 0x22) {
        pos.position = p->body.position;
        pos.packed.x += scaled(180 * gUnk_086149C4[(p->body.direction + 64) & 255]);
        pos.packed.z += scaled(180 * gUnk_086149C4[p->body.direction]);
        bounds.packed.x = 64;
        bounds.packed.z = 64;
        bounds.packed.y = 100;
        {
            struct ContactCollider *collider = &context->collider;
            collider->bounds = *(struct ContactPosition *)&bounds;
            Collider_SetPosition(collider, &pos, 0);
        }
    }
}
