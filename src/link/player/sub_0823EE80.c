#include "global.h"
#include "player/player.h"

extern const s16 gUnk_086149C4[];
void Player_PlayLocalSong(u32, u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

u32 sub_0823EE80(struct Player *p)
{
    struct PlayerStatModifierContext *q = &p->statModifierContext;

    if (q->contactTimer == 0)
        Player_PlayLocalSong(p->slot, 0x391);
    if (++q->contactTimer <= 5) {
        q->contactDestination.x =
            q->contactOrigin.x + scale(gUnk_086149C4[(q->contactHeading + 0x40) & 0xFF] * 100);
        q->contactDestination.y = q->contactOrigin.y;
        q->contactDestination.z =
            q->contactOrigin.z + scale(gUnk_086149C4[q->contactHeading] * 100);
        Collider_SetPosition(&p->statModifierContext.attackCollider,
                             &p->statModifierContext.contactDestination, 0);
        Collider_Refresh(&p->statModifierContext.attackCollider);
        return 0;
    }
    q->contactTimer = 0;
    return 1;
}
