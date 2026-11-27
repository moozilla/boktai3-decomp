#include "global.h"

struct A {
    u8 f[0xe0];
    u16 n;
};
void Camera_RequestShake(s32);
void Hresvelgr_ReleaseProjectile(void *);

void Hresvelgr_UpdateProjectileImpact(struct A *p)
{
    u16 *q = &p->n;
    s32 d = 8 - *q;
    Camera_RequestShake(d * d);
    if (*q > 7)
        Hresvelgr_ReleaseProjectile(p);
    *q += 1;
}
