#include "global.h"
#include "ram.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void Camera_ReadMoveRate(void *);
void Camera_ReturnToDefault(s32);
void Camera_ReturnToDefaultFromScript(void)
{
    if (Script_SeekToKeyword(0x66)) {
        s32 v = Script_GetValue();
        Camera_ReadMoveRate(gActiveCamera);
        Camera_ReturnToDefault(v);
    }
}
