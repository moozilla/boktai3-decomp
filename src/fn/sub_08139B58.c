#include "global.h"
struct CallbackPoolEntry {
    void *callback;
    u8 unknown04[0xc6];
    u8 active;
    u8 unknownCB[0xd];
};
struct CallbackPool {
    u8 unknown00[0x18];
    struct CallbackPoolEntry entries[6];
    u32 frame;
};
s32 sub_08249240(void *, u32, void *);
s32 CallbackPool_Update(struct CallbackPool *p)
{
    s32 i;
    for (i = 0; i < 6; i++) {
        if (p->entries[i].active) {
            void *callback = p->entries[i].callback;
            if (callback)
                sub_08249240(&p->entries[i], i * 0xd8, callback);
        }
    }
    p->frame++;
    return 0;
}
