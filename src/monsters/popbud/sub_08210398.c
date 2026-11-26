#include "engine/word_flags.h"
#include "global.h"
#include "monsters/popbud/pink_spine_playback.h"
#include "engine/ram.h"
#include "scene/actor/contact_collider.h"
#include "player/cues/player_cue_sequence.h"
#include "engine/engine_pause.h"
/* Species identity is tentative. The update callback increments age after animation. */
extern u32 gUnk_02000560;
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void ProjectileEmitter_UpdateAttack(void);
s32 PinkSpine_UpdateDamageAnimation(struct PinkSpineState *p)
{
    struct ContactCollider *collider;

    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gUnk_02000560))
        return 0;
    Callback_InvokeActor(p, p->update);
    p->sprite.flags &= ~1;
    if (!p->f134) {
        collider = (struct ContactCollider *)&p->collider;
        collider->flags &= ~4;
        Collider_SetPosition(collider, &p->sprite.position, 0);
    } else {
        collider = (struct ContactCollider *)&p->collider;
        collider->flags |= 4;
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
        ended = PinkSpine_StepAnimation(p);
        if (ended && (p->update == ProjectileEmitter_UpdateAttack))
            Sound_PlaySong(0x521);
    }
    p->age++;
}
