#include "engine/engine_pause.h"
#include "monsters/muspell_head.h"
#include "engine/video/render.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "engine/video/display_sprite_playback.h"
#include "player/player.h"
#include "player/player_slot_status.h"
void Collider_SetPosition(void *collider, void *position, u32 value);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void Sound_PlaySong(u32 song);
void TerrainModifiers_UnregisterCell(void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
s32 Callback_InvokeActor(void *arg, void *function);
void SceneHud_SetTimedState(void *, int, int, int, int, int);
u32 MuspellHead_UpdateDefeatEffect(struct MuspellHead *);

// confidence=low: runs the actor action, damage response, palette timers and sprite playback.
u32 MuspellHead_UpdateState(struct MuspellHead *head)
{
    if (Engine_PauseBits(0x400))
        return 0;
    if (PlayerSlot_ReadStatus(0, 4))
        return 0;
    Callback_InvokeActor(head, head->action);
    head->sprite.flags &= ~1;
    if ((s16)head->dead == 0) {
        struct ContactCollider *collider = &head->bodyCollider;

        collider->flags &= ~4;
        Collider_SetPosition(collider, &head->sprite.position, 0);
    } else {
        struct ContactCollider *collider = &head->bodyCollider;

        collider->flags |= 4;
        head->graphics.paletteData = gPaletteBankData + 0x36e0;
    }
    if (head->damage > 0 && (s16)head->dead == 0) {
        head->health -= head->damage;
        head->previousHit = head->currentHit;
        Sound_PlaySong(0x13e);
        if (head->damage > 0) {
            DamageNumberDisplay_Show(&head->bodyCollider, head->damage, 0,
                                     &head->bodyCollider.position, &head->bodyCollider.offset, 1);
            SceneHud_SetTimedState(head->parent, head->health, head->maximumHealth, head->hudMode,
                                   head->bodyCollider.costFlags == 4, head->hudValue);
        }
        if (head->health <= 0) {
            head->dead = 1;
            if (head->terrain)
                TerrainModifiers_UnregisterCell(head->cell);
            Anim_RestartSequence(&head->playback, head->sequence, 2, 0, 0);
            AnimData_SelectPaletteBank(&head->graphics, 0x1b7);
            head->graphics.paletteData = gPaletteBankData + 0x36e0;
            head->flash = 0;
            head->age = 0;
            head->action = (void (*)(void))MuspellHead_UpdateDefeatEffect;
            Sound_PlaySong(0x529);
        }
        head->flash = 8;
        head->unknown182 = 1;
        head->damage = 0;
    }
    if ((s16)head->flash > 0 && (s16)head->dead == 0) {
        AnimData_SelectPaletteBank(&head->graphics, 0x192);
        head->flash--;
    } else {
        AnimData_SelectPaletteBank(&head->graphics, 0x1b7);
        if ((s16)head->dead == 0) {
            if (head->enabled == 0) {
                AnimData_SelectPaletteBank(&head->graphics, 0x1b8);
                if (head->phase > head->delayB) {
                    head->enabled = 1;
                    head->phase = 0;
                }
            } else {
                AnimData_SelectPaletteBank(&head->graphics, 0x1b7);
                if (head->phase > head->delayA) {
                    head->enabled = 0;
                    head->phase = 0;
                }
            }
        }
        head->phase++;
        if (head->bodyCollider.responseValue) {
            head->bodyCollider.responseValue--;
            if ((head->bodyCollider.responseValue >> 2) & 1)
                head->sprite.flags |= 1;
            else
                head->sprite.flags &= ~1;
        }
    }
    PlayerCue_StepSequenceEnded(&head->sprite, &head->playback);
    head->age++;
}
