#include "monsters/pollen_flower.h"
#include "engine/video/display_animation.h"
#include "engine/math/scene_vec.h"
#include "engine/ram.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/engine_pause.h"
#include "engine/word_flags.h"
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
void ProximitySequence_TestPlayerDistance(void);
#include "player/player.h"
s32 PollenFlower_UpdateAnimation(struct PollenFlower *p)
{
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gPlayerSlotStatus[0]))
        return 0;
    Callback_InvokeActor(p, p->state);
    p->sprite.flags &= ~1;
    if (p->flashTimer > 0) {
        p->graphics.paletteData = p->flashPalette;
        p->flashTimer--;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->sprite,
                           (struct DisplaySequenceState *)&p->sequence);
}
