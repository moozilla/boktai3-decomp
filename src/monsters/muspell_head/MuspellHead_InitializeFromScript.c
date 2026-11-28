#include "monsters/muspell_head.h"
#include "engine/math/scene_vec.h"
#include "engine/engine_globals.h"
#include "player/cutscene/player_cue_sequence.h"
#include "scene/terrain/terrain_cell_probe.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void MuspellHead_InitializeColliders(struct MuspellHead *);
u32 MuspellHead_TryProjectileAttack(u8 *);
void MuspellHead_UpdateDefeatEffect(void);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
s32 MuspellHead_InitializeFromScript(struct MuspellHead *head, void *parent)
{
    if (Script_SeekToKeyword('p')) {
        head->origin.x = Script_GetValue();
        head->origin.y = Script_GetValue();
        head->origin.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        s32 r = Script_GetValue();
        head->attackDistance = r * r;
    } else
        head->attackDistance = 0x64000;
    if (Script_SeekToKeyword('a')) {
        head->attackA = Script_GetValue();
        head->attackB = Script_GetValue();
        head->attackC = Script_GetValue();
    } else {
        head->attackA = 30;
        head->attackB = 10;
        head->attackC = 30;
    }
    if (Script_SeekToKeyword('l')) {
        s32 health = Script_GetValue();
        head->maximumHealth = health;
        head->health = health;
        if ((s16)health < 0)
            head->initialDead = 1;
    } else {
        head->maximumHealth = 120;
        head->health = 120;
    }
    if (Script_SeekToKeyword('d'))
        head->terrain = Script_GetValue();
    if (Script_SeekToKeyword('i'))
        head->duration = Script_GetValue();
    else
        head->duration = 120;
    if (Script_SeekToKeyword('w')) {
        head->delayA = Script_GetValue();
        head->delayB = Script_GetValue();
    } else {
        head->delayA = 180;
        head->delayB = 120;
    }
    if (Script_SeekToKeyword('S'))
        head->overlay = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        head->damageType = Script_GetValue();
        head->damageFlags = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        head->hudMode = Script_GetValue();
    else
        head->hudMode = 2;
    if (Script_SeekToKeyword('D'))
        head->optionD = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        head->optionG0 = Script_GetValue();
        head->optionG1 = Script_GetValue();
        head->optionG2 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        head->optionA = Script_GetValue();
        head->optionB = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        head->hudValue = Script_GetValue();
        if (head->hudValue > 99)
            head->hudValue = 99;
        if (head->hudValue <= 0)
            head->hudValue = 1;
    } else
        head->hudValue = 0;
    head->parent = parent;
    AnimData_Load(&head->graphics, 0x7e7d);
    Sprite_Init(&head->sprite, &head->graphics, 1);
    head->sprite.position = *(struct ContactPosition *)&head->origin;
    AnimData_SelectPaletteBank(&head->graphics, 0x1b7);
    head->sequence = ScriptProc_Start(0x922e, 0x5184);
    if (head->initialDead == 0) {
        head->action = (void (*)(void))MuspellHead_TryProjectileAttack;
        Anim_SetSequence(&head->playback, head->sequence, 0, 0, 0);
    } else {
        head->action = MuspellHead_UpdateDefeatEffect;
        head->dead = 1;
        Anim_SetSequence(&head->playback, head->sequence, 2, 0, 0);
    }
    PlayerCue_StepSequence(&head->sprite, &head->playback);
    if (head->initialDead == 0)
        MuspellHead_InitializeColliders(head);
    if (head->terrain && head->initialDead == 0) {
        TerrainModifiers_RegisterCell(head->cell,
                                      cellIndexAt((struct Pos16 *)&head->sprite.position), 0,
                                      head->sprite.position.y >> 8, 0, 0x803);
    }
    if (head->overlay) {
        SceneBody_Init(head->body, head->parent, &head->sprite.position, 0, 4, head);
        SceneBody_SetSprite(head->body, &head->sprite);
        TerrainOverlay_QueueAddActor(head->body);
    }
    head->active = 1;
    head->enabled = 0;
}
