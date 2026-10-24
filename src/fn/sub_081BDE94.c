// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_input_grid.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining: transition-array pointer induction, stack lifetimes and register allocation.
struct SwapWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x54];
};
struct SwapFlags {
    u8 unknown0[0x20];
    struct {
        u32 flags;
        u8 unknown4[0x5c];
    } widgets[32];
};
s32 Inventory_GetItemId(u32);
s32 Inventory_GetEquipmentId(u32);
s32 Inventory_GetConsumableId(u32);
u32 Selection_GetDisplayFrame(s32);
s32 Storage_FindEquippedItemSlot(void *, s32);
s32 GameState_IsConsumableSlotEquipped(u32);
void Storage_GetSlotPosition(struct SceneInputPosition *, u32);
void Sound_PlaySong(u32);
struct SwapCategories {
    u8 unknown0[0x1754];
    u8 bank;
    u8 unknown1755[3];
    u8 categories[3];
};
void Storage_BeginSwapTransition(u8 *p)
{
    struct SceneInputPosition starts[2];
    struct SceneInputPosition targets[2];
    u32 enabled[2];
    u32 frames[2];
    u32 attributes[2];
    s32 mode = ((struct SwapCategories *)p)->categories[((struct SwapCategories *)p)->bank] >> 1;
    u8 *slots = p + 0x1790;
    s32 i;
    for (i = 0; i < 2; i++) {
        u32 selected;
        if (i == 0)
            selected = p[0x1766];
        else
            selected = p[0x1763];
        ((struct SwapFlags *)p)->widgets[selected + 8].flags |= 1;
        switch (mode) {
            case 0: {
                register s32 id asm("r2") = Inventory_GetItemId(slots[selected]);
                enabled[i] = id >= 0;
                frames[i] = Selection_GetDisplayFrame(id);
                break;
            }

            case 1: {
                s32 slot = Storage_FindEquippedItemSlot(p, slots[selected]);
                register s32 id asm("r2");
                if (slot >= 0)
                    ((struct SwapWidget *)(p + 0x10F8))[slot].flags |= mode;
                id = (u8)Inventory_GetEquipmentId(slots[selected]);
                enabled[i] = id != 255;
                break;
                frames[i] = id + 0xF0;
            }

            case 2: {
                s32 slot;
                s8 id;
                struct SwapWidget *widget = (void *)(p + 0x10F8);
                for (slot = 0; slot < 32; slot++) {
                    if ((Inventory_GetConsumableId(slots[slot]) != (-1)) &&
                        GameState_IsConsumableSlotEquipped(slots[slot])) {
                        if ((slot == p[0x1763]) || (slot == p[0x1766])) {
                            widget->flags |= 1;
                            widget++;
                        }
                    }
                }

                id = *((s16 *)((((u8 *)gUnk_02000710) + 0x160) + (slots[selected] * 2)));
                enabled[i] = id >= 0;
                frames[i] = id + 0x121;
                break;
            }
        }

        attributes[i] = 0x373;
        Storage_GetSlotPosition(&starts[i], selected);
    }

    for (i = 0; i < 2; i++)
        targets[i] = starts[1 - i];

    SceneInput_BeginPairTransition((void *)(p + 0x1558), enabled, starts, targets, frames,
                                   attributes, 12);
    Sound_PlaySong(0x38A);
}

#else
INCLUDE_ASM("asm/nonmatching", Storage_BeginSwapTransition);
#endif
