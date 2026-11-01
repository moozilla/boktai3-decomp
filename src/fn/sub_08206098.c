#include "global.h"

#include "display_animation.h"
#include "scene_vec.h"
#include "player_cue_sequence.h"

struct OozeInit {
    u8 pad0[0x18];
    struct SceneVec initialPosition;
    struct DisplaySequenceSprite sprite;
    u8 animationData[0x1c];
    u8 collider[0x54];
    u8 body[0x48];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s16 extentX, extentY, extentZ, timer;
    s16 interval, variant;
    u8 pad124[0x14];
    s16 repeatAttack, repeatLimit, repeatCount, attackDelay, phase, unknown142;
    s16 facing;
    u16 idleSequence, startSequence, holdSequence, endSequence;
    s16 solid;
    void *parent;
    u8 pad154[4];
    void *resource;
    void *delayScript;
};
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
s32 TerrainOverlay_QueueAddActor(void *);
u32 Script_GetValueSafe(void);
void Ooze_InitAttackCollider(void *);
void Ooze_ReadScriptValueList(void *);
void Ooze_UpdateAttackDelay(void);
static inline u32 Ooze_StepInitialAnimation(struct DisplaySequenceSprite *sprite,
                                            struct DisplaySequenceState *state)
{
    return PlayerCue_StepSequenceEnded(sprite, state);
}

static inline void CopyPosition(struct SceneVec *out, const struct SceneVec *in)
{
    *out = *in;
}
static inline void Ooze_OffsetBodyPosition(struct SceneVec *position, s32 facing)
{
    if (!facing)
        position->x += 128;
    else
        position->z += 128;
}
s32 Ooze_InitializeFromScript(struct OozeInit *p, void *parent)
{
    struct SceneVec position;
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        p->extentX = Script_GetValue();
        p->extentY = Script_GetValue();
        p->extentZ = Script_GetValue();
    } else {
        p->extentX = 30;
        p->extentY = 10;
        p->extentZ = 30;
    }
    if (Script_SeekToKeyword('i'))
        p->interval = Script_GetValue();
    else
        p->interval = 60;
    if (Script_SeekToKeyword('t'))
        p->variant = Script_GetValue();
    if (Script_SeekToKeyword('r'))
        p->delayScript = (void *)Script_GetValueSafe();
    if (Script_SeekToKeyword('w')) {
        p->attackDelay = Script_GetValue();
        p->repeatLimit = Script_GetValue();
        if (p->repeatLimit == 0)
            p->repeatLimit = 1;
        if (p->repeatLimit == 1)
            p->repeatAttack = 0;
        else
            p->repeatAttack = 1;
    } else {
        p->attackDelay = 30;
        p->repeatAttack = 1;
        p->repeatLimit = 3;
    }
    if (Script_SeekToKeyword('h')) {
        p->facing = Script_GetValue();
        {
            u32 facing = (u16)(p->facing = (p->facing - 1) / 2);
            if (facing <= 3) {
                switch (facing) {
                    case 0:
                    case 1:
                        p->facing = 0;
                        break;
                    case 2:
                    case 3:
                        p->facing = 1;
                        break;
                    default:
                        p->facing = 1;
                        break;
                }
            } else
                p->facing = 1;
        }
    } else
        p->facing = 0;
    if (Script_SeekToKeyword('S'))
        p->solid = Script_GetValue();
    p->parent = parent;
    AnimData_Load(p->animationData, 0x7e7d);
    Sprite_Init(&p->sprite, p->animationData, 0x201);
    *(struct SceneVec *)((u8 *)p + 0x3c) = p->initialPosition;
    AnimData_SelectPaletteBank(p->animationData, 0x1c3);
    p->resource = ScriptProc_Start(0x922e, 0x5184);
    Anim_SetSequence(&p->sequence, p->resource, 0x22, (u8)p->facing, 8);
    Ooze_StepInitialAnimation(&p->sprite, &p->sequence);
    if (p->variant == 0) {
        p->idleSequence = 0x22;
        p->startSequence = 0x23;
        p->holdSequence = 0x24;
        p->endSequence = 0x25;
    } else {
        p->idleSequence = 0x1e;
        p->startSequence = 0x1f;
        p->holdSequence = 0x20;
        p->endSequence = 0x21;
    }
    Ooze_InitAttackCollider(p);
    if (p->delayScript)
        Ooze_ReadScriptValueList(p);
    if (p->solid) {
        CopyPosition(&position, (struct SceneVec *)((u8 *)p + 0x3c));
        if (p->variant == 1) {
            Ooze_OffsetBodyPosition(&position, p->facing);
        }
        SceneBody_Init(p->body, p->parent, &position, 0, 4, p);
        SceneBody_SetSprite(p->body, &p->sprite);
        TerrainOverlay_QueueAddActor(p->body);
    }
    p->state = Ooze_UpdateAttackDelay;
}
