#include "global.h"
#include "player/player.h"

// confidence=low: continuation of the neighboring object-push action.
extern const s16 gUnk_086149C4[];
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
s32 PuzzleBlock_TryPushNeighborByIndex(void *, u32);
static inline s32 Scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u8 IsTwo(struct Player *p)
{
    u16 active = p->active;
    u8 result = 0;
    if (active && p->sceneMode == 2)
        result = 1;
    return result;
}
void Player_UpdateObjectPushRecovery(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0: {
            s32 angle = ((p->coordinateAxis + 5) & 7) << 5;
            p->velocityX = Scale(gUnk_086149C4[(angle + 0x40) & 255] * 4);
            p->velocityZ = Scale(gUnk_086149C4[angle] * 4);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 6), 0x40, 1);
            if (++p->tetherFrame > 63)
                ActorSequence_Begin(p, 5, 1);
            break;
        }
        case 1:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 6), 0x40, 1);
            if ((s16)p->directionHistory[0] == p->coordinateAxis &&
                PuzzleBlock_TryPushNeighborByIndex(&p->position, p->facing) > 0) {
                if (p && IsTwo(p))
                    ActorSequence_Begin(p, 0, 0);
                else
                    ActorSequence_Begin(p, 5, 0);
            } else
                ActorSequence_Begin(p, 5, 2);
            break;
        case 2:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 7), 0x40, 1)) {
                ActorSequence_Begin(p, 4, 1);
                p->actionFourEnabled = 0;
            }
            break;
    }
}
