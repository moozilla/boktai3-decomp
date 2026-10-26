#include "global.h"
#include "ram.h"
struct Packed {
    u32 a : 16, b : 16, c : 16, pad : 16;
};
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void Camera_ReadMoveRate(u32);
void Camera_SnapTo(struct Packed *);
void Camera_SnapToFromScript(void)
{
    struct Packed pos;
    if (Script_SeekToKeyword('p')) {
        pos.a = Script_GetValue();
        pos.b = Script_GetValue();
        pos.c = Script_GetValue();
        Camera_ReadMoveRate(gActiveCamera);
        Camera_SnapTo(&pos);
    }
}
