#include "global.h"
typedef struct { void *ptr; u8 pad[0xd1]; u8 active; u8 tail[0xa]; } Entry;
typedef struct { u8 pad[0x18]; Entry entries[4]; } Obj;
void sub_0824923C(void *, void *);
s32 sub_08139298(Obj *p) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (p->entries[i].active && p->entries[i].ptr)
            sub_0824923C(&p->entries[i], p->entries[i].ptr);
    }
    return 0;
}
