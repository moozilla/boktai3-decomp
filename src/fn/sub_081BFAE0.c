#define Storage_RefreshCategoryWidgets sub_081BFAE0
#define Storage_UpdateSwapSelection sub_081BF04C
#define Storage_RefreshItemSlots sub_081BE16C
#define Storage_RefreshEquipmentSlots sub_081BE504
#define Storage_RefreshConsumableSlots sub_081BE2E0
#include "global.h"
#include "scene_input_navigation.h"
struct StorageSlotFlags {
    u32 flags;
    u8 unknown4[0x5C];
};
struct StorageCategoryWidgets {
    u8 unknown0[0x158];
    struct SceneInputPosition categoryIconPosition;
    u8 unknown160[0x1100 - 0x160];
    struct StorageSlotFlags slots[8];
    u8 unknown1400[0x164C - 0x1400];
    struct SceneInputPair glyphs;
    u8 unknown1740[0xC];
    u8 selectingMode;
    u8 unknown174D[3];
    u32 mode;
    u8 category;
    u8 unknown1755[3];
    u8 categoryKinds[0x17F4 - 0x1758];
    void (*state)(struct StorageCategoryWidgets *);
};
void sub_081BE16C(struct StorageCategoryWidgets *);
void sub_081BE504(struct StorageCategoryWidgets *);
void sub_081BE2E0(struct StorageCategoryWidgets *);
void sub_081BF04C(struct StorageCategoryWidgets *);
void sub_081BFAE0(struct StorageCategoryWidgets *p)
{
    u32 hide = 1;
    s32 i;
    struct StorageSlotFlags *widget = p->slots;
    for (i = 7; i >= 0; i--) {
        widget->flags |= hide;
        widget++;
    }
    switch (p->categoryKinds[p->category] >> 1) {
    case 0:
        sub_081BE16C(p);
        break;
    case 1:
        sub_081BE504(p);
        break;
    case 2:
        sub_081BE2E0(p);
        break;
    }
    if (p->selectingMode) {
        if (((u8 *)p)[0x1763] == 0x21)
            SceneInput_SetGlyphPair(&p->glyphs, 0, 4);
        else
            SceneInput_SetGlyphPair(&p->glyphs, 0, -1);
    } else {
        if (p->mode == 1) {
            if (p->state == sub_081BF04C)
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
        struct SceneInputPosition *position = &p->categoryIconPosition;
        position->x = (categoryKind << 3) + 16;
        position->y = 0;
        position->z = 0;
    }
}
