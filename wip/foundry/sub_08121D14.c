#define SpriteGroup_AllocateEntry sub_08121D14
#include "global.h"
struct SpriteGroupEntry { u8 pad[10]; u8 active; s8 slot; };
struct SpriteGroup { u8 pad[0x218]; u32 used; struct SpriteGroupEntry entries[8]; };
static inline u8 IsUsed(u32 *used, s32 i, u32 one) { if (*used & (one << i)) return 1; return 0; }
struct SpriteGroupEntry *sub_08121D14(struct SpriteGroup *input)
{
    struct SpriteGroup *p = input;
    s32 i = 0;
    u32 *used = &p->used;
    u16 one = 1;
    u32 offset = i;
    for (; i < 8; offset += sizeof(struct SpriteGroupEntry), i++) {
        if (!IsUsed(used, i, one)) {
            struct SpriteGroupEntry *slot = (void *)((u8 *)p->entries + offset);
            slot->slot = i;
            { s32 mask = one; *used |= mask << i; }
            return slot;
        }
    }
    return 0;
}
