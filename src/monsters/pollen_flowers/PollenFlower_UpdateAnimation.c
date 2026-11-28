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
s32 PollenFlower_UpdateAnimation(struct PollenFlower *flower)
{
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gPlayerSlotStatus[0]))
        return 0;
    Callback_InvokeActor(flower, flower->state);
    flower->sprite.flags &= ~1;
    if (flower->flashTimer > 0) {
        flower->graphics.paletteData = flower->flashPalette;
        flower->flashTimer--;
    }
    PlayerCue_StepSequence(&flower->sprite, &flower->sequence);
}
