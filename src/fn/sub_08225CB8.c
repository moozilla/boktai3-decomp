#include "global.h"
#include "render.h"
#include "engine.h"
struct Grid {
    u8 pad0[8];
    s16 originX, originY; /* BG map pixels * 16 */
};
struct TerrainTask {
    u32 unk0;
    struct Grid *grid;
};
void Terrain_LoadHP(s32 hp);
void Terrain_LoadGridResource(u16);
void Terrain_LoadBoxesResource(u16);
void Terrain_LoadPathsResource(u16);
void sub_0821F15C(u16);
struct TerrainTask *ResourceRegistry_GetResource(u32);
void Video_SetRenderCallbacks(s32, void *, void *, void *);
void SpriteOam_QueueProjectedSprites(void);
void Sprite_RenderScaledList(void);
void sub_08219624(void);
void SpriteOam_QueueCameraRelativeSprites(void);
void sub_08214D20(void);
void sub_0821964C(void);
/* Script: loads the stage terrain, either from one HP resource (-p) or from
   separate grid (-h), -t, -r and -z resources; -n is read and ignored. -v 0
   installs the default map callbacks and aligns the BG offset with the grid
   origin; another -v value installs the alternative set with offset 0. */
void Terrain_LoadFromScript(void)
{
    s32 grid, t, r, z, hp;
    struct Grid *g;
    Script_GetKeywordValue('n', 0);
    grid = Script_GetKeywordValue('h', 0);
    t = Script_GetKeywordValue('t', 0);
    r = Script_GetKeywordValue('r', 0);
    z = Script_GetKeywordValue('z', 0);
    hp = Script_GetKeywordValue('p', 0);
    if (hp)
        Terrain_LoadHP(hp);
    else {
        if (grid)
            Terrain_LoadGridResource(grid);
        if (t)
            Terrain_LoadBoxesResource(t);
        if (r)
            Terrain_LoadPathsResource(r);
        if (z)
            sub_0821F15C(z);
    }
    g = ResourceRegistry_GetResource(0x56C2)->grid;
    if (Script_SeekToKeyword('v')) {
        s32 mode = Script_GetValue();
        if (mode == 0) {
            Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                                     sub_08219624);
            gCamera.offsetX = g->originX >> 4;
            gCamera.offsetY = g->originY >> 4;
        } else {
            Video_SetRenderCallbacks(mode, SpriteOam_QueueCameraRelativeSprites, sub_08214D20,
                                     sub_0821964C);
            gCamera.offsetX = 0;
            gCamera.offsetY = 0;
        }
    }
}
