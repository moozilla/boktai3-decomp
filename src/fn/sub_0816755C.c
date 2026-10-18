#include "global.h"
#include "level_advance_display.h"
/* Tentative inventory display family, inferred from slot and item consumers. */
#define InventoryPanel_UpdateCallbackGroups sub_0816755C
struct InventoryPanelCallbackMetadata {
    u8 active;
    u8 unknown1[3];
    void *callback;
    u8 unknown8[0x38];
};
struct InventoryPanelCallback {
    struct LevelAdvanceParticle particle;
    void *callback;
};
struct InventoryPanelCallbackGroup {
    void *context;
    u8 active;
    u8 unknown5[3];
    struct InventoryPanelCallback entries[8];
};
struct InventoryPanel {
    u8 unknown0[0x4450];
    struct InventoryPanelCallbackGroup groups[2];
};
void sub_0816755C(struct InventoryPanel *panel)
{
    s32 i = 0;
    do {
        s32 offset = ((i << 6) + i) << 3;
        struct InventoryPanelCallbackGroup *group;
        s32 enabled;
        s32 next;
        offset += 0x4450;
        group = (struct InventoryPanelCallbackGroup *)((u32)panel + offset);
        enabled = group->active;
        next = i + 1;
        if (enabled) {
            s32 active = 0;
            struct InventoryPanelCallbackMetadata *metadata =
                (struct InventoryPanelCallbackMetadata *)((u8 *)group + 0x40);
            struct InventoryPanelCallback *entry = group->entries;
            s32 j = 7;
            do {
                if (metadata->active &&
                    sub_08249240(&entry->particle, group->context, metadata->callback))
                    active = 1;
                metadata++;
                entry++;
            } while (--j >= 0);
            group->active = active;
        }
        i = next;
    } while (i <= 1);
}
