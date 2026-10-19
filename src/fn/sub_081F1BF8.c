#include "global.h"
void Sound_StopSong(u32);
void SceneLink_RemoveNode(u8 *);
void SpatialIndex_RemoveEntry(u8 *);
void SceneActors_Destroy(u8 *);
void SingletonSlot7_RemoveSprites(u8 *);
s32 SingletonSlot7_OnDestroy(u8 *p)
{
    Sound_StopSong(0x590);
    SceneLink_RemoveNode(p + 0x250);
    SpatialIndex_RemoveEntry(p + 0x17c);
    SceneActors_Destroy(p);
    SingletonSlot7_RemoveSprites(p);
    return 0;
}
