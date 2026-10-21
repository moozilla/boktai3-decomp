// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_input.h"
void Storage_GetSlotPosition(void *, s32);
void SceneHud_SetKeyframePaletteBank(void *, u32);
void Storage_InitWidgets(u8 *p)
{
    struct ContactPackedVector pos;
    u32 zero;
    u32 *zeroPtr = &zero;
    struct SceneInputResource *r;
    s32 i;
    r = ScriptProc_Start(0xCB05, 0x5D04);
    *(struct SceneInputResource *)(p + 0x1538) = *r;
    ResourceHeader_Relocate(p + 0x1538, r);
    r = ScriptProc_Start(0xCB05, 0x530D);
    *(struct SceneInputResource *)(p + 0x1518) = *r;
    ResourceHeader_Relocate(p + 0x1518, r);
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    {
        register u32 blank asm("r1") = 0;
        Widget_InitLinked(p + 0x18, p + 0x1518, 0xD2, 0x30, blank, blank, 60, &pos);
    }
    Widget_InitLinked(p + 0x78, p + 0x1518, 0x41, 0x30, 0, 0, 60, &pos);
    pos.x += 8;
    Widget_InitLinked(p + 0xD8, p + 0x1518, 0x42, 0x30, 0, 0, 60, &pos);
    zero = 0;
    {
        register u32 *source asm("r0") = zeroPtr;
        register void *target asm("r1") = &pos;
        CpuSet(source, target, 0x05000002);
    }
    Widget_InitLinked(p + 0x138, p + 0x1518, 0x40, 0x30, 0, 0, 60, &pos);
    Widget_InitLinked(p + 0x198, p + 0x1518, 0xD3, 0x30, 0, 0, 60, &pos);
    Widget_InitLinked(p + 0x1F8, p + 0x1518, 0x63, 0x30, 0, 0, 60, &pos);
    for (i = 0; i < 16; i++) {
        Storage_GetSlotPosition(&pos, i);
        Widget_InitLinked(p + 0x318 + i * 0x60, p + 0x1538, 0x151, 0x30, 0, 0, 60, &pos);
        Storage_GetSlotPosition(&pos, i + 16);
        Widget_InitLinked(p + 0x918 + i * 0x60, p + 0x1538, 0x151, 0x30, 0, 0, 60, &pos);
    }
    pos.x = 0;
    pos.y = 128;
    Widget_InitLinked(p + 0x1098, p + 0x1538, 0xCF, 0x30, 0, 0, 60, &pos);
    pos.x = 216;
    pos.y = 104;
    Widget_InitLinked(p + 0xF18, p + 0x1538, 0x190, 0x30, 0, 0, 60, &pos);
    SceneHud_SetKeyframePaletteBank(p + 0xF18, 0x373);
    pos.x = 216;
    pos.y = 80;
    Widget_InitLinked(p + 0xFD8, p + 0x1538, 0x191, 0x30, 0, 0, 60, &pos);
    SceneHud_SetKeyframePaletteBank(p + 0xFD8, 0x373);
    pos.x = 216;
    pos.y = 56;
    Widget_InitLinked(p + 0x1038, p + 0x1538, 0x192, 0x30, 0, 0, 60, &pos);
    SceneHud_SetKeyframePaletteBank(p + 0x1038, 0x373);
    for (i = 0; i < 8; i++) {
        Storage_GetSlotPosition(&pos, i);
        Widget_InitLinked(p + 0x10F8 + i * 0x60, p + 0x1518, 0x50, 0x31, 0, 0, 60, &pos);
    }
    Widget_InitLinked(p + 0x14B8, p + 0x1518, 0x4E, 0x31, 0, 0, 60, &pos);
    pos.x = 144;
    pos.y = 32;
    Widget_InitLinked(p + 0x1458, p + 0x1518, 0x4D, 0x30, 0, 0, 60, &pos);
    Widget_InitLinked(p + 0x13F8, p + 0x1518, 0x4D, 0x31, 0, 0, 60, &pos);
    pos.x = 0;
    pos.y = 0;
    Widget_InitLinked(p + 0x258, p + 0x1518, 0x4B, 0x11, 0, 0, 60, &pos);
    SceneInput_InitMarkerPair((void *)(p + 0x1558));
    SceneInput_InitGlyphPair((void *)(p + 0x164C));
}
