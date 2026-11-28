#include "global.h"
#include "monsters/screamer/screamer_playback.h"
#include "engine/video/display_sequence_inline.h"
s32 Screamer_WaitForPlayerApproach(struct ScreamerState *);
void SceneDrawablePair_SetFlags(void);
s32 Screamer_InitColliders(struct ScreamerState *);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
s32 Screamer_InitFromScript(struct ScreamerState *screamer, void *resource)
{
    struct ContactPosition position;
    void *sequences;
    if (Script_SeekToKeyword('p')) {
        screamer->initialPosition.x = Script_GetValue();
        screamer->initialPosition.y = Script_GetValue();
        screamer->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        screamer->f1C4 = Script_GetValue();
        screamer->f1C6 = Script_GetValue();
        screamer->f1C8 = Script_GetValue();
    } else {
        screamer->f1C4 = 30;
        screamer->f1C6 = 10;
        screamer->f1C8 = 30;
    }
    if (Script_SeekToKeyword('l')) {
        screamer->f1E4 = screamer->f1E6 = Script_GetValue();
        if (screamer->f1E4 < 0)
            screamer->f1F6 = 1;
    } else {
        screamer->f1E6 = 120;
        screamer->f1E4 = 120;
    }
    if (Script_SeekToKeyword('i'))
        screamer->f1CC = Script_GetValue();
    else
        screamer->f1CC = 120;
    if (Script_SeekToKeyword('r'))
        screamer->f1DA = Script_GetValue();
    else
        screamer->f1DA = 256;
    if (Script_SeekToKeyword('w')) {
        screamer->f1D4 = Script_GetValue();
        screamer->f1D0 = Script_GetValue();
    } else {
        screamer->f1D4 = 30;
        screamer->f1D0 = 3;
    }
    if (Script_SeekToKeyword('h')) {
        screamer->f1D8 = Script_GetValue();
        screamer->f1D8 = (screamer->f1D8 - 1) / 2;
        if ((u16)screamer->f1D8 > 3)
            screamer->f1D8 = 1;
        else if ((u16)screamer->f1D8 <= 1)
            screamer->f1D8 = 0;
        else
            screamer->f1D8 = 1;
    } else
        screamer->f1D8 = 0;
    if (Script_SeekToKeyword('S'))
        screamer->f1EE = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        screamer->f1E8 = Script_GetValue();
        screamer->f1EA = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        screamer->f1EC = Script_GetValue();
    else
        screamer->f1EC = 2;
    if (Script_SeekToKeyword('D'))
        screamer->f214 = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        screamer->f208 = Script_GetValue();
        screamer->f20C = Script_GetValue();
        screamer->f210 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        screamer->f1F0 = Script_GetValue();
        screamer->f1F2 = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        screamer->f1F4 = Script_GetValue();
        if (screamer->f1F4 > 99)
            screamer->f1F4 = 99;
        if (screamer->f1F4 <= 0)
            screamer->f1F4 = 1;
    } else
        screamer->f1F4 = 0;
    screamer->f1F8 = resource;
    AnimData_Load(&screamer->graphics, 0x7E7D);
    Sprite_Init(&screamer->sprite, &screamer->graphics, 1);
    screamer->sprite.position = screamer->initialPosition;
    AnimData_SelectPaletteBank(&screamer->graphics, 0x1C4);
    screamer->sprite.priority = 3;
    sequences = ScriptProc_Start(0x922E, 0x5184);
    screamer->f218 = sequences;
    if (!screamer->f1F6) {
        screamer->update = Screamer_WaitForPlayerApproach;
        Anim_SetSequence(&screamer->sequence, sequences, 0x1A, (u8)screamer->f1D8, 8);
    } else {
        screamer->update = SceneDrawablePair_SetFlags;
        screamer->f1E0 = 1;
        Anim_SetSequence(&screamer->sequence, screamer->f218, 0x1A, (u8)screamer->f1D8, 8);
    }
    PlayerCue_StepSequenceEnded(&screamer->sprite, &screamer->sequence);
    if (!screamer->f1F6)
        Screamer_InitColliders(screamer);
    if (!screamer->f1D8)
        screamer->sprite.position.x -= 16;
    else
        screamer->sprite.position.z -= 16;
    if (screamer->f1EE) {
        struct ContactPosition *offsetPosition;
        offsetPosition = &position;
        *offsetPosition = screamer->sprite.position;
        if (!screamer->f1D8)
            offsetPosition->x += 128;
        else
            offsetPosition->z += 128;
        SceneBody_Init(screamer->body, screamer->f1F8, &position, 0, 4, screamer);
        SceneBody_SetSprite(screamer->body, &screamer->sprite);
        TerrainOverlay_QueueAddActor(screamer->body);
    }
    if (screamer->f1DA > 0)
        screamer->f1FC = screamer->f1DA * screamer->f1DA;
    else
        screamer->f1FC = screamer->f1DA;
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
