#ifndef GUARD_PROXIMITY_ICON_H
#define GUARD_PROXIMITY_ICON_H
#include "effects/sprites/projected_display.h"
#include "objects/scene_object.h"
#include "player/player.h"
struct ProximityIcon {
    struct Task task;
    u16 type, radius;
    struct AnimSprite animation;
    struct ContactPosition origin;
};
s32 ProximityIcon_IsPlayerNearby(struct ProximityIcon *);
s32 ProximityIcon_Update(struct ProximityIcon *);
u32 ProximityIcon_Destroy(struct ProximityIcon *);
s32 ProximityIcon_InitFromScript(struct ProximityIcon *);
void AnimSprite_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);
#endif
