#include "global.h"
#include "ui/widgets/scene_input_navigation.h"
#include "ui/town/storage_screen.h"
void Storage_RefreshItemSlots(struct StorageScreen *);
void Storage_RefreshEquipmentSlots(struct StorageScreen *);
void Storage_RefreshConsumableSlots(struct StorageScreen *);
void Storage_UpdateSwapSelection(struct StorageScreen *);
void Storage_RefreshCategoryWidgets(struct StorageScreen *p)
{
    s32 i;
    for (i = 0; i < 8; i++)
        p->categorySlots[i].flags |= 1;
    switch (p->categoryKinds[p->category] >> 1) {
        case 0:
            Storage_RefreshItemSlots(p);
            break;
        case 1:
            Storage_RefreshEquipmentSlots(p);
            break;
        case 2:
            Storage_RefreshConsumableSlots(p);
            break;
    }
    if (p->interactionMode) {
        if (p->selection == 0x21)
            SceneInput_SetGlyphPair(&p->glyphs, 0, 4);
        else
            SceneInput_SetGlyphPair(&p->glyphs, 0, -1);
    } else {
        if (p->mode == 1) {
            if (p->state == (u32)Storage_UpdateSwapSelection)
                SceneInput_SetGlyphPair(&p->glyphs, 0, 3);
            else
                SceneInput_SetGlyphPair(&p->glyphs, 0, -1);
        } else if (p->mode == 2) {
            SceneInput_SetGlyphPair(&p->glyphs, 0, -1);
        } else {
            SceneInput_SetGlyphPair(&p->glyphs, -1, -1);
        }
    }
    {
        u32 categoryKind = p->categoryKinds[p->category];
        struct SceneInputPosition *position = &p->controls[3].position;
        position->x = (categoryKind << 3) + 16;
        position->y = 0;
        position->z = 0;
    }
}
