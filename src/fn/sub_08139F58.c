#define gCallbackPool gSceneProcParticles
#include "global.h"
#include "ram.h"
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
void Sprite_Remove(void *);
void CallbackPool_RemoveEntry(u32 i)
{
    struct CallbackPool *p = gSceneProcParticles;
    if (p && i < 6 && p->entries[i].spriteActive) {
        p->entries[i].active = 0;
        p->entries[i].kind = 0;
        p->entries[i].callback = 0;
        Sprite_Remove(p->entries[i].sprite);
    }
}
