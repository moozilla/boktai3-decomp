#include "global.h"
struct Packed {
    u32 a : 16, b : 16, c : 16, pad : 16;
};
extern u32 gUnk_03005420;
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void Camera_ReadMoveRate(u32);
void Camera_SnapTo(struct Packed *);
void sub_08226670(void)
{
    struct Packed pos;
    if (Script_SeekToKeyword('p')) {
        pos.a = Script_GetValue();
        pos.b = Script_GetValue();
        pos.c = Script_GetValue();
        Camera_ReadMoveRate(gUnk_03005420);
        Camera_SnapTo(&pos);
    }
}
