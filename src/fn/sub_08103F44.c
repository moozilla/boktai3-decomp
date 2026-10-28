#include "global.h"
#include "scene_actor_terrain.h"
#include "ram.h"
extern void *gUnk_02000580;
u32 Player_TestAttachmentFlags(void *, u32);
static inline u32 Attributes(s32 cell)
{
    return *(u16 *)(SceneActorTerrain_GetCell(cell) + 2);
}
static inline u8 CanExit(s32 cell, s32 height)
{
    if ((*SceneActorTerrain_GetCell(cell) & 15) != height || (Attributes(cell) & 0xC3))
        return 0;
    return 1;
}
static inline u8 PlayExit(s32 cell, s32 height)
{
    if (CanExit(cell, height)) {
        Sound_PlaySong(0x151);
        return 1;
    }
    return 0;
}
void NamedObject_PlayAdjacentExitSound(u8 *o)
{
    u8 *manager = gNamedActorManager;
    s32 x, z, cell, height;
    if (!manager)
        return;
    if (!(*(u32 *)(o + 0xE0) & 0x10)) {
        if (!Player_TestAttachmentFlags(gUnk_02000580, 1))
            return;
        if (*(u32 *)(manager + 0x1370) != *(u32 *)(o + 0xDC))
            return;
        if (*(u32 *)(o + 0xE0) & 4)
            return;
    }
    {
        u16 *position = (u16 *)(o + 0x10C);
        x = (s16)position[0] >> 8;
        z = (s16)position[2] >> 8;
    }
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    height = (s16) * (u16 *)(o + 0x10E) >> 8;
    if (PlayExit(cell + 1, height))
        return;
    if (PlayExit(cell - 1, height))
        return;
    if (PlayExit(cell + SceneActorTerrain_Width(), height))
        return;
    if (CanExit(cell - SceneActorTerrain_Width(), height))
        Sound_PlaySong(0x151);
}
