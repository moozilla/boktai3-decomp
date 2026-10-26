#include "global.h"
#include "ram.h"
struct E {
    u8 f[4];
    u8 a;
    u8 g[0x90 - 5];
};
void Sprite_Remove(void *);
void Collider_Remove(void *);
s32 Hresvelgr_DestroyShotPool(struct E *e)
{
    s32 i;
    e = (struct E *)((u8 *)e + 0x48);
    i = 0x1f;
    do {
        if (e->a != 0) {
            Sprite_Remove(e);
            Collider_Remove((u8 *)e + 0x2c);
        }
        e++;
        i--;
    } while (i >= 0);
    return gDirectionBurstEmitter = 0;
}
