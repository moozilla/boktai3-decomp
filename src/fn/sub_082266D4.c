#include "global.h"
#include "ram.h"
struct Packed {
    u32 a : 16, b : 16, c : 16, pad : 16;
};
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void Camera_ReadMoveRate(u32);
void Camera_PanTo(s32, struct Packed *);
void Camera_PanToFromScript(void)
{
    struct Packed pos;
    if (Script_SeekToKeyword('f')) {
        s32 value = Script_GetValue();
        if (Script_SeekToKeyword('p')) {
            pos.a = Script_GetValue();
            pos.b = Script_GetValue();
            pos.c = Script_GetValue();
            Camera_ReadMoveRate(gActiveCamera);
            Camera_PanTo(value, &pos);
        }
    }
}
