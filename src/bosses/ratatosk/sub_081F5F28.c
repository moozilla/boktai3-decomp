#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

static inline void setFacing(struct Ratatosk *p, int angle)
{
    u32 value;
    int shifted;
    p->body.direction = angle;
    shifted = p->body.direction + 0x60;
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->body.direction + 0x70;
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySong(u32);
u32 Callback_InvokeActor(void *, void *);
int Math_Atan2(int, int);
extern s16 gUnk_086149C4[];
static inline int choose(u32 mode, int normal, int alternate)
{
    int test = mode & 1;
    int result;
    result = normal;
    if (test)
        result = alternate;
    return result;
}
static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
void SceneActors_UpdateSpawnShake(struct Ratatosk *p, int time)
{
    int tick = time;
    struct Ratatosk *p2 = p;
    struct SceneActorBody *body = &p->body;
    if (p->actionChanged) {
        p->actionChanged = 0;
        p->targetPosition = body->position;
        Sound_PlaySong(0x567);
    }
    {
        Ratatosk_SetFlags(p, 0x0200000C);
    }
    if (!(u8)Callback_InvokeActor(p2, p->actionReset)) {
        body->position = p->targetPosition;
        {
            int dir = tick & 3;
            s16 *velocity = &body->motion.x;
            u32 angle;
            int speed;
            angle = dir << 6;
            speed = tick << 3;
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
        if (tick > 31)
            p->actionFinished = 1;
    }
}
