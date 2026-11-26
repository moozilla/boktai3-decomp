#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"

u32 Callback_InvokeActor(void *, void *);
extern s16 gUnk_086149C4[];
static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
// confidence=low: restores the saved position while applying a decreasing shake.
#define Ratatosk_UpdateDecayingShake sub_081F5E60
void Ratatosk_UpdateDecayingShake(struct Ratatosk *p, int tick)
{
    struct Ratatosk *p2 = p;
    struct SceneActorBody *body = &p->body;
    if (p->actionChanged) {
        p->actionChanged = 0;
        {
            Ratatosk_SetFlags(p, 16);
        }
        p->targetPosition = body->position;
    }
    {
        Ratatosk_SetFlags(p, 0x0200000C);
    }
    if (!(u8)Callback_InvokeActor(p2, p->actionReset)) {
        body->position = p->targetPosition;
        if (tick > 31)
            p->actionFinished = 1;
        else {
            int dir = 3 & ~tick;
            int remaining = 32 - tick;
            s16 *velocity = &p->body.motion.x;
            u32 angle;
            int speed;
            angle = dir << 6;
            speed = remaining << 3;
            {
                int x = trunc12(gUnk_086149C4[(angle + 64) & 255] * speed);
                int zero = 0;
                velocity[0] = x;
                velocity[1] = zero;
            }
            {
                s16 *entry = &gUnk_086149C4[angle];
                velocity[2] = trunc12(*entry * speed);
            }
        }
    }
}
