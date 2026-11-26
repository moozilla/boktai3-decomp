#include "engine/shared/global.h"
#include "player/player.h"
#include "player/player_status_flags.h"

extern const s16 gUnk_086149C4[];
void Player_PlayLocalSong(s32, u32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void Player_EmitScatterEffect(struct Player *, struct SceneVec *, u32, u32, s32);

struct Vec16 {
    u32 x : 16, y : 16, z : 16;
};

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

s32 Player_UpdateThrowArc(struct Player *p)
{
    struct PlayerStatModifierContext *q = &p->statModifierContext;
    if (++q->contactTimer <= q->windupFrames)
        return 0;
    if (q->contactTimer <= q->windupFrames + q->swingFrames) {
        u32 angle;
        s32 t;
        if (q->contactTimer == q->windupFrames + q->swingFrames) {
            if (p->actionMode == 0)
                Player_PlayLocalSong(p->slot, 0x390);
            else
                Player_PlayLocalSong(p->slot, 0x38E);
        }
        angle = (0x150 - (q->contactTimer - q->windupFrames) * q->motionSpeed) & 255;
        q->contactDestination.y = q->contactOrigin.y + scale(gUnk_086149C4[angle & 255] * 250);
        t = scale(gUnk_086149C4[(angle + 64) & 255] * 250);
        q->contactDestination.x =
            q->contactOrigin.x + scale(gUnk_086149C4[(q->contactHeading + 64) & 255] * t);
        q->contactDestination.z = q->contactOrigin.z + scale(gUnk_086149C4[q->contactHeading] * t);
        if (q->contactTimer > q->windupFrames + 2 && q->savedActionIndex >= 0)
            Player_EmitScatterEffect(p, &q->contactDestination, angle, q->contactHeading,
                                     q->savedActionIndex);
        if (q->contactTimer > q->windupFrames + q->swingFrames - 2) {
            struct Vec16 pos;
            s32 radius = 0xAA;
            pos.x =
                q->contactOrigin.x + scale(gUnk_086149C4[(q->contactHeading + 64) & 255] * radius);
            pos.y = (q->windupFrames + q->swingFrames - q->contactTimer + 1) * 90 +
                    q->contactOrigin.y - 0x104;
            pos.z = q->contactOrigin.z + scale(gUnk_086149C4[q->contactHeading] * radius);
            if (q->savedActionIndex >= 0) {
                Player_AddStatusFlags(p, 0x80000);
            }
            Collider_SetPosition(&p->statModifierContext.attackCollider, &pos, 0);
            Collider_Refresh(&q->attackCollider);
        }
        return 0;
    }
    if (q->contactTimer <= q->windupFrames + q->swingFrames + q->holdFrames) {
        if (q->savedActionIndex >= 0) {
            Player_AddStatusFlags(p, 0x80000);
        }
        Collider_Refresh(&p->statModifierContext.attackCollider);
        return 0;
    }
    q->contactTimer = 0;
    return 1;
}
