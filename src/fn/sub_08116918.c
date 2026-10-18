#include "global.h"
#define MotionActor_ParsePosition sub_08116918
struct ScriptPosition {
    s32 x, y, z, unused;
};
struct MotionActorState {
    u8 unknown0[0x10];
    s32 x, y, z;
    u8 unknown1C[0x5C0];
    s32 limit;
};
s32 Script_GetKeywordValue(u32, s32);
s32 Script_GetPc(void);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);
void sub_08116918(u8 *actor)
{
    struct ScriptPosition position;
    struct MotionActorState *state;
    if (Script_SeekToKeyword('p')) {
        {
            struct ScriptPosition *dst = &position;
            dst->x = Script_GetPc() ? Script_GetValue() : 0;
        }
        {
            struct ScriptPosition *dst = &position;
            dst->y = Script_GetPc() ? Script_GetValue() : 0;
        }
        {
            struct ScriptPosition *dst = &position;
            dst->z = Script_GetPc() ? Script_GetValue() : 0;
        }
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    state = (struct MotionActorState *)(actor + 0x18);
    {
        s32 zero = 0;
        state->y = position.y << 12;
        state->z = position.z << 12;
        state->limit = zero;
        state->x = position.x << 12;
    }
    *(s32 *)(actor + 0x5F4) = Script_GetKeywordValue('l', 0x20) << 12;
}
