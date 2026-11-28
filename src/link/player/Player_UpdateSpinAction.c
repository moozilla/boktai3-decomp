#include "global.h"
#include "player/player_status_flags.h"

extern const s16 gUnk_086149C4[];
void Player_PlayLocalSong(s32, u32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void Player_EmitRandomSpark(struct Player *, struct SceneVec *, u32, s32);

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

static inline void PlaceAttack(struct PlayerStatModifierContext *q, s32 radius)
{
    q->contactDestination.x =
        q->contactOrigin.x + scale(gUnk_086149C4[(q->contactHeading + 64) & 255] * radius);
    q->contactDestination.y = q->contactOrigin.y;
    q->contactDestination.z = q->contactOrigin.z + scale(gUnk_086149C4[q->contactHeading] * radius);
    Collider_SetPosition(&q->attackCollider, &q->contactDestination, 0);
    Collider_Refresh(&q->attackCollider);
}

s32 Player_UpdateSpinAction(struct Player *p)
{
    struct PlayerStatModifierContext *q = &p->statModifierContext;
    if (q->contactTimer == 0) {
        if (p->actionMode == 0)
            Player_PlayLocalSong(p->slot, 0x38F);
        else
            Player_PlayLocalSong(p->slot, 0x38D);
    }
    if (q->contactTimer < q->swingFrames) {
        s32 radius = q->motionSpeed * q->contactTimer + 0xCC;
        PlaceAttack(q, radius);
        if (q->savedActionIndex >= 0) {
            Player_AddStatusFlags(p, 0x80000);
            Player_EmitRandomSpark(p, &q->contactDestination, q->contactHeading,
                                   q->savedActionIndex);
        }
    } else if (q->contactTimer < q->swingFrames + q->holdFrames) {
        s32 radius = q->motionSpeed * q->swingFrames + 0xCC;
        PlaceAttack(q, radius);
        if (q->savedActionIndex >= 0) {
            Player_AddStatusFlags(p, 0x80000);
            if (!(q->contactTimer & 1))
                Player_EmitRandomSpark(p, &q->contactDestination, q->contactHeading,
                                       q->savedActionIndex);
        }
    }
    if (++q->contactTimer >= q->swingFrames + q->holdFrames + q->windupFrames) {
        q->contactTimer = 0;
        return 1;
    }
    return 0;
}
