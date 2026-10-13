#include "global.h"
struct Object {
    u8 pad0[0x50];
    u32 flags;
    u8 pad54[0xc0];
    void (*update)(void *);
    void (*destroy)(void *);
    u8 pad11c[0x2b];
    u8 slot;
    u8 pad148[0xd0];
    struct Object *next, *prev;
};
struct Manager {
    u8 pad0[0x18];
    void *targets[4];
    struct Object *head;
    u32 ticks, slot, serial;
};
extern struct Manager *gUnk_020000D8;
extern u8 *gUnk_02000580[4];
int sub_0802C6C8(struct Manager *, struct Object *);
void Heap_Free(void *);
void sub_0802C710(struct Manager *);
int sub_0802C7E8(struct Manager *m)
{
    int i;
    gUnk_020000D8 = m;
    {
        int zero = 0;
        for (i = 3; i >= 0; i--)
            m->targets[i] = (void *)zero;
    }
    m->head = 0;
    sub_0802C710(m);
    return 0;
}
