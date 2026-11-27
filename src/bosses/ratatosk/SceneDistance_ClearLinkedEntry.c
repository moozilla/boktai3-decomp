#include "global.h"
struct SceneDistanceEntry {
    u32 value;
    u8 pad4[20];
};
struct SceneDistanceSet {
    u8 pad0[0xDF0];
    struct SceneDistanceEntry linked[4];
};
void *SceneActors_GetBySlot(u32);
void SceneDistance_ClearLinkedEntry(s32 index)
{
    struct SceneDistanceSet *set = SceneActors_GetBySlot(4);
    set->linked[index].value = 0;
}
