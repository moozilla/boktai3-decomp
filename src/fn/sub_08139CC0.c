#include "global.h"
struct CallbackPoolEntry {
    void *callback;
    u8 unknown04[0xc6];
    u8 active;
    u8 unknownCB;
    u16 kind;
    u8 unknownCE[0xa];
};
struct CallbackPool {
    u8 unknown00[0x18];
    struct CallbackPoolEntry entries[6];
    u32 frame;
};
s32 CallbackPool_MarkNextOfKind(struct CallbackPool *p, s32 first, s32 kind, u8 *visited)
{
    s32 i;
    for (i = first; i < 6; i++) {
        if (p->entries[i].active && !visited[i] && p->entries[i].kind == kind) {
            visited[i] = 1;
            return 1;
        }
    }
    return 0;
}
