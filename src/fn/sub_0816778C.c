#include "global.h"
#include "level_advance_display.h"
/* Tentative inventory display family, inferred from slot and item consumers. */
struct InventoryPanelCallback {
    struct LevelAdvanceParticle particle;
    void *callback;
};
struct InventoryPanelCallbackMetadata {
    u8 active;
    u8 unknown1[3];
    void *callback;
    u8 unknown8[0x38];
};
struct InventoryPanelCallbackGroup {
    void *context;
    u8 active, unknown5;
    u8 unknown6[2];
    struct InventoryPanelCallback entries[8];
};
struct InventoryPanel {
    u8 unknown0[0x4450];
    struct InventoryPanelCallbackGroup groups[2];
};
void InventoryPanel_InitCallbackGroups(struct InventoryPanel *panel)
{
    s32 i = 0;
    do {
        s32 offset = ((i << 6) + i) << 3;
        struct InventoryPanelCallbackGroup *group;
        s32 frame;
        s32 negative;
        u32 zero;
        s32 next;
        struct InventoryPanelCallbackMetadata *metadata;
        struct InventoryPanelCallback *entry;
        s32 j;
        offset += 0x4450;
        group = (struct InventoryPanelCallbackGroup *)((u32)panel + offset);
        frame = 0x91;
        if (i == 0)
            frame = 0x11;
        group->context = AnimData_FindEntry(0x1C1E);
        group->active = 0;
        group->unknown5 = 0;
        next = i + 1;
        negative = -4;
        zero = 0;
        metadata = (struct InventoryPanelCallbackMetadata *)((u8 *)group + 0x40);
        entry = group->entries;
        j = 7;
        do {
            sub_08217DE0(&entry->particle, group->context, frame);
            Effect_SetOrigin(&entry->particle, negative, negative);
            entry->particle.mode = zero;
            metadata->active = zero;
            metadata++;
            entry++;
        } while (--j >= 0);
        i = next;
    } while (i <= 1);
}
