#include "global.h"
#include "engine.h"
#include "ram.h"
void Camera_ReadMoveRate(u32);
void Camera_SetSway(s32 target, s32 frames, s32 mode);
/* Script: -w target, -f (default 1), -N (default 0); stops the current camera
   move and hands the three values to Camera_SetSway (tentative: camera follow). */
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
        Camera_ReadMoveRate(gActiveCamera);
        Camera_SetSway(target, frames, mode);
    }
}
