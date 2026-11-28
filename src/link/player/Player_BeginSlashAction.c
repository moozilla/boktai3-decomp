#include "global.h"
#include "player/player.h"

extern const s16 gUnk_086149C4[];
u32 Player_GetConfiguredActionPower(struct Player *);
u32 Player_CollectCallbackFlags(struct Player *);
void Player_ConsumeActionResource(struct Player *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
s32 Terrain_ClassifyOffsetHeight(void *, u32, u32);

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

void Player_BeginSlashAction(struct Player *p)
{
    struct PlayerStatModifierContext *q = &p->statModifierContext;
    u32 power, flags;
    s32 kind;
    u16 duration;
    q->contactOrigin = p->position;
    q->contactOrigin.y += 0x96;
    q->contactHeading = ((p->facing + 5) & 7) << 5;
    q->contactDestination.x =
        q->contactOrigin.x + scale(gUnk_086149C4[(q->contactHeading + 64) & 255] * 0xCC);
    q->contactDestination.y = q->contactOrigin.y;
    q->contactDestination.z = q->contactOrigin.z + scale(gUnk_086149C4[q->contactHeading] * 0xCC);
    power = Player_GetConfiguredActionPower(p);
    flags = Player_CollectCallbackFlags(p);
    if (p->actionMode == 1)
        flags |= 0x40000;
    duration = q->actionTimer;
    kind = Terrain_ClassifyOffsetHeight(&q->contactOrigin, q->contactHeading, 0xE0);
    if (kind != 0) {
        if ((q->savedActionIndex == 2 || q->savedActionIndex == 4) && kind == 2) {
            q->attackAnimation = 0x1B;
            q->motionSpeed = 0x38;
            q->swingFrames = 0;
            q->holdFrames = 3;
            q->windupFrames = 6;
        } else {
            q->attackAnimation = 0x1A;
            q->motionSpeed = 0x38;
            q->swingFrames = 0;
            q->holdFrames = 3;
            q->windupFrames = 0;
        }
    } else if (Terrain_ClassifyOffsetHeight(&q->contactOrigin, q->contactHeading, 0x150) != 0) {
        q->attackAnimation = 0x1B;
        q->motionSpeed = 0x38;
        q->swingFrames = 2;
        q->holdFrames = 7;
        q->windupFrames = kind;
    } else {
        q->attackAnimation = 0x1C;
        q->motionSpeed = 0x38;
        q->swingFrames = 4;
        q->holdFrames = 7;
        q->windupFrames = 5;
    }
    if (q->savedActionIndex >= 0) {
        flags |= 1 << q->savedActionIndex;
        Player_ConsumeActionResource(p);
    }
    Collider_SetAttackProperties(&q->attackCollider, power, duration, 2, flags,
                                 q->contactParameter);
    q->attackCollider.direction = q->contactHeading;
    q->contactTimer = 0;
}
