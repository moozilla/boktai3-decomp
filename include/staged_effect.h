#ifndef GUARD_STAGED_EFFECT_H
#define GUARD_STAGED_EFFECT_H
#include "engine.h"
#include "display_animation.h"
#include "scene_presentation_commands.h"
#include "engine_globals.h"
#include "ram.h"
/* Tentative identity: a five-sprite scripted burst/transition effect. */
struct StagedEffect {
    struct Task task;
    struct ScenePresentationReceiver receiver;
    u16 hasScript, state, paletteIndex, scale;
    u32 scriptReleaseFrame;
    s16 travel, initialTravel, spread, initialSpread;
    s16 shakeX, shakeZ;
    u32 frame;
    u32 pendingScriptId;
    union {
        u32 words[2];
        struct {
            u16 x, y, z, pad;
        } axes;
    } position;
    struct DisplaySequenceSprite sprites[5];
    struct SceneAnimation graphics;
    const u16 *activePalette;
    u16 blendedPalette[16];
};

void StagedEffect_MarkOuterSprites(struct StagedEffect *);
void StagedEffect_MarkOuterSpritesAlternate(struct StagedEffect *);
void StagedEffect_BlendPalette(struct StagedEffect *, s32, s32, s32);
void Sound_PlaySong(u32);
#endif
