#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySong(u32);
u32 Callback_InvokeActor(void *, void *);
int Math_Atan2(int, int);
extern s16 gUnk_086149C4[];
static inline int trunc12(int x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    s32 x = trunc12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->x = x;
    v->y = 0;
    v->z = trunc12(gUnk_086149C4[angle & 255] * radius);
}

void SceneActors_UpdateSpawnShake(struct Ratatosk *p, int tick)
{
    struct SceneActorBody *body = &p->body;
    if (p->actionChanged) {
        p->actionChanged = 0;
        p->targetPosition = body->position;
        Sound_PlaySong(0x567);
    }
    Ratatosk_SetFlags(p, 0x0200000C);
    if (!(u8)Callback_InvokeActor(p, p->actionReset)) {
        body->position = p->targetPosition;
        {
            int dir = tick & 3;
            polar(&body->motion, dir << 6, tick << 3);
        }
        if (tick > 31)
            p->actionFinished = 1;
    }
}
