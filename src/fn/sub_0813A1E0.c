#include "global.h"
struct PairCallbackPoolEntry {
    void *callback;
    u8 unknown04[0xca];
    u8 active;
    u8 unknownCF[0xd];
};
struct PairCallbackPool {
    u8 unknown00[0x18];
    struct PairCallbackPoolEntry entries[2];
    u32 frame;
};
s32 sub_08249240(void *, u32, void *);
s32 PairCallbackPool_Update(struct PairCallbackPool *p)
{
    s32 i;
    for (i = 0; i < 2; i++) {
        if (p->entries[i].active) {
            void *callback = p->entries[i].callback;
            if (callback)
                sub_08249240(&p->entries[i], i * 0xdc, callback);
        }
    }
    p->frame++;
    return 0;
}
