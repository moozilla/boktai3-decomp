#include "global.h"
#include "engine.h"
extern u32 gUnk_03005420; /* camera (guess) */
void Camera_ReadMoveRate(u32);
void sub_08226558(s32 target, s32 frames, s32 mode);
/* Script: -w target, -f (default 1), -N (default 0); stops the current camera
   move and hands the three values to sub_08226558 (tentative: camera follow). */
void Camera_SetFollowFromScript(void)
{
    s32 target, frames, mode;
    if (Script_SeekToKeyword('w')) {
        target = Script_GetValue();
        if (Script_SeekToKeyword('f'))
            frames = Script_GetValue();
        else
            frames = 1;
        if (Script_SeekToKeyword('N'))
            mode = Script_GetValue();
        else
            mode = 0;
        Camera_ReadMoveRate(gUnk_03005420);
        sub_08226558(target, frames, mode);
    }
}
