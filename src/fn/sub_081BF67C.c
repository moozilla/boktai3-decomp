#include "global.h"
#include "storage_screen.h"
void Storage_GetSlotPosition(void *, s32);
void SceneHud_SetKeyframePaletteBank(void *, u32);

void Storage_InitWidgets(struct StorageScreen *p)
{
    struct SceneInputPosition pos;
    struct SceneInputResource *r;
    struct SceneWidgetPlayback labelPlayback;
    u8 labelPriority;
    s32 i;
    r = ScriptProc_Start(0xCB05, 0x5D04);
    p->storageResource = *r;
    ResourceHeader_Relocate(&p->storageResource, r);
    r = ScriptProc_Start(0xCB05, 0x530D);
    p->commonResource = *r;
    ResourceHeader_Relocate(&p->commonResource, r);
    labelPlayback.priority = 0;
    labelPlayback.mode = 0;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    labelPriority = labelPlayback.priority;
    Widget_InitLinked(&p->controls[0], &p->commonResource, 0xD2, 0x30, labelPriority,
                      labelPlayback.mode, 60, &pos);
    Widget_InitLinked(&p->controls[1], &p->commonResource, 0x41, 0x30, 0, 0, 60, &pos);
    pos.x += 8;
    Widget_InitLinked(&p->controls[2], &p->commonResource, 0x42, 0x30, 0, 0, 60, &pos);
    ZeroFill8(&pos);
    Widget_InitLinked(&p->controls[3], &p->commonResource, 0x40, 0x30, 0, 0, 60, &pos);
    Widget_InitLinked(&p->controls[4], &p->commonResource, 0xD3, 0x30, 0, 0, 60, &pos);
    Widget_InitLinked(&p->controls[5], &p->commonResource, 0x63, 0x30, 0, 0, 60, &pos);
    for (i = 0; i < 16; i++) {
        Storage_GetSlotPosition(&pos, i);
        Widget_InitLinked(&p->firstSlots[i], &p->storageResource, 0x151, 0x30, 0, 0, 60, &pos);
        Storage_GetSlotPosition(&pos, i + 16);
        Widget_InitLinked(&p->secondSlots[i], &p->storageResource, 0x151, 0x30, 0, 0, 60, &pos);
    }
    pos.x = 0;
    pos.y = 128;
    Widget_InitLinked(&p->indicators[4], &p->storageResource, 0xCF, 0x30, 0, 0, 60, &pos);
    pos.x = 216;
    pos.y = 104;
    Widget_InitLinked(&p->indicators[0], &p->storageResource, 0x190, 0x30, 0, 0, 60, &pos);
    SceneHud_SetKeyframePaletteBank(&p->indicators[0], 0x373);
    pos.x = 216;
    pos.y = 80;
    Widget_InitLinked(&p->indicators[2], &p->storageResource, 0x191, 0x30, 0, 0, 60, &pos);
    SceneHud_SetKeyframePaletteBank(&p->indicators[2], 0x373);
    pos.x = 216;
    pos.y = 56;
    Widget_InitLinked(&p->indicators[3], &p->storageResource, 0x192, 0x30, 0, 0, 60, &pos);
    SceneHud_SetKeyframePaletteBank(&p->indicators[3], 0x373);
    for (i = 0; i < 8; i++) {
        Storage_GetSlotPosition(&pos, i);
        Widget_InitLinked(&p->categorySlots[i], &p->commonResource, 0x50, 0x31, 0, 0, 60, &pos);
    }
    Widget_InitLinked(&p->labels[2], &p->commonResource, 0x4E, 0x31, 0, 0, 60, &pos);
    pos.x = 144;
    pos.y = 32;
    Widget_InitLinked(&p->labels[1], &p->commonResource, 0x4D, 0x30, 0, 0, 60, &pos);
    Widget_InitLinked(&p->labels[0], &p->commonResource, 0x4D, 0x31, 0, 0, 60, &pos);
    pos.x = 0;
    pos.y = 0;
    Widget_InitLinked(&p->controls[6], &p->commonResource, 0x4B, 0x11, 0, 0, 60, &pos);
    SceneInput_InitMarkerPair(&p->markers);
    SceneInput_InitGlyphPair(&p->glyphs);
}
