#include "monsters/pollen_flower.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/video/display_animation.h"
#include "engine/math/scene_vec.h"

void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
s32 TerrainOverlay_QueueAddActor(void *);
void ProximitySequence_TestPlayerDistance(void);
s32 PollenFlower_InitializeFromScript(struct PollenFlower *flower, void *parent)
{
    s32 i;

    if (Script_SeekToKeyword('p')) {
        flower->initialPosition.x = Script_GetValue();
        flower->initialPosition.y = Script_GetValue();
        flower->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        s32 range = Script_GetValue();
        flower->rangeSquared = range * range;
    } else
        flower->rangeSquared = 0x40000;
    if (Script_SeekToKeyword('a')) {
        flower->effectX = Script_GetValue();
        flower->effectY = Script_GetValue();
        flower->effectZ = Script_GetValue();
    } else {
        flower->effectX = 0;
        flower->effectY = 0;
        flower->effectZ = 30;
    }
    if (Script_SeekToKeyword('t'))
        flower->variant = Script_GetValue();
    if (Script_SeekToKeyword('s'))
        flower->spread = Script_GetValue();
    else
        flower->spread = 2;
    if (Script_SeekToKeyword('S'))
        flower->solid = Script_GetValue();
    flower->parent = parent;
    AnimData_Load(&flower->graphics, 0x7e7d);
    Sprite_Init(&flower->sprite, &flower->graphics, 1);
    flower->sprite.position = flower->initialPosition;
    AnimData_SelectPaletteBank(&flower->graphics, flower->variant + 0x1ba);
    for (i = 0; i < 16; i++)
        flower->flashPalette[i].value = 0x7fff;
    flower->resource = ScriptProc_Start(0x922e, 0x5184);
    Anim_SetSequence(&flower->sequence, flower->resource, 6, 0, 0);
    PlayerCue_StepSequence(&flower->sprite, &flower->sequence);
    if (flower->solid) {
        SceneBody_Init(flower->body, flower->parent, &flower->sprite.position, 0, 4, flower);
        SceneBody_SetSprite(flower->body, &flower->sprite);
        TerrainOverlay_QueueAddActor(flower->body);
    }
    flower->state = ProximitySequence_TestPlayerDistance;
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
