#include "global.h"
#include "engine.h"
/* Tentative: per-frame update of a collider-carrying object (restart animation once, move colliders, advance timer). */
struct ColliderObjectCtx {
    u8 pad[0x18];
    void *animation;
};
struct ColliderObject {
    u8 pad0, restart, pad2, pad3, hold, done;
    u16 timer;
    u8 pad8[0xE];
    s16 health;
    void *collider, *attack;
    u8 pad20[0x1C];
    u32 flags;
    u8 pad40[0x28];
    u8 anim[1];
};
extern struct ColliderObjectCtx *gColliderSpritePool;

void ColliderObject_Update(struct ColliderObject *p)
{
    if (p->restart) {
        p->restart = 0;
        p->flags &= ~1;
        Anim_SetSequence(p->anim, gColliderSpritePool->animation, 1, 0, 0);
    }
    if (p->health <= 0 || p->hold) {
        p->done = 1;
    } else {
        if (p->collider)
            Collider_SetPosition(p->collider, (u8 *)p + 0xC, 0);
        if (p->attack) {
            Collider_SetPosition(p->attack, (u8 *)p + 0xC, 0);
            Collider_Refresh(p->attack);
        }
        p->timer++;
    }
}
