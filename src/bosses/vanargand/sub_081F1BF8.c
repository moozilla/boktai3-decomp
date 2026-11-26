#include "global.h"
void Sound_StopSong(u32);
void SceneLink_RemoveNode(void *);
void SpatialIndex_RemoveEntry(u8 *);
void SceneActors_Destroy(u8 *);
void VanargandHead_RemoveSprites(u8 *);
s32 VanargandHead_OnDestroy(u8 *p)
{
    Sound_StopSong(0x590);
    SceneLink_RemoveNode(p + 0x250);
    SpatialIndex_RemoveEntry(p + 0x17c);
    SceneActors_Destroy(p);
    VanargandHead_RemoveSprites(p);
    return 0;
}
