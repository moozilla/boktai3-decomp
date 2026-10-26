#include "global.h"
#include "ram.h"
struct CameraOffset {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 w : 16;
};
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void Camera_ReadMoveRate(void *);
void Camera_TrackActor(s32, struct CameraOffset *);
void sub_08226748(void)
{
    s32 actor;
    struct CameraOffset offset;
    if (!Script_SeekToKeyword('n'))
        return;
    actor = Script_GetValue();
    if (Script_SeekToKeyword('o')) {
        offset.x = Script_GetValue();
        offset.y = Script_GetValue();
        offset.z = Script_GetValue();
    } else {
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
    }
    Camera_ReadMoveRate(gActiveCamera);
    Camera_TrackActor(actor, &offset);
}
