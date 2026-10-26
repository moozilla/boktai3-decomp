#include "global.h"
#include "render.h"
// Provisional name: SoundTest_Initialize
struct Sprite {
    u8 head[8];
    u32 flags;
    u8 gap[0x20 - 12];
    s16 x, y;
    u8 tail[0x60 - 0x24];
};
struct Resource {
    u32 words[8];
};
struct Viewer {
    u8 p0[0x20];
    struct Sprite background;
    struct Resource backgroundResource;
    u8 p1[8];
    struct Sprite panel;
    struct Resource panelResource;
    u8 p2[8];
    struct Sprite cursor;
    struct Resource cursorResource;
    u8 p3[8];
    struct Sprite label;
    struct Resource labelResource;
    u8 p4[8];
    struct Resource digitsResource;
    struct Sprite digitsA[3], digitsB[3], labels[3];
    s32 state, exitFlag, exitTimer, fadeShift, selection, countA, countB, countC, textIndex, valueA,
        valueB, previousA, previousB, previousC, repeatTimer;
    const u16 *savedPalette;
    u16 palette[16];
};
void Anim_LoadKeyframe(void *, void *, u16, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
extern const s16 gUnk_086136F0[], gUnk_086137AE[], gUnk_08613BC4[];
u32 Sound_IsSongPlaying(s32);
void Sound_StopSong(s32);
void Sound_PlaySong(s32);
void Anim_SetKeyframes(void *, void *, u16, s32, s32);
void Bg_ClearTilemap(s32);
void BgGfx_LoadTilemapSimple(s32, s32, s32, s32, s32);
void *ScriptProc_Start(s32, s32);
void Bg_SetupLayers(s32, s32, void *, s32, s32, s32, void *);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void Menu_StartFadeIn(s32);
void SoundTest_CountEntries(void *);
void SoundTest_ShowCategory(void *);
void SoundTest_MoveCategoryCursor(void *);
void SoundTest_DrawTrackName(void *);
void SoundTest_DrawSecondCategoryIndex(void *);
void SoundTest_DrawThirdCategoryIndex(void *);
struct Vec {
    s16 x, y, z, w;
};
static inline void clear(u32 *value, void *dest)
{
    *value = 0;
    CpuSet(value, dest, 0x05000002);
}
// Load sound-test assets and initialize its category labels, counters and palette.
s32 SoundTest_Initialize(struct Viewer *p)
{
    struct Vec pos;
    u32 zero;
    void *asset;
    s32 option, i;
    clear(&zero, &pos);
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    {
        void *background = ScriptProc_Start(0xc091, 0x3536);
        option = 10;
        Bg_SetupLayers(2, 0, background, 0, 0, 1, &option);
    }
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet((u8 *)ScriptProc_Start(0x92b3, 0x204) + 20, gBgPaletteBuffer, 256);
    asset = ScriptProc_Start(0xcb05, 0xe2ab);
    ResourceHeader_Relocate(&p->backgroundResource, asset);
    p->digitsResource = p->backgroundResource;
    Widget_InitLinked(&p->background, &p->backgroundResource, 59, 16, 0, 0, 0, 0);
    ResourceHeader_Relocate(&p->labelResource, asset);
    Widget_InitLinked(&p->label, &p->labelResource, 47, 16, 0, 0, 0, 0);
    for (i = 0; i < 3; i++) {
        Widget_InitLinked(&p->labels[i], &p->digitsResource, 43, 48, 1, 0, 60, &pos);
        p->labels[i].flags |= 1;
        pos.y += 32;
    }
    pos.x = 184;
    pos.y = 64;
    for (i = 0; i < 3; i++) {
        Widget_InitLinked(&p->digitsA[i], &p->digitsResource, 0, 48, 0, 0, 60, &pos);
        p->digitsA[i].flags |= 1;
        pos.x += 8;
    }
    pos.x = 184;
    pos.y = 96;
    for (i = 0; i < 3; i++) {
        Widget_InitLinked(&p->digitsB[i], &p->digitsResource, 0, 48, 0, 0, 60, &pos);
        p->digitsB[i].flags |= 1;
        pos.x += 8;
    }
    asset = ScriptProc_Start(0xcb05, 0x530d);
    ResourceHeader_Relocate(&p->panelResource, asset);
    Widget_InitLinked(&p->panel, &p->panelResource, 139, 16, 0, 0, 0, 0);
    ResourceHeader_Relocate(&p->cursorResource, asset);
    Widget_InitLinked(&p->cursor, &p->cursorResource, 71, 16, 0, 0, 0, 0);
    p->savedPalette = *(u16 **)((u8 *)p + 0x178);
    CpuSet(p->savedPalette, p->palette, 16);
    *(u16 **)((u8 *)p + 0x178) = p->palette;
    p->selection = 0;
    p->fadeShift = 3;
    Menu_StartFadeIn(3);
    SoundTest_CountEntries(p);
    SoundTest_ShowCategory(p);
    SoundTest_MoveCategoryCursor(p);
    SoundTest_DrawTrackName(p);
    SoundTest_DrawSecondCategoryIndex(p);
    SoundTest_DrawThirdCategoryIndex(p);
    return 0;
}
