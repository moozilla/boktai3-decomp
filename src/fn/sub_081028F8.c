#include "global.h"
#include "ram.h"
struct NamedObject {
    u8 pad0[0xB8];
    s16 flags;
    u8 padBA[0x22];
    u32 name;
    u8 padE0[0xBC];
};
struct NamedObjectManager {
    u8 pad0[0x1C];
    struct NamedObject objects[12];
    u32 activeMask;
};
static inline u8 isActive(u32 *mask, s32 index)
{
    u32 bit = 1 << index;
    if (*mask & bit)
        return 1;
    return 0;
}
void NamedObject_ClearAttachmentFlag(u32 name, s32 tether)
{
    struct NamedObjectManager *manager = gNamedActorManager;
    s32 i;
    if (manager->activeMask) {
        for (i = 0; i < 12; i++) {
            if (isActive(&manager->activeMask, i)) {
                if (manager->objects[i].name == name) {
                    if (!tether)
                        ((struct NamedObject *)((u8 *)manager + i * 0x19C + 0x1C))->flags &= ~8;
                    else
                        ((struct NamedObject *)((u8 *)manager + i * 0x19C + 0x1C))->flags &= ~16;
                    break;
                }
            }
        }
    }
}
