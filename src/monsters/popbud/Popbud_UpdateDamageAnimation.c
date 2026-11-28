#include "engine/word_flags.h"
#include "global.h"
#include "monsters/popbud/popbud_playback.h"
#include "engine/ram.h"
#include "scene/collision/contact_collider.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/engine_pause.h"
/* Species identity is tentative. The update callback increments age after animation. */
#include "player/player.h"
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void ProjectileEmitter_UpdateAttack(void);
s32 Popbud_UpdateDamageAnimation(struct PinkSpineState *popbud)
{
    struct ContactCollider *collider;

    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gPlayerSlotStatus[0]))
        return 0;
    Callback_InvokeActor(popbud, popbud->update);
    popbud->sprite.flags &= ~1;
    if (!popbud->f134) {
        collider = &popbud->collider;
        collider->flags &= ~4;
        Collider_SetPosition(collider, &popbud->sprite.position, 0);
    } else {
        collider = &popbud->collider;
        collider->flags |= 4;
    }
    if ((popbud->f130 > 0) && (!popbud->f134)) {
        popbud->f136 -= popbud->f130;
        popbud->f144 = popbud->f148;
        Sound_PlaySong(0x13E);
        if (popbud->f136 <= 0) {
            popbud->f134 = 1;
            Anim_RestartSequence(&popbud->sequence, popbud->f15C, 0xD, 0, 0);
            AnimData_SelectPaletteBank(&popbud->graphics, 0x1BD);
            popbud->f12C = 0;
            popbud->update = ProjectileEmitter_EmitAttackFrame;
            Sound_PlaySong(0x522);
            popbud->age = 0;
        }
        popbud->f12C = 4;
        popbud->f132 = 1;
        popbud->f130 = 0;
    }
    if (popbud->f12C > 0) {
        AnimData_SelectPaletteBank(&popbud->graphics, 0x192);
        popbud->f12C--;
    } else
        AnimData_SelectPaletteBank(&popbud->graphics, 0x1BD);
    if (Popbud_StepAnimation(popbud) && popbud->update == ProjectileEmitter_UpdateAttack)
        Sound_PlaySong(0x521);
    popbud->age++;
}
