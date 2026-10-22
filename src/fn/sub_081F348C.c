#include "global.h"
struct Pos {
    s32 x, z;
};
struct Entry {
    struct Pos pos;
    u32 a;
    u32 b;
    u8 pad[8];
};
struct Set {
    u8 pad[0xd1c];
    struct Entry entries[9];
};
void *SceneActors_GetBySlot(u32);
s32 sub_081F348C(s32 index, struct Pos *out)
{
    struct Set *set = SceneActors_GetBySlot(4);
    s32 r;
    if (set) {
        *out = ((struct Entry *)((u8 *)set + 0xd1c))[index].pos;
        r = 0;
    } else {
        r = -1;
    }
    return r;
}
