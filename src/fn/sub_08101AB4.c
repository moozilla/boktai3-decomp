#include "global.h"

struct FollowObject {
    u8 pad0[0xB8];
    u16 flags;
    u8 padBA[0x158 - 0xBA];
    u8 cells[4][0x10];
    u8 registered[4];
};
void TerrainModifiers_UnregisterCell(void *);
static inline u8 Test(u32 mask, u16 *flags)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline void And(u32 mask, u16 *p)
{
    *p = mask & *p;
}
void ActorGroup_ReleaseMember(struct FollowObject *o)
{
    u16 *flags;
    s32 i;
    if (Test(0x800, &o->flags)) {
        i = 0;
        flags = &o->flags;
        for (; i < 4; i++) {
            if (o->registered[i]) {
                TerrainModifiers_UnregisterCell(o->cells[i]);
                o->registered[i] = 0;
            }
        }
        And(~0x800, flags);
    }
}
