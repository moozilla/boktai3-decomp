#include "global.h"

struct SpriteGroup {
    u8 pad[0x1c];
    u8 sprites[0x2c * 11];
    u8 unknown200[0x18];
    u32 activeSlots;
};
struct SpriteGroupEntry {
    u8 pad[10];
    u8 active;
    s8 slot;
};
void Sprite_Remove(void *);
void Memory_Clear(void *, u32);
void SpriteGroup_RemoveEntry(struct SpriteGroup *group, struct SpriteGroupEntry *entry)
{
    Sprite_Remove(group->sprites + entry->slot * 0x2c);
    entry->active = 0;
    {
        s32 slot = entry->slot;
        group->activeSlots &= ~(1 << slot);
    }
    Memory_Clear(entry, 12);
    entry->slot = -1;
}
