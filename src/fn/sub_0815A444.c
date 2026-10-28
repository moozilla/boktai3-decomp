#include "global.h"
#include "engine.h"
#include "player.h"
s32 Script_GetActorIndex(void);
s32 Script_ReadPositionOption(s16 *);
void CellObject_Init(void *, void *);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_StopActionSounds(struct Player *);
void sub_0815A444(void)
{
    s32 i = Script_GetActorIndex();
    struct Player *p = gPlayers[i];
    if (p && Script_ReadPositionOption((s16 *)&p->position)) {
        if (Script_SeekToKeyword('d')) {
            p->facing = Script_GetValue();
            if (p->facing > 4) {
                p->heading = 8 - p->facing;
                p->side = 1;
            } else {
                p->heading = p->facing;
                p->side = 0;
            }
        }
        CellObject_Init((u8 *)p + 0x284, &p->position);
        ActorSequence_Begin(p, 0, 0);
        Player_StopActionSounds(p);
    }
}
