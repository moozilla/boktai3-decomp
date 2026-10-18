#include "global.h"

struct S080FC844 {
    u8 filler[0x3d0];
    u8 *q;
};
void Collider_Remove(u8 *);
void Shadow_Destroy(u8 *);
void Sprite_Remove(u8 *);

void Axe_ReleaseAuxiliarySprite(struct S080FC844 *s)
{
    u8 *q = s->q;
    if (q[0xa3c]) {
        u8 *p = q + 0xb8c;
        s32 i = 2;
        do {
            Collider_Remove(p);
            p += 0x54;
        } while (--i >= 0);
        Shadow_Destroy(q + 0xcac);
        Sprite_Remove(q + 0xa38);
    }
}
