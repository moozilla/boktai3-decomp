#include "global.h"
#include "ram.h"
struct E {
    u8 f[4];
    u8 a;
    u8 g[0x178 - 5];
};
void Sprite_Remove(void *);
void ScaledShadow_Destroy(void *);
void Collider_Remove(void *);
void Hresvelgr_DestroyHomingShotPool(struct E *e)
{
    s32 i;
    e = (struct E *)((u8 *)e + 0x48);
    i = 2;
    do {
        if (e->a != 0) {
            Sprite_Remove(e);
            ScaledShadow_Destroy((u8 *)e + 0xec);
            Collider_Remove((u8 *)e + 0x3c);
        }
        e++;
        i--;
    } while (i >= 0);
    gEffectPoolManager = 0;
}
