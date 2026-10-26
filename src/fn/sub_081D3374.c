#include "global.h"
#include "ram.h"

struct E {
    u8 pad[0x48];
};
void MessageWindow_Clear(u8 *);
void Effect_Remove(u8 *);

u32 PlayerParticleCloud_Destroy(u8 *p)
{
    u8 *e;
    s32 i;
    MessageWindow_Clear(p);
    e = p + 0x30;
    for (i = 39; i >= 0; i--, e += 0x48) {
        Effect_Remove(e + 0x20);
    }
    return gCloudTask = 0;
}
