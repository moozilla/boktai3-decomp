#include "global.h"
#include "player/player_status_flags.h"

extern const s16 gUnk_086149C4[];
void Player_PlayLocalSong(s32, u32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void Player_EmitSweepSpark(struct Player *, struct SceneVec *, s32, u32, s32);

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

s32 Player_UpdateSweepAction(struct Player *p)
{
    s32 angle;
    struct PlayerStatModifierContext *q = &p->statModifierContext;
    if (q->contactTimer == 0) {
        if (p->actionMode == 0)
            Player_PlayLocalSong(p->slot, 0x38C);
        else
            Player_PlayLocalSong(p->slot, 0x38B);
    }
    if (++q->contactTimer <= q->swingFrames) {
        if (p->side) {
            angle = q->contactHeading - q->contactTimer * q->motionSpeed + 0x120;
            angle &= 255;
        } else {
            u8 *heading = &q->contactHeading;
            s32 sum = (q->contactTimer * q->motionSpeed) + 0x100;
            sum += *heading;
            angle = sum - 0x20;
            angle &= 255;
        }
        q->contactDestination.x =
            q->contactOrigin.x + scale(gUnk_086149C4[(angle + 64) & 255] * 160);
        q->contactDestination.y = q->contactOrigin.y;
        q->contactDestination.z = q->contactOrigin.z + scale(gUnk_086149C4[angle] * 160);
        Collider_SetPosition(&q->attackCollider, &q->contactDestination, 0);
        Collider_Refresh(&q->attackCollider);
        if (q->savedActionIndex >= 0) {
            Player_AddStatusFlags(p, 0x80000);
            if (q->contactTimer & 1)
                Player_EmitSweepSpark(p, &q->contactOrigin, angle, p->side, q->savedActionIndex);
        }
        return 0;
    }
    return 1;
}
