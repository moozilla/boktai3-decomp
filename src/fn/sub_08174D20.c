#include "global.h"
#include "scene_input.h"

/* Local view of the HUD's contiguous widget storage and stat comparison values. */
struct InventoryStatMarkerView {
    u8 unk_00[0x1420];
    struct SceneInputWidget widgets[94];
    u8 unk_3760[0x114C];
    u8 comparisonValues[3];
    u8 unk_48AF;
    s16 comparisonDeltas[3];
    s16 comparisonOffsets[3];
};

enum { BASE_STAT_MARKER = 85, OFFSET_STAT_MARKER = 88, DELTA_STAT_MARKER = 91 };

#define WIDGET_OFFSET(index) ((u32) & ((struct InventoryStatMarkerView *)0)->widgets[index])

void InventoryGrid_LayoutStatMarkers(struct InventoryStatMarkerView *p)
{
    s32 i;
    for (i = 0; i < 3; i++) {
        u32 address = (u32)p + i * sizeof(struct SceneInputWidget);
        struct SceneInputWidget *w = (void *)(address + WIDGET_OFFSET(BASE_STAT_MARKER));
        w->flags &= ~1;
        p->widgets[i + BASE_STAT_MARKER].position.x = p->comparisonValues[i] - 100;
        p->widgets[i + BASE_STAT_MARKER].position.y = i * 8;
    }
    for (i = 0; i < 3; i++) {
        u32 address = (u32)p + i * sizeof(struct SceneInputWidget);
        struct SceneInputWidget *w = (void *)(address + WIDGET_OFFSET(OFFSET_STAT_MARKER));
        w->flags &= ~1;
        p->widgets[i + OFFSET_STAT_MARKER].position.x =
            p->widgets[i + BASE_STAT_MARKER].position.x + p->comparisonOffsets[i];
        p->widgets[i + OFFSET_STAT_MARKER].position.y = i * 8;
    }
    for (i = 0; i < 3; i++) {
        u32 address = (u32)p + i * sizeof(struct SceneInputWidget);
        struct SceneInputWidget *w = (void *)(address + WIDGET_OFFSET(DELTA_STAT_MARKER));
        w->flags &= ~1;
        p->widgets[i + DELTA_STAT_MARKER].position.x =
            p->widgets[i + OFFSET_STAT_MARKER].position.x + p->comparisonDeltas[i];
        p->widgets[i + DELTA_STAT_MARKER].position.y = i * 8;
    }
}
