#include "global.h"
struct Nd {
    struct Nd *next;
    u16 pad;
    u16 f6;
};
struct M {
    u8 p[0x18];
    struct Nd *f18;
    struct Nd *f1c;
    struct Nd *f20;
    u8 q[0xac];
    u16 c0;
    u16 c1;
    u16 c2;
};
extern struct M *gColliderManager;
void Collider_Register(struct Nd *n)
{
    struct Nd *p;
    if (gColliderManager == 0)
        return;
    p = gColliderManager->f18;
    while ((p = p->next) != 0)
        if (p == n)
            return;
    gColliderManager->f20->next = n;
    n->next = 0;
    gColliderManager->f20 = n;
    gColliderManager->c0++;
}
