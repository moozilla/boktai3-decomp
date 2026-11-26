#ifndef GUARD_PROXIMITY_ICON_H
#define GUARD_PROXIMITY_ICON_H
#include "effects/five_entry_projected_display/projected_display.h"
#include "field/scene_objects/scene_object.h"
#include "engine/shared/player.h"
struct ProximityIcon {
    struct Task task;
    u16 type, radius;
    struct SceneInputWidget animation;
    struct ContactPosition origin;
};
s32 ProximityIcon_IsPlayerNearby(struct ProximityIcon *);
s32 ProximityIcon_Update(struct ProximityIcon *);
u32 ProximityIcon_Destroy(struct ProximityIcon *);
s32 ProximityIcon_InitFromScript(struct ProximityIcon *);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);
#endif
