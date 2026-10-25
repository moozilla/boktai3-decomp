#include "global.h"

#include "record_menu.h"

extern u16 gUnk_03005260[];
void Bg_ClearTilemap(s32);
void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void LinkLobbyPanel_ClearTextWindows(struct RecordMenu *);
void LinkLobbyPanel_HideSprites(struct RecordMenu *);
void Palette_UpdateHighlightPulse(void *, void *);
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
void TextWindow_SetScript(s32, const u8 *);
void TextWindow_Refresh(u32, u32);
void TextWindow_StartPlayback(s32);
s32 Anim_SelectWidgetGlyph(void *, void *, u32);
void Registry1745_SetSelectionResponse(s32);
void Sound_PlaySong(u32);
void SceneTask_UpdateTextDelay(struct RecordMenu *);

#define RMS_AT(T, off) (*(T *)((u8 *)menu + (off)))

#ifdef NONMATCHING
// fuzzy: 69.6%
// scope shape 59 (differences once low registers are set aside)
// confidence=low: record menu cursor input, confirm/cancel and redraw handling
void RecordMenu_UpdateSelection(struct RecordMenu *menu)
{
    u32 layer;
    u16 keys;

    if (menu->redraw) {
        LinkLobbyPanel_ClearTextWindows(menu);
        LinkLobbyPanel_HideSprites(menu);
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        layer = 3;
        Bg_SetupLayers(2, 0, menu->otherMap, 0, 0, 1, &layer);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        TextWindow_SetBounds(RMS_AT(s32, 0x14F0), 1, 6, 0x1c, 6);
        TextWindow_SetScript(RMS_AT(s32, 0x14F0), RMS_AT(const u8 *, 0x14DC));
        TextWindow_Refresh(RMS_AT(s32, 0x14F0), 7);
        TextWindow_StartPlayback(RMS_AT(s32, 0x14F0));
        RMS_AT(u32, 0x32C) &= ~1;
        RMS_AT(u8, 0x14EE) = 1;
        menu->inputDelay = 0;
        menu->redraw = 0;
    }

    Palette_UpdateHighlightPulse((u8 *)menu + 0x40C, (u8 *)menu + 0x14EF);
    keys = gUnk_03005260[1];
    if (keys & 0x20) {
        if (RMS_AT(u8, 0x14EE) != 0)
            goto done;
        RMS_AT(u16, 0x3D4) = 0x4B;
        RMS_AT(u8, 0x14EE) = 1;
        goto anim;
    }
    if (!(keys & 0x10))
        goto select;
    if (RMS_AT(u8, 0x14EE) == 0)
        goto done;
    RMS_AT(u16, 0x3D4) = 0x4C;
    RMS_AT(u8, 0x14EE) = 0;
anim:
    Sound_PlaySong(0xDC);
    Anim_SelectWidgetGlyph((u8 *)menu + 0x324, (u8 *)menu + 0x384, RMS_AT(u16, 0x3D4));
    goto done;
select:
    if (keys & 1) {
        RMS_AT(u32, 0x32C) |= 1;
        Registry1745_SetSelectionResponse(RMS_AT(u8, 0x14EE));
        menu->update = (void (*)(struct RecordMenu *))SceneTask_UpdateTextDelay;
        menu->redraw = 1;
        RMS_AT(u8, 0x14EC) = 2;
        if (RMS_AT(u8, 0x14EE) != 0)
            Sound_PlaySong(0xDD);
        else
            Sound_PlaySong(0xDE);
        goto done;
    }
    if (keys & 2) {
        RMS_AT(u32, 0x32C) |= 1;
        Registry1745_SetSelectionResponse(0);
        menu->update = (void (*)(struct RecordMenu *))SceneTask_UpdateTextDelay;
        menu->redraw = 1;
        RMS_AT(u8, 0x14EC) = 2;
        Sound_PlaySong(0xDE);
    }
done:;
}
#else
INCLUDE_ASM("asm/nonmatching", RecordMenu_UpdateSelection);
#endif
