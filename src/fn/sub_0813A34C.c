#define PairCallbackPool_MarkNextOfKind sub_0813A34C
#include "global.h"
struct PairCallbackPoolEntry {
    void *callback;
    u8 unknown04[0xca];
    u8 active;
    u8 unknownCF;
    u16 kind;
    u8 unknownD2[0xa];
};
struct PairCallbackPool {
    u8 unknown00[0x18];
    struct PairCallbackPoolEntry entries[2];
    u32 frame;
};
s32 sub_0813A34C(struct PairCallbackPool *p, s32 first, s32 kind, u8 *visited)
{
    s32 i;
    for (i = first; i < 2; i++) {
        if (p->entries[i].active && !visited[i] && p->entries[i].kind == kind) {
            visited[i] = 1;
            return 1;
        }
    }
    return 0;
}
