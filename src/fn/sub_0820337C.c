#include "global.h"
#include "player_cue_sequence.h"
#include "display_animation.h"
#include "scene_vec.h"

struct CannonPaletteColor {
    u16 value;
} __attribute__((packed, aligned(2)));

struct CannonEmitter {
    u8 pad0[0x18];
    struct SceneVec initialPosition;
    struct DisplaySequenceSprite sprite;
    u8 animationData[0x1c];
    u8 body[0x48];
    struct CannonPaletteColor flashPalette[16];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s32 rangeSquared;
    s16 effectX, effectY, effectZ, solid;
    s16 timer, flashTimer, variant, spread;
    void *parent;
    u8 padFC[4];
    void *resource;
};
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
s32 TerrainOverlay_QueueAddActor(void *);
void ProximitySequence_TestPlayerDistance(void);
s32 CannonEmitter_InitializeFromScript(struct CannonEmitter *p, void *parent)
{
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        s32 range = Script_GetValue();
        p->rangeSquared = range * range;
    } else
        p->rangeSquared = 0x40000;
    if (Script_SeekToKeyword('a')) {
        p->effectX = Script_GetValue();
        p->effectY = Script_GetValue();
        p->effectZ = Script_GetValue();
    } else {
        p->effectX = 0;
        p->effectY = 0;
        p->effectZ = 30;
    }
    if (Script_SeekToKeyword('t'))
        p->variant = Script_GetValue();
    if (Script_SeekToKeyword('s'))
        p->spread = Script_GetValue();
    else
        p->spread = 2;
    if (Script_SeekToKeyword('S'))
        p->solid = Script_GetValue();
    p->parent = parent;
    AnimData_Load(p->animationData, 0x7e7d);
    Sprite_Init(&p->sprite, p->animationData, 1);
    *(struct SceneVec *)((u8 *)p + 0x3c) = p->initialPosition;
    AnimData_SelectPaletteBank(p->animationData, p->variant + 0x1ba);
    {
        s32 i = 15;
        struct CannonPaletteColor *palette;
        s32 first;
        do {
            first = (s32)p->flashPalette;
            palette = &p->flashPalette[i];
            palette->value = 0x7fff;
            --i;
        } while ((s32)palette - (s32)sizeof(*palette) >= first);
    }
    p->resource = ScriptProc_Start(0x922e, 0x5184);
    Anim_SetSequence(&p->sequence, p->resource, 6, 0, 0);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->sprite, &p->sequence);
    if (p->solid) {
        SceneBody_Init(p->body, p->parent, (u8 *)p + 0x3c, 0, 4, p);
        SceneBody_SetSprite(p->body, &p->sprite);
        TerrainOverlay_QueueAddActor(p->body);
    }
    p->state = ProximitySequence_TestPlayerDistance;
}
