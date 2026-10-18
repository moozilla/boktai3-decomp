#include "global.h"
void Actor_CapturePendingHit(void *, void *, void *);
void Actor_OnHitboxContact(void *a, void *b, void *p)
{
    Actor_CapturePendingHit(p, a, b);
}
