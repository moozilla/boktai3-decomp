#include "monsters/ooze.h"

#include "engine/video/display_animation.h"
#include "engine/math/scene_vec.h"
#include "player/cutscene/player_cue_sequence.h"

void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
s32 TerrainOverlay_QueueAddActor(void *);
u32 Script_GetValueSafe(void);
void Ooze_InitAttackCollider(void *);
void Ooze_ReadScriptValueList(void *);
void Ooze_UpdateAttackDelay(void);
s32 Ooze_InitializeFromScript(struct Ooze *ooze, void *parent)
{
    struct SceneVec position;
    u32 facing;
    if (Script_SeekToKeyword('p')) {
        ooze->initialPosition.x = Script_GetValue();
        ooze->initialPosition.y = Script_GetValue();
        ooze->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('a')) {
        ooze->extentX = Script_GetValue();
        ooze->extentY = Script_GetValue();
        ooze->extentZ = Script_GetValue();
    } else {
        ooze->extentX = 30;
        ooze->extentY = 10;
        ooze->extentZ = 30;
    }
    if (Script_SeekToKeyword('i'))
        ooze->interval = Script_GetValue();
    else
        ooze->interval = 60;
    if (Script_SeekToKeyword('t'))
        ooze->variant = Script_GetValue();
    if (Script_SeekToKeyword('r'))
        ooze->delayScript = (void *)Script_GetValueSafe();
    if (Script_SeekToKeyword('w')) {
        ooze->attackDelay = Script_GetValue();
        ooze->repeatLimit = Script_GetValue();
        if (ooze->repeatLimit == 0)
            ooze->repeatLimit = 1;
        if (ooze->repeatLimit == 1)
            ooze->repeatAttack = 0;
        else
            ooze->repeatAttack = 1;
    } else {
        ooze->attackDelay = 30;
        ooze->repeatAttack = 1;
        ooze->repeatLimit = 3;
    }
    if (Script_SeekToKeyword('h')) {
        ooze->facing = Script_GetValue();
        facing = (u16)(ooze->facing = (ooze->facing - 1) / 2);
        if (facing <= 3) {
            switch (facing) {
                case 0:
                case 1:
                    ooze->facing = 0;
                    break;
                case 2:
                case 3:
                default:
                    ooze->facing = 1;
                    break;
            }
        } else
            ooze->facing = 1;
    } else
        ooze->facing = 0;
    if (Script_SeekToKeyword('S'))
        ooze->solid = Script_GetValue();
    ooze->parent = parent;
    AnimData_Load(&ooze->graphics, 0x7e7d);
    Sprite_Init(&ooze->sprite, &ooze->graphics, 0x201);
    ooze->sprite.position = ooze->initialPosition;
    AnimData_SelectPaletteBank(&ooze->graphics, 0x1c3);
    ooze->resource = ScriptProc_Start(0x922e, 0x5184);
    Anim_SetSequence(&ooze->sequence, ooze->resource, 0x22, (u8)ooze->facing, 8);
    PlayerCue_StepSequenceEnded(&ooze->sprite, &ooze->sequence);
    if (ooze->variant == 0) {
        ooze->idleSequence = 0x22;
        ooze->startSequence = 0x23;
        ooze->holdSequence = 0x24;
        ooze->endSequence = 0x25;
    } else {
        ooze->idleSequence = 0x1e;
        ooze->startSequence = 0x1f;
        ooze->holdSequence = 0x20;
        ooze->endSequence = 0x21;
    }
    Ooze_InitAttackCollider(ooze);
    if (ooze->delayScript)
        Ooze_ReadScriptValueList(ooze);
    if (ooze->solid) {
        SceneVec_Copy(&position, (struct SceneVec *)&ooze->sprite.position);
        if (ooze->variant == 1) {
            if (!ooze->facing)
                position.x += 128;
            else
                position.z += 128;
        }
        SceneBody_Init(ooze->body, ooze->parent, &position, 0, 4, ooze);
        SceneBody_SetSprite(ooze->body, &ooze->sprite);
        TerrainOverlay_QueueAddActor(ooze->body);
    }
    ooze->state = Ooze_UpdateAttackDelay;
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
