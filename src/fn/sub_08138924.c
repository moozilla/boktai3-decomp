#include "global.h"
#include "display_animation.h"

#ifdef NONMATCHING
// fuzzy: 86.5%
// foundry draft (worker best 86.5%); what differs is mostly register allocation and instruction order
#define PlayerSlotDisplay_Initialize sub_08138924
#define gPlayerSlotDisplay           gUnk_020004C0
struct PlayerSlotDisplaySprite {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[0x14];
};
struct PlayerSlotDisplaySlot {
    u8 resource[0x1C];
    struct PlayerSlotDisplaySprite sprite;
    u8 buffer[8], unknown40[8];
    struct DisplaySequenceState animation;
    u8 unknown58[6];
    u16 active;
};
struct PlayerSlotDisplay {
    u8 task[0x18];
    struct PlayerSlotDisplaySlot slots[4];
    void *sequenceResource;
};
extern struct PlayerSlotDisplay *gPlayerSlotDisplay;
s32 sub_08138924(register struct PlayerSlotDisplay *manager)
{
    s32 i = 0;
    u8 *buffer = manager->slots[0].buffer;
    struct DisplaySequenceState *animation = &manager->slots[0].animation;
    struct PlayerSlotDisplaySprite *sprite = &manager->slots[0].sprite;
    u8 *resource = manager->slots[0].resource;
    u32 offset = 0;
    do {
        void *sequenceResource;
        if (AnimData_Load(resource, 0x409C)) {
            Sprite_Init(sprite, resource, 0);
            sequenceResource = ScriptProc_Start(0x922E, 0x1C22);
            manager->sequenceResource = sequenceResource;
            Anim_SetSequence(animation, sequenceResource, 1, 0, 0);
            Memory_Clear(buffer, 8);
            *(u16 *)(buffer + 0x26) = 0;
            {
                u8 *base = (u8 *)manager + 0x34;
                u32 *flags = (void *)(base + offset);
                *flags |= 0x11;
            }
            sprite->priority = 1;
            buffer += 0x60;
            animation = (void *)((u8 *)animation + 0x60);
            sprite = (void *)((u8 *)sprite + 0x60);
            resource += 0x60;
            offset += 0x60;
            i++;
        } else
            return -1;
    } while (i <= 3);
    gPlayerSlotDisplay = manager;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08138924);
#endif
