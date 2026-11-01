// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "pink_spine_playback.h"
#include "ram.h"
#include "player_cue_sequence.h"
/* Species identity is tentative. The update callback increments age after animation. */
extern u32 gUnk_02000560;
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void ProjectileEmitter_EmitAttackFrame(void);
void ProjectileEmitter_UpdateAttack(void);
static inline u32 PauseFlags(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline u32 TestFlags(u32 bits)
{
    return gUnk_02000560 & bits;
}
static inline u32 PinkSpine_AdvanceDamageSequence(struct PinkSpineState *owner)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence);
}

s32 PinkSpine_UpdateDamageAnimation(struct PinkSpineState *p)
{
    if (PauseFlags(0x400) || TestFlags(4))
        return 0;
    Callback_InvokeActor(p, p->update);
    p->sprite.flags &= ~1;
    if (!p->f134) {
        struct ColliderView {
            u8 pad[6];
            u16 flags;
        };
        register struct ColliderView *c asm("r2") = (void *)(&p->collider);
        {
            register u32 value = c->flags;
            u32 mask = 0xFFFB;
            mask &= value;
            c->flags = mask;
        }
        Collider_SetPosition(c, &p->sprite.position, 0);
    } else {
        struct ColliderView {
            u8 pad[6];
            u16 flags;
        } *c = (void *)(&p->collider);
        c->flags |= 4;
    }
    if ((p->f130 > 0) && (!p->f134)) {
        p->f136 -= p->f130;
        p->f144 = p->f148;
        Sound_PlaySong(0x13E);
        if (p->f136 <= 0) {
            p->f134 = 1;
            Anim_RestartSequence(&p->sequence, p->f15C, 0xD, 0, 0);
            AnimData_SelectPaletteBank(p->graphics, 0x1BD);
            p->f12C = 0;
            p->update = ProjectileEmitter_EmitAttackFrame;
            Sound_PlaySong(0x522);
            p->age = 0;
        }
        p->f12C = 4;
        p->f132 = 1;
        p->f130 = 0;
    }
    if (p->f12C > 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->f12C--;
    } else
        AnimData_SelectPaletteBank(p->graphics, 0x1BD);
    {
        u32 ended;
        ended = PinkSpine_AdvanceDamageSequence(p);
        if (ended && (p->update == ProjectileEmitter_UpdateAttack))
            Sound_PlaySong(0x521);
    }
    p->age++;
}
