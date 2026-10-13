#include "global.h"
struct Vec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
s32 Script_GetKeywordValue(u32, s32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
s32 FallingObject_Create(u32, s32, s32, s32, s32, s32, s32, s32, struct Vec *);
s32 FallingObject_CreateFromScript(void)
{
    struct Vec position;
    u32 id;
    s32 type, flags, parameter, speed, health, power, force;
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
    return FallingObject_Create(id, type, flags, parameter, speed, health, power, force, &position);
}
