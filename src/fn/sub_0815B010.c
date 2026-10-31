#include "player.h"

void Player_EnterKindTwo(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_UpdateTriggerAnimation(struct Player *);

static inline u8 inRange(u32 q)
{
    if ((u8)(q - 1) <= 1)
        return TRUE;
    return FALSE;
}

static inline u8 above(u32 q)
{
    if (q > 1)
        return TRUE;
    return FALSE;
}

void PlayerCue_StandVariant3(struct Player *p, s32 dir, u32 c)
{
    if (p->actionMode == 6) {
        if (p->heading > 1)
            p->heading--;
    } else {
        u8 a = (dir << 5) + 0xA0;
        u32 q = ((a + 0x60) >> 6) & 3;
        u8 *a2 = &p->heading;
        u8 *a3 = &p->side;
        *a2 = inRange(q);
        *a3 = above(q);
    }
    p->cueParameter = c;
    Player_EnterKindTwo(p);
    ActorSequence_Begin(p, 0, 3);
    p->triggerAnimationCallback = Player_UpdateTriggerAnimation;
}
