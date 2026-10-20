// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 94.4%
// Remaining differences: animation constant copies, switch-case liveness and field-address registers.
#include "global.h"
#include "screamer_playback.h"
// confidence=low: animation phases and repeated collider activation imply a scream attack.
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
s32 Screamer_WaitForPlayerApproach(struct ScreamerState *);
s32 Screamer_UpdateScream(struct ScreamerState *p)
{
    register s32 phase asm("r3");
    p->sprite.flags &= ~1;
    phase = p->f1D6;
    asm("" : "+r"(phase));
    switch (phase) {
        case 0:
            Screamer_StepAnimation(p);
            if (p->f1CA > p->f1D4) {
                Anim_RestartSequence(&p->sequence, p->f218, 0x1B, (u8)p->f1D8, 0);
                Anim_SetDurationScale(&p->sequence, 0x20);
                p->f1D6 = 1;
                Sound_PlaySong(0x51A);
            }
            p->colliderFlags &= ~4;
            Collider_SetPosition(p->bodyCollider, &p->sprite.position, 0);
            p->contactFlags |= 4;
            break;
        case 1:
            if (Screamer_StepAnimation(p)) {
                p->f1D2++;
                if (p->f1D2 >= p->f1D0) {
                    Anim_RestartSequence(&p->sequence, p->f218, 0x1D, (u8)p->f1D8, 0);
                    p->f1D6 = 2;
                }
            }
            Collider_SetPosition(p->attackCollider, &p->sprite.position, 0);
            Collider_Refresh(p->attackCollider);
            p->colliderFlags &= ~4;
            p->contactFlags |= 4;
            break;
        case 2:
            Screamer_StepAnimation(p);
            if (p->f1CA > p->f1CC) {
                Anim_RestartSequence(&p->sequence, p->f218, 0x1A, (u8)p->f1D8, 0);
                p->update = Screamer_WaitForPlayerApproach;
                p->f1D6 = 0;
                p->f1CA = 0;
                p->f1D2 = 0;
            }
            p->colliderFlags &= ~4;
            Collider_SetPosition(p->bodyCollider, &p->sprite.position, 0);
            p->contactFlags |= 4;
            break;
    }
    p->sprite.flags &= ~1;
    p->f1CA++;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", Screamer_UpdateScream);
#endif
