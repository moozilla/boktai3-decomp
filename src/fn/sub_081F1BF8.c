#include "global.h"
void Sound_StopSong(u32);
void sub_0815F738(u8 *);
void sub_0811B394(u8 *);
void SceneActors_Destroy(u8 *);
void sub_081F1240(u8 *);
s32 sub_081F1BF8(u8 *p)
{
    Sound_StopSong(0x590);
    sub_0815F738(p + 0x250);
    sub_0811B394(p + 0x17c);
    SceneActors_Destroy(p);
    sub_081F1240(p);
    return 0;
}
