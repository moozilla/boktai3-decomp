#include "falling_objects.h"
s32 FallingObject_CreateFromScript(void)
{
    struct FallingPackedVec position;
    u16 id;
    s32 type; u32 flags, parameter, speed, health, power, force;
    id = (u16)Script_GetKeywordValue(110, 0);
    type = Script_GetKeywordValue(116, 0);
    flags = Script_GetKeywordValue(102, 0);
    parameter = Script_GetKeywordValue(84, 0);
    speed = Script_GetKeywordValue(115, 0);
    health = Script_GetKeywordValue(108, 0);
    power = Script_GetKeywordValue(80, 200);
    force = Script_GetKeywordValue(70, 50);
    if (Script_SeekToKeyword(112)) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    return FallingObject_Create(id, type, flags, parameter, speed, health, power, force, (struct FallingVec *)&position);
}
