#ifndef GUARD_PROXIMITY_ICON_H
#define GUARD_PROXIMITY_ICON_H
#include "projected_display.h"
#include "scene_object.h"
#include "player.h"
struct ProximityIcon {
    u8 task[0x18];
    u16 type, radius;
    struct ProjectedDisplayEntry animation;
    struct ContactPosition origin;
};
s32 sub_08057678(struct ProximityIcon *);
s32 sub_080576C8(struct ProximityIcon *);
u32 sub_08057704(struct ProximityIcon *);
s32 sub_08057714(struct ProximityIcon *);
void sub_0821983C(void *, void *, u32, u32, u32, u32, s32, const void *);
#endif
