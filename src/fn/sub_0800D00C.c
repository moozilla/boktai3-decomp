#include "global.h"
struct Vec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
s32 sub_0821ABA8(u32, s32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
s32 FallingObject_Create(u32, s32, s32, s32, s32, s32, s32, s32, struct Vec *);
s32 FallingObject_CreateFromScript(void)
{
    struct Vec position;
    u32 id;
    s32 type, flags, parameter, speed, health, power, force;
    id = (u16)sub_0821ABA8(110, 0);
    type = sub_0821ABA8(116, 0);
    flags = sub_0821ABA8(102, 0);
    parameter = sub_0821ABA8(84, 0);
    speed = sub_0821ABA8(115, 0);
    health = sub_0821ABA8(108, 0);
    power = sub_0821ABA8(80, 200);
    force = sub_0821ABA8(70, 50);
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
