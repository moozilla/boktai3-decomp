#include "global.h"
#include "player/player.h"
#include "scene/terrain/terrain_sampling.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/ram.h"
#include "engine/engine.h"

struct KeyframeResource {
    u32 words[8];
};

void AnimSprite_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);

s32 Script_GetValue(void);

void Player_UpdateActionFacingSide(struct Player *);

void KeyframeActor_InitKeyframeDisplay(struct Player *p)
{
    struct KeyframeResource *data = ScriptProc_Start(0xcb05, 0xde23);

    if (data) {
        *(struct KeyframeResource *)p->animationData = *data;
        ResourceHeader_Relocate(p->animationData, data);
        AnimSprite_InitLinked(&p->animation, p->animationData, 0, 0x9000, 2, 0, 60, &p->position);
        if (Script_SeekToKeyword(0x64))
            p->savedLinkFacing = Script_GetValue();
        else
            p->savedLinkFacing = 0;
        p->facing = p->savedLinkFacing;
        Player_UpdateActionFacingSide(p);
        p->animationIndex = 0;
        Anim_SetKeyframes(&p->animation, p->animationData, p->heading, 1, 0);
        if (p->side)
            p->animation.flags |= 4;
        else
            p->animation.flags &= ~4;
    }
}

// confidence=low: script body initialization installs this auxiliary sprite and animation.

struct KeyframeAuxiliaryView {
    u8 unknown0[0x30];
    struct SceneVec position;
    u8 unknown38[0x114];
    struct ContactSwarmSprite sprite; /* 0x14C */
    void *spriteData;                 /* 0x178 */
    u8 unknown17C[0x1C];
    u8 spriteBuffer[0x264];               /* 0x198 */
    struct DisplaySequenceState sequence; /* 0x3FC */
    u8 unknown40C[4];
    void *sequenceData; /* 0x410 */
    u8 unknown414[0xD4];
    u16 formCueTimer; /* 0x4E8 */
};

void KeyframeActor_InitAuxiliarySprite(struct Player *p)
{
    struct KeyframeAuxiliaryView *view = (struct KeyframeAuxiliaryView *)p;
    u8 *data = view->spriteBuffer;
    void *sequence;
    struct ContactSwarmSprite *sprite;
    u32 zero;
    struct DisplaySequenceState *frames;

    AnimData_Load(data, 0xD495);
    sequence = ScriptProc_Start(0x922E, 0xAF44);
    view->sequenceData = sequence;
    sprite = &view->sprite;
    view->spriteData = data;
    zero = 0;
    Sprite_Init(sprite, data, 0x1001);
    view->sprite.position.x = view->position.x;
    view->sprite.position.y = view->position.y;
    view->sprite.position.z = view->position.z;
    view->formCueTimer = zero;
    frames = &view->sequence;
    Anim_SetSequence(frames, sequence, 0, p->heading, p->side);
    PlayerCue_StepSequence(sprite, frames);
}

/* tentative: reads the script position and sets up the body, probe and contact node */

void SceneBody_Init(void *, u32, void *, u32, u32, void *);

void KeyframeActor_InitKeyframeDisplay(struct Player *);

void KeyframeActor_InitAuxiliarySprite(struct Player *);

void SceneBody_SetKeyframeDisplay(void *, void *);

void CellObject_Init(void *, void *);

void SceneBody_SetMovementProbe(void *, void *, s32, s32);

void SceneBody_SetPositionOffset(void *, struct SceneVec *);

void SceneActorContacts_RegisterNode(void *, void *, u32);

u32 KeyframeActor_InitBodyFromScript(struct Player *p, u32 entry)
{
    void *body;
    struct SceneVec offset;
    u32 id;

    if (Script_SeekToKeyword('p')) {
        p->savedLinkPosition.x = Script_GetValue();
        p->savedLinkPosition.y = Script_GetValue();
        p->savedLinkPosition.z = Script_GetValue();
    } else {
        p->savedLinkPosition.x = 0;
        p->savedLinkPosition.y = 0;
        p->savedLinkPosition.z = 0;
    }
    p->savedLinkPosition.y = Terrain_SampleHeight((struct ContactPosition *)&p->savedLinkPosition);
    body = &p->positionalEffectGroup;
    SceneBody_Init(body, entry, &p->savedLinkPosition, 0, (u8)p->slot, p);
    KeyframeActor_InitKeyframeDisplay(p);
    KeyframeActor_InitAuxiliarySprite(p);
    p->unk419 = 0;
    SceneBody_SetKeyframeDisplay(body, &p->animation);
    CellObject_Init(&p->movementContactFlags, &p->savedLinkPosition);
    SceneBody_SetMovementProbe(body, &p->movementContactFlags, 0x1e, 0x1e);
    offset.x = 0;
    offset.y = 0x12c;
    offset.z = 0;
    SceneBody_SetPositionOffset(body, &offset);
    id = *(u32 *)&p->positionalEffectGroup;
    *(u16 *)p->positionTracker = id;
    SceneActorContacts_RegisterNode(p->contactListNode, body, 2);
    gTrackedPlayerId[p->slot] = *(u32 *)&p->positionalEffectGroup;
}
