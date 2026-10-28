// MATCHING_CONCESSION: register-pinning
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

/* Tentative fields used by the record-selection page. */
struct RecordMenuSelectionView {
    u8 unknown0[0x1C];
    void *otherMap;
    u8 unknown20[0x304];
    struct {
        u8 unknown0[8];
        u32 flags;
        u8 unknownC[0x54];
    } cursor;
    u8 cursorResource[0x20];
    u8 unknown3A4[0x30];
    u16 cursorGlyph;
    u8 unknown3D6[0x36];
    u8 palettePulse[0x20];
    u8 unknown42C[0x10AC];
    void (*update)(struct RecordMenu *);
    const u8 *titleScript;
    u32 choiceScript, recordScript;
    u8 unknown14E8[4];
    u8 leaving, selection, choice, pulsePhase;
    s32 textWindow;
    u8 unknown14F4[8];
    u8 redraw, unknown14FD;
    u16 inputDelay;
};

#define RM_FIELD(member)                                                                           \
    (*(typeof(((struct RecordMenuSelectionView *)0)->member)                                       \
           *)((u8 *)menu + (u32) & ((struct RecordMenuSelectionView *)0)->member))

static inline void RecordMenu_SetRedraw(struct RecordMenu *menu, u16 offset)
{
    *((u8 *)menu + offset) = 1;
}

// scope shape 59 (differences once low registers are set aside)
// confidence=low: record menu cursor input, confirm/cancel and redraw handling
void RecordMenu_UpdateSelection(struct RecordMenu *menu)
{
    u32 layer;
    u8 *redraw = &menu->redraw;
    register u16 keys asm("r1");
    register u16 *glyph asm("r4");

    if (*redraw) {
        LinkLobbyPanel_ClearTextWindows(menu);
        LinkLobbyPanel_HideSprites(menu);
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        layer = 3;
        Bg_SetupLayers(2, 0, menu->otherMap, 0, 0, 1, &layer);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        TextWindow_SetBounds(RM_FIELD(textWindow), 1, 6, 0x1c, 6);
        TextWindow_SetScript(RM_FIELD(textWindow), RM_FIELD(titleScript));
        TextWindow_Refresh(RM_FIELD(textWindow), 7);
        TextWindow_StartPlayback(RM_FIELD(textWindow));
        RM_FIELD(cursor.flags) &= ~1;
        RM_FIELD(choice) = 1;
        menu->inputDelay = 0;
        *redraw = 0;
    }

    Palette_UpdateHighlightPulse(&RM_FIELD(palettePulse), &RM_FIELD(pulsePhase));
    keys = gUnk_03005260[1];
    {
        u32 left = 0x20;
        left &= keys;
        if (left) {
            if (RM_FIELD(choice) != 0)
                goto done;
            glyph = &RM_FIELD(cursorGlyph);
            *glyph = 0x4B;
            RM_FIELD(choice) = 1;
            goto anim;
        }
        if (!(keys & 0x10))
            goto select;
        {
            register u32 choiceOffset asm("r3") =
                (u32) & ((struct RecordMenuSelectionView *)0)->choice;
            register u8 *choice;
            register u32 zero asm("r1");
            asm("" : "+r"(choiceOffset));
            choice = (u8 *)menu + choiceOffset;
            if (*choice == 0)
                goto done;
            {
                register u32 glyphOffset asm("r0") =
                    (u32) & ((struct RecordMenuSelectionView *)0)->cursorGlyph;
                asm("" : "+r"(glyphOffset));
                glyph = (u16 *)((u8 *)menu + glyphOffset);
            }
            zero = 0;
            *glyph = 0x4C;
            *choice = zero;
        }
    anim:
        Sound_PlaySong(0xDC);
        Anim_SelectWidgetGlyph(&RM_FIELD(cursor), &RM_FIELD(cursorResource), *glyph);
        goto done;
    select:
        if (1 & keys) {
            RM_FIELD(cursor.flags) |= 1;
            Registry1745_SetSelectionResponse(RM_FIELD(choice));
            {
                void (*callback)(struct RecordMenu *) = SceneTask_UpdateTextDelay;
                u32 leaving = 2;
                menu->update = callback;
                RecordMenu_SetRedraw(menu, (u32) & ((struct RecordMenuSelectionView *)0)->redraw);
                RM_FIELD(leaving) = leaving;
            }
            if (RM_FIELD(choice) != 0)
                Sound_PlaySong(0xDD);
            else
                Sound_PlaySong(0xDE);
            goto done;
        }
        if (2 & keys) {
            RM_FIELD(cursor.flags) |= 1;
            Registry1745_SetSelectionResponse(0);
            {
                void (*callback)(struct RecordMenu *) = SceneTask_UpdateTextDelay;
                u32 leaving = 2;
                menu->update = callback;
                RecordMenu_SetRedraw(menu, (u32) & ((struct RecordMenuSelectionView *)0)->redraw);
                RM_FIELD(leaving) = leaving;
            }
            Sound_PlaySong(0xDE);
        }
    done:;
    }
}
