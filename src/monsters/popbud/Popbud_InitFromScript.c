#include "global.h"
#include "monsters/popbud/popbud_playback.h"
// confidence=low: prefix follows the neighboring creator; species identity remains tentative.
void ProjectileEmitter_WaitForPlayerApproach(void);
s32 Popbud_InitCollider(struct PinkSpineState *);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
s32 Popbud_InitFromScript(struct PinkSpineState *popbud, void *resource)
{
    void *sequences;
    if (Script_SeekToKeyword('p')) {
        popbud->initialPosition.x = Script_GetValue();
        popbud->initialPosition.y = Script_GetValue();
        popbud->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        popbud->nearRadiusSquared = Script_GetValue();
        popbud->farRadiusSquared = Script_GetValue();
        popbud->nearRadiusSquared *= popbud->nearRadiusSquared;
        popbud->farRadiusSquared *= popbud->farRadiusSquared;
    } else {
        popbud->nearRadiusSquared = 0x90000;
        popbud->farRadiusSquared = 0x10000;
    }
    if (Script_SeekToKeyword('l')) {
        popbud->f136 = Script_GetValue();
        if (popbud->f136 < 0)
            popbud->f13A = 1;
    }
    if (Script_SeekToKeyword('a')) {
        popbud->projectilePower = Script_GetValue();
        popbud->projectileKnockback = Script_GetValue();
        popbud->projectileImmunity = Script_GetValue();
        popbud->projectileSpeed = Script_GetValue();
    } else {
        popbud->projectilePower = 30;
        popbud->projectileKnockback = 0;
        popbud->projectileImmunity = 30;
        popbud->projectileSpeed = 24;
    }
    if (Script_SeekToKeyword('S'))
        popbud->f138 = Script_GetValue();
    if (Script_SeekToKeyword('D'))
        popbud->f158 = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        popbud->f14C = Script_GetValue();
        popbud->f150 = Script_GetValue();
        popbud->f154 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        popbud->f13C = Script_GetValue();
        popbud->f13E = Script_GetValue();
    }
    popbud->f140 = resource;
    AnimData_Load(&popbud->graphics, 0x7E7D);
    Sprite_Init(&popbud->sprite, &popbud->graphics, 1);
    popbud->sprite.position = popbud->initialPosition;
    AnimData_SelectPaletteBank(&popbud->graphics, 0x1BD);
    sequences = ScriptProc_Start(0x922E, 0x5184);
    popbud->f15C = sequences;
    if (!popbud->f13A) {
        popbud->update = ProjectileEmitter_WaitForPlayerApproach;
        Anim_SetSequence(&popbud->sequence, sequences, 0xB, 0, 0);
    } else {
        popbud->update = ProjectileEmitter_EmitAttackFrame;
        popbud->f134 = 1;
        Anim_SetSequence(&popbud->sequence, popbud->f15C, 0xD, 0, 0);
    }
    PlayerCue_StepSequence(&popbud->sprite, &popbud->sequence);
    if (!popbud->f13A)
        Popbud_InitCollider(popbud);
    if (popbud->f138) {
        SceneBody_Init(popbud->body, popbud->f140, &popbud->sprite.position, 0, 4, popbud);
        SceneBody_SetSprite(popbud->body, &popbud->sprite);
        TerrainOverlay_QueueAddActor(popbud->body);
    }
    popbud->f136 = 1;
    /* This initializer has no explicit return-value assignment in the ROM. */
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
