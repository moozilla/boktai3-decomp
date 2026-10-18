#define CallbackPool_RemoveEntry sub_08139F58
#define gCallbackPool gUnk_020004D0
#include "global.h"
struct CallbackPoolEntry {
    void *callback;
    u8 unknown04[0x5c];
    u8 sprite[4];
    u8 spriteActive;
    u8 unknown65[0x65];
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
extern struct CallbackPool *gUnk_020004D0;
void Sprite_Remove(void *);
void sub_08139F58(u32 i)
{
    struct CallbackPool *p = gUnk_020004D0;
    if (p && i < 6 && p->entries[i].spriteActive) {
        p->entries[i].active = 0;
        p->entries[i].kind = 0;
        p->entries[i].callback = 0;
        Sprite_Remove(p->entries[i].sprite);
    }
}
