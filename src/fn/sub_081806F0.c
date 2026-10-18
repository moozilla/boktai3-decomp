#define gProximityPlayer gUnk_02000580
#include "global.h"
#include "contact_actor.h"
struct ProximityVolume {
    u8 unknown0[8];
    struct ContactPosition position;
    void *reference;
    u8 bounds[1];
};
struct ProximityPlayer {
    u8 unknown0[0x30];
    struct ContactPosition position;
};
extern struct ProximityPlayer *gUnk_02000580;
s32 sub_0821F204(void *, void *, void *);
u32 sub_0821F2EC(void *, void *, void *);
u32 ProximityVolume_ContainsPlayer(struct ProximityVolume *volume)
{
    struct ContactPosition *player = &gUnk_02000580->position;
    void *bounds = volume->bounds;
    struct ContactPosition *position = &volume->position;
    if (sub_0821F204(bounds, position, player) && sub_0821F2EC(bounds, position, player))
        return 1;
    return 0;
}
