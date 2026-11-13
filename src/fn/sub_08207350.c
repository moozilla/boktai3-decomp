// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "vulcan_cannon_worker.h"
#include "display_sequence_sprite.h"
void VulcanCannon_UpdateTargeting(void);
s32 Anim_RestartSequence(void *, void *, u32, u32, u32);

s32 VulcanCannon_UpdateFacingTurn(struct VulcanCannon *p)
{
    register u32 allowed asm("r1") = p->allowTurn;
    if (!allowed) {
        p->state = VulcanCannon_UpdateTargeting;
        Anim_RestartSequence(&p->sequence, p->resource, 0x11, (u8)p->facing, 0);
        return 0;
    }
    if (DisplaySequence_StepSprite(&p->display.sprite, &p->sequence)) {
        if (!p->facing)
            p->facing = 1;
        else
            p->facing = 0;
        p->state = VulcanCannon_UpdateTargeting;
        return Anim_RestartSequence(&p->sequence, p->resource, 0x11, (u8)p->facing, 0);
    }
}
