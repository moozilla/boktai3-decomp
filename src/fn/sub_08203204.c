#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
#include "ram.h"
#include "player_cue_sequence.h"
#include "engine_pause.h"
#include "word_flags.h"
struct CannonEmitter {
    u8 pad0[0x18];
    struct SceneVec initialPosition;
    struct DisplaySequenceSprite sprite;
    u8 animationData[0x1c];
    u8 body[0x48];
    s16 flashPalette[16];
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
void TerrainOverlay_QueueAddActor(void *);
void ProximitySequence_TestPlayerDistance(void);
extern u32 gUnk_02000560;
s32 CannonEmitter_UpdateAnimation(struct CannonEmitter *p)
{
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gUnk_02000560))
        return 0;
    Callback_InvokeActor(p, p->state);
    p->sprite.flags &= ~1;
    if (p->flashTimer > 0) {
        *(void **)((u8 *)p + 0x58) = p->flashPalette;
        p->flashTimer--;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->sprite,
                           (struct DisplaySequenceState *)&p->sequence);
}
