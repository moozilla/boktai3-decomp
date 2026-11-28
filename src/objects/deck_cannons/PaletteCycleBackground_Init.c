#include "global.h"
#include "objects/deck_cannons/script_value_rotation_layout.h"
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void ResourceHeader_Relocate(void *, void *);
void AnimSprite_InitLinked(u8 *, void *, u32, u32, u32, u32, u32, void *);
extern u8 gUnk_0824F6F8[];
u32 PaletteCycleBackground_Init(struct ScriptValueRotation *rotation)
{
    struct Blob {
        u32 words[8];
    } resource;
    u32 layers[2];
    void *p = ScriptProc_Start(0xC091, 0xCF15);

    layers[0] = 3;
    layers[1] = 4;
    Bg_SetupLayers(1, 0, p, 0, 0, 2, layers);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    p = ScriptProc_Start(0xCB05, 0x7C8B);
    resource = *(struct Blob *)p;
    ResourceHeader_Relocate(&resource, p);
    AnimSprite_InitLinked(rotation->animation, &resource, 1, 0x10, 0, 0, 0x3C, gUnk_0824F6F8);
    rotation->startIndex = 1;
    rotation->ticks = 0;
    return 0;
}
