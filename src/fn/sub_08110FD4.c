#include "global.h"
#include "background_controller.h"
#include "render.h"
#define TilemapPanel_LoadScriptMap SceneTilemap_LoadLayoutFromScript
#define gTilemapPanel              gUnk_020004B8
struct TilemapPanel {
    u8 pad0[0x1c];
    const u8 *tileData;
    u8 *mapScript;
    u8 *map;
    s32 mapSize;
    u8 *tileListScript;
    u16 *tiles;
    s32 tileCount;
    s32 value38;
    u8 pad3c[4];
    s32 value40;
    u8 pad44[0x10];
    s32 width, height;
    u16 *screenMap;
};
extern struct TilemapPanel *gUnk_020004B8;

extern const u8 gUnk_0860B2D0[], gUnk_08607DB0[], gUnk_08606550[];
extern const u8 gUnk_0860E3D0[], gUnk_0860A470[], gUnk_08609610[];
void SpriteOam_QueueProjectedSprites(void), Sprite_RenderScaledList(void), sub_08219624(void);
void Video_SetRenderCallbacks(s32, void *, void *, void *);
void SceneTilemap_LoadLayoutFromScript(struct TilemapPanel *),
    TilemapPanel_LoadScriptTileList(struct TilemapPanel *);
void *Video_GetBackgroundMap(u32);
void Terrain_SetColumnLayout(void *), Terrain_SetColumnDefinitions(const void *);
void Bg_SetControlFields(s32, u32, u32, u32, u32, u32);
void Bg_SetTileUpload(u32, const void *, u16, u16);
void Bg_SetPriority(u32, s32);
void TilemapPanel_DrawRegion(struct TilemapPanel *, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                             s32);
void SceneEffect_ExecuteCommandList(u16 *, s32);
s32 TilemapPanel_Initialize(struct TilemapPanel *p)
{
    s32 choice;
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                             sub_08219624);
    SceneTilemap_LoadLayoutFromScript(p);
    TilemapPanel_LoadScriptTileList(p);
    p->screenMap = Video_GetBackgroundMap(2);
    p->width = p->mapSize;
    p->height = 1;
    Terrain_SetColumnLayout(p->map);
    Bg_SetControlFields(2, 0, 29, 0, 0, 0);
    choice = Script_GetKeywordValue('c', 0);
    switch (choice) {
        case 0:
            Bg_SetTileUpload(2, gUnk_0860B2D0, 0, 0x188);
            Terrain_SetColumnDefinitions(gUnk_08607DB0);
            p->tileData = gUnk_08606550;
            break;
        case 1:
            Bg_SetTileUpload(2, gUnk_0860E3D0, 0, 0x117);
            Terrain_SetColumnDefinitions(gUnk_0860A470);
            p->tileData = gUnk_08609610;
            break;
    }
    Bg_SetPriority(2, 3);
    *(vu16 *)0x04000018 = 0;
    {
        u32 mask = 0x400;
        gDispCntShadow = mask | gDispCntShadow;
    }
    gBgRecords[2].y = 0;
    TilemapPanel_DrawRegion(p, 0, 0, 0, 0, 8, 1, 4, 20, 0, 0);
    p->value40 = 0x100;
    gUnk_020004B8 = p;
    SceneEffect_ExecuteCommandList(p->tiles, p->tileCount);
    p->value38 = 0xa000;
    return 0;
}
