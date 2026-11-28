#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

u32 Callback_InvokeActor(void *, void *);
extern s16 gUnk_086149C4[];
static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
// confidence=low: restores the saved position while applying a decreasing shake.
static inline void polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}

void Ratatosk_UpdateDecayingShake(struct Ratatosk *p, int tick)
{
    struct SceneActorBody *body = &p->body;
    if (p->actionChanged) {
        p->actionChanged = 0;
        Ratatosk_SetFlags(p, 16);
        p->targetPosition = body->position;
    }
    Ratatosk_SetFlags(p, 0x0200000C);
    if (!(u8)Callback_InvokeActor(p, p->actionReset)) {
        body->position = p->targetPosition;
        if (tick > 31)
            p->actionFinished = 1;
        else {
            int dir = 3 & ~tick;
            int remaining = 32 - tick;
            polar(&body->motion, dir << 6, remaining << 3);
        }
    }
}
