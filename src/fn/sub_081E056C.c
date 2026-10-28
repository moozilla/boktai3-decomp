#include "global.h"
#include "render.h"
#include "menu_parameters.h"

extern const u8 gUnk_086136D8[];
extern const u8 gUnk_086136E4[];
void EncodedRecord_EncodeFields(void *);
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
u8 *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
s32 ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
s32 TextWindow_Create(u32, u32, u32, u32);
void TextWindow_SetString(int, int, void *);
s32 TextWindow_SetScript(s32 id, const u8 *script);
s32 TextWindow_Refresh(s32 id, s32 index);
void TextWindow_StartPlayback(s32);
void Menu_StartFadeIn(void);

s32 sub_081E056C(u8 *p)
{
    u8 *src;
    s32 row, i, j, code;
    u8 *out;
    s32 *w;
    u32 size;

    EncodedRecord_EncodeFields(p + 0x4a8);
    src = p + 0x4a8;
    row = 0;
    do {
        out = p + 0xa8 + (row << 8);
        i = 0;
        do {
            code = *src++ << 1;
            if (code > 0x59) {
                j = 0;
                do {
                    *out++ = gUnk_086136D8[j];
                    j++;
                } while (j <= 7);
            }
            {
                u8 *m = gScriptMessageParameters.text + code;
                *out++ = m[0];
                *out++ = m[1];
            }
            if (code > 0x59) {
                j = 0;
                do {
                    *out++ = gUnk_086136E4[j];
                    j++;
                } while (j <= 8);
            }
            i++;
        } while (i <= 5);
        *out = 0;
        row++;
    } while (row <= 3);

    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    {
        u8 *res = ScriptProc_Start(0xc091, 0x3536);
        *(u8 **)(p + 0x9c) = res;
        size = 12;
        Bg_SetupLayers(2, 0, res, 0, 0, 1, &size);
    }
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(ScriptProc_Start(0x92b3, 0x204) + 0x14, gBgPaletteBuffer, 0x100);
    ResourceHeader_Relocate(p + 0x78, ScriptProc_Start(0xcb05, 0xe2ab));
    Widget_InitLinked(p + 0x18, p + 0x78, 0x39, 0x10, 0, 0, 0, 0);

    w = (s32 *)(p + 0x4c0);
    if ((*w = TextWindow_Create(2, 5, 28, 2)) < 0)
        return -1;
    for (i = 0; i <= 3; i++)
        TextWindow_SetString(*w, i, p + 0xa8 + i * 0x100);
    TextWindow_SetScript(*w, gScriptMessageParameters.pc);
    TextWindow_Refresh(*w, 0);
    TextWindow_StartPlayback(*w);

    w = (s32 *)(p + 0x4c4);
    if ((*w = TextWindow_Create(6, 17, 28, 1)) < 0)
        return -1;
    TextWindow_SetScript(*w, gScriptMessageParameters.pc);
    TextWindow_Refresh(*w, 1);
    TextWindow_StartPlayback(*w);

    w = (s32 *)(p + 0x4c8);
    if ((*w = TextWindow_Create(2, 11, 28, 2)) < 0)
        return -1;
    TextWindow_SetScript(*w, gScriptMessageParameters.pc);
    TextWindow_Refresh(*w, 2);
    TextWindow_StartPlayback(*w);

    *(u32 *)(p + 0xa4) = 0;
    Menu_StartFadeIn();
    return 0;
}
