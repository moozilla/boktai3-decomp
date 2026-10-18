#define SelectionPanel_RefreshVisibleEntries sub_08168FC4
#define SelectionPanel_LoadSelectionMarker PauseMenu_LoadIndexedIconKeyframe
#include "global.h"
struct PanelEntry {
    u8 unknown;
    s8 id;
    u8 pad[2];
};
struct SelectionPanel {
    u8 pad[0xC70];
    u8 count;
    u8 unknownC71;
    u8 selection;
    u8 padC73[5];
    struct PanelEntry entries[4];
    u8 sprites[3][0x60];
    u8 padDA8[0x2E8];
    u8 marker[0x60];
};
void PauseMenu_LoadAttributeIconFrame(struct SelectionPanel *, void *, s32);
void PauseMenu_LoadIndexedIconKeyframe(struct SelectionPanel *, void *, s32);
void sub_08168FC4(struct SelectionPanel *p)
{
    s32 i, index;
    u8 *sprite;
    i = 0;
    sprite = p->sprites[0];
    for (; i <= 2; i++) {
        index = i + (s8)p->selection;
        if (index >= p->count)
            index -= p->count;
        PauseMenu_LoadAttributeIconFrame(p, sprite, p->entries[index].id);
        sprite += 0x60;
    }
    PauseMenu_LoadIndexedIconKeyframe(p, p->marker, (s8)p->selection);
}
