#include "scene_panel_interaction.h"
#include "scene_panel_config.h"
#include "scene_panel_orchestration.h"
#include "scene_panel_commands.h"
#include "game_state.h"
#include "scene_panel_lifecycle.h"
#include "scene_panel_subject.h"

s32 ScenePanel_IsWithinHorizontalRadius(const struct ContactPosition *first,
                                        const struct ContactPosition *second, s32 radius)
{
    s32 x = first->x - second->x;
    s32 z = first->z - second->z;
    if (x > radius || z > radius || (s32)((u32)x * x + (u32)z * z) > (s32)((u32)radius * radius))
        return 0;
    else
        return 1;
}

s32 ScenePanel_CanAcquireTether(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    struct TetherActor *tether = gUnk_02000488;
    if (tether) {
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        if (player->kindValue == 1 && player->tetherMode == 6) {
            if (ScenePanel_IsWithinHorizontalRadius(&tether->position, &panel->center, 70))
                return 1;
        }
    }
    return 0;
}

s32 ScenePanel_AreSubjectsSettled(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    s32 i = 0;
    s32 count = panel->subjectCount;
    if (i < count) {
        s32 limit = count;
        struct ScenePanelSubject **subject = panel->subjects;
        do {
            if ((*subject)->state != 2)
                return 0;
            subject++;
            i++;
        } while (i < limit);
    }
    return 1;
}

// MATCHING_CONCESSION: register-pinning
s32 ScenePanel_CanAcceptPlayer(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
    if (player->kindValue == 1 && (u8)(player->actionMode - 3) > 2) {
        asm("" : : : "r3");
        if (player->tetherMode == 7) {
            if (ScenePanel_IsWithinHorizontalRadius(&player->position, &panel->approachPosition,
                                                    100))
                return 1;
        }
    }
    return 0;
}

s32 ScenePanel_IsTetherDistant(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    if (ScenePanel_IsWithinHorizontalRadius(&gUnk_02000488->position, &panel->center, 800) != 0)
        return 0;
    return 1;
}

void ScenePanel_WaitInitialTether(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    if (ScenePanel_CanAcquireTether(input) != 0) {
        u32 zero;
        PlayerCue_MoveTo(SCENE_PANEL_PLAYER, 0, -1, 0, zero = 0);
        panel->variant = zero;
        ScenePanel_SetEffectState(input, 1);
    }
}

// MATCHING_CONCESSION: register-pinning
void ScenePanel_AdvanceTetherAcquisition(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    u16 *framePointer = &panel->effectFrame;
    u32 frame;
    asm("" : "+r"(panel));
    {
        u32 value = *framePointer;
        value++;
        *framePointer = value;
        frame = (u16)value;
    }
    if (frame <= 5) {
        struct ScenePanelInteractionPackedPosition position;
        s32 divisor;
        s32 previousWeight;
        s32 sourceX;
        u32 direction;
        {
            u16 *sourceFrame = framePointer;
            divisor = 6 - *sourceFrame;
        }
        sourceX = gUnk_02000488->position.x;
        previousWeight = divisor - 1;
        position.x = Div(sourceX * previousWeight + panel->center.x, divisor);
        position.z = Div(gUnk_02000488->position.z * previousWeight + panel->center.z, divisor);
        direction = gUnk_02000488->heading;
        if (*framePointer & 1) {
            u32 facing = direction;
            asm("" : "+r"(facing));
            if (facing) {
                if (facing > 4)
                    direction++;
                else
                    direction--;
                direction &= 7;
            }
        }
        TetherActor_SetInteractionTarget(&position, direction);
    } else if (frame == 6) {
        gUnk_02000488->state = 12;
        gUnk_02000488->position = panel->center;
        {
            u8 *heading = &gUnk_02000488->heading;
            register u32 zero asm("r0");
            zero = 0;
            *heading = zero;
        }
        panel->paletteSprite.flags |= 1;
        Sound_PlaySong(249);
    } else if (frame <= 69) {
        if (panel->variant != 2 && frame == 20) {
            ScenePanel_SetState((void *)panel, ScenePanel_BeginLongSequence);
            Sound_PlaySong(294);
        }
        Camera_RequestShake((70 - panel->effectFrame) >> 1);
    } else if (panel->variant == 0) {
        s32 i = 0;
        if (i < panel->subjectCount) {
            struct ScenePanelSubject **subject = panel->subjects;
            do {
                ScenePanelSubject_SetState((void *)*subject++, 1);
                i++;
            } while (i < panel->subjectCount);
        }
        PlayerCue_Stand(SCENE_PANEL_PLAYER, -1);
        Sound_PlaySong(250);
        ScenePanel_SetEffectState((void *)panel, 2);
    } else if (panel->variant == 1) {
        s32 anyActive;
        u32 count;
        PlayerCue_Stand(SCENE_PANEL_PLAYER, -1);
        PlayerCue_ReleaseControl(SCENE_PANEL_PLAYER);
        anyActive = 0;
        count = panel->subjectCount;
        if (anyActive < (s32)count) {
            struct ScenePanelSubject **subject = panel->subjects;
            s32 remaining = count;
            do {
                if ((*subject)->state == 4)
                    anyActive = 1;
                subject++;
                remaining--;
            } while (remaining != 0);
        }
        if (anyActive)
            ScenePanel_SetEffectState((void *)panel, 4);
        else
            ScenePanel_SetEffectState((void *)panel, 3);
    } else {
        PlayerCue_Stand(SCENE_PANEL_PLAYER, -1);
        PlayerCue_ReleaseControl(SCENE_PANEL_PLAYER);
        Sound_PlaySong(490);
        gUnk_02000488->state = 14;
        ScenePanelRig_SetState(&panel->rig, 1);
        ScenePanel_SetEffectState((void *)panel, 10);
    }
}

void ScenePanel_WaitSettledSubjects(struct ScenePanel *input)
{
    if (ScenePanel_AreSubjectsSettled(input) != 0) {
        PlayerCue_ReleaseControl(SCENE_PANEL_PLAYER);
        ScenePanel_SetEffectState(input, 3);
        ScenePanel_CallPendingScript(input, 0);
    }
}

void ScenePanel_WaitTetherOrActiveSubject(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    s32 i, count;
    if (gUnk_02000488->state != 12 && ScenePanel_CanAcquireTether((void *)panel)) {
        PlayerCue_MoveTo(SCENE_PANEL_PLAYER, 0, -1, 0, 0);
        panel->variant = 1;
        ScenePanel_SetEffectState((void *)panel, 1);
    }
    i = 0;
    count = panel->subjectCount;
    for (; i < count; i++) {
        if (panel->subjects[i]->state == 4) {
            ScenePanel_SetEffectState((void *)panel, 4);
            break;
        }
    }
}

void ScenePanel_UpdateActiveSubjects(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    s32 active = 0;
    s32 i = 0;
    for (; i < panel->subjectCount; i++) {
        struct ScenePanelSubject **subject;
        u32 offset;
        struct ScenePanelSubject **base;
        /* Keep the indexed slot calculation: direct indexing becomes a different
         * induction loop in agbcc. The slot is reloaded after callbacks. */
        offset = i * 4;
        base = panel->subjects;
        subject = (void *)((u8 *)base + offset);
        if ((*subject)->state == 4) {
            active++;
            if ((*subject)->changed) {
                Sound_PlaySong(294);
                ScenePanel_SetState((void *)panel, ScenePanel_BeginLongSequence);
                (*subject)->changed = 0;
            }
        }
    }
    if (!panel->allSubjectsNotified && active >= panel->subjectCount) {
        panel->allSubjectsNotified = 1;
        ScenePanel_CallPendingScript((void *)panel, 1);
    }
    if (!panel->state)
        ScenePanel_SetState((void *)panel, ScenePanel_BeginPulseSequence);
    if (ScenePanel_CanAcceptPlayer((void *)panel)) {
        PlayerCue_WalkTo(SCENE_PANEL_PLAYER, &panel->approachPosition, 0, 4);
        ScenePanel_SetEffectState((void *)panel, 5);
    }
}

void ScenePanel_AdvancePlayerArrival(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
    if (player->tetherMode == 0) {
        struct ScenePanelInteractionPackedPosition position;
        position.x = (u32)(panel->origin.x * 3 + player->position.x) >> 2;
        position.y = (u32)(panel->origin.y * 3 + player->position.y) >> 2;
        position.z = (u32)(panel->origin.z * 3 + player->position.z) >> 2;
        sub_08226380(16, &position);
        if (panel->paletteMode) {
            PlayerCue_Motion7Front(SCENE_PANEL_PLAYER, 1, 0);
            Sound_PlaySong(5);
            ScenePanel_SetState(input, ScenePanel_BeginForwardSequence);
            ScenePanel_SetEffectState(input, 7);
        } else {
            PlayerCue_Motion7Front(SCENE_PANEL_PLAYER, 0, 0);
            Sound_PlaySong(407);
            ScenePanel_SetEffectState(input, 6);
        }
    }
}

// MATCHING_CONCESSION: register-pinning
void ScenePanel_AdvanceSubjectDeparture(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    u32 frame;
    DEBUG_NOP();
    frame = (u16)(++panel->effectFrame);
    if (frame == 30) {
        s32 i = 0;
        for (; i < panel->subjectCount; i++) {
            struct ScenePanelSubject *subject = panel->subjects[i];
            if (subject->state == 4) {
                ScenePanelSubject_SetState((void *)subject, 5);
            }
        }
        ScenePanel_SetState((void *)panel, ScenePanel_BeginLongSequence);
        Sound_FadeOutChannel10(4);
    } else if (frame == 90) {
        Camera_FollowPlayerWithoutCompletion(16);
        PlayerCue_Motion7End(SCENE_PANEL_PLAYER, 0);
    } else if (frame > 105) {
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        if (!player->tetherMode) {
            PlayerCue_ReleaseControl(player);
            ScenePanel_SetEffectState((void *)panel, 3);
            panel->allSubjectsNotified = 0;
            ScenePanel_CallPendingScript((void *)panel, 2);
        }
    }
}

void ScenePanel_AdvanceArrivalRamp(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    u16 *frame = &panel->effectFrame;
    if (*frame <= 69) {
        *frame += 1;
        if (*frame == 70)
            ScenePanel_CallPendingScript(input, 3);
    }
    if (panel->arrivalRampEnabled) {
        u16 *fade = &panel->arrivalRampFrame;
        if (*fade <= 59)
            *fade += 1;
        Camera_RequestShake(*fade >> 1);
    }
}

void ScenePanel_WaitTransitionGate(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    u16 *frame = &panel->effectFrame;
    if (*frame <= 29)
        *frame += 1;
    else if (!panel->gateC12) {
        Sound_PlaySong(490);
        gUnk_02000488->state = 14;
        ScenePanel_SetEffectState(input, 9);
    }
}

void ScenePanel_WaitTetherDirectedState(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    if (gUnk_02000488->state == 13) {
        ScenePanelRig_SetState(&panel->rig, 1);
        ScenePanel_SetEffectState(input, 10);
    }
}

void ScenePanel_WaitRigRelease(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    s32 anyActive = 0;
    if (panel->paletteMode) {
        u32 count = panel->subjectCount;
        if (anyActive < (s32)count) {
            struct ScenePanelSubject **subject = panel->subjects;
            s32 remaining = count;
            do {
                if ((*subject)->state == 4)
                    anyActive = 1;
                subject++;
                remaining--;
            } while (remaining != 0);
        }
    }
    if (gGameState->unk618 > 0)
        ScenePanel_RunScheduledAction((void *)panel);
    if (anyActive)
        panel->effectFrame = 0;
    else if (++panel->effectFrame > 119) {
        ScenePanelRig_SetState(&panel->rig, 6);
        ScenePanel_SetEffectState((void *)panel, 11);
    }
}

void ScenePanel_WaitRigIdle(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    if (panel->rig.state == 0) {
        Sound_PlaySong(491);
        TetherActor_BeginDirectedState(&panel->center);
        ScenePanel_SetEffectState(input, 12);
        ScenePanel_CallPendingScript(input, 5);
    }
}

void ScenePanel_WaitTetherReturn(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    if (ScenePanel_CanAcquireTether(input)) {
        PlayerCue_MoveTo(SCENE_PANEL_PLAYER, 0, -1, 0, 0);
        panel->variant = 2;
        ScenePanel_SetEffectState(input, 1);
    } else if (ScenePanel_IsTetherDistant(input)) {
        struct TetherActor *tether = gUnk_02000488;
        s32 sourceX = tether->position.x;
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        s32 x = sourceX - player->position.x;
        s32 z = tether->position.z - player->position.z;
        s32 direction;
        if (player->actionMode <= 1 || player->actionMode == 6) {
            direction = (((u16)Math_Atan2(x, z) + 112) >> 5) & 7;
            PlayerCue_Face(player, direction, 0);
        } else {
            direction = (((u16)Math_Atan2(x, z) + 112) >> 5) & 7;
            PlayerCue_Stand(player, direction);
            PlayerCue_StateMotion(SCENE_PANEL_PLAYER, 0);
        }
        TetherActor_BeginAuxiliaryState(gUnk_02000488);
        sub_08226380(16, &gUnk_02000488->position);
        ScenePanel_SetEffectState(input, 13);
        ScenePanel_CallPendingScript(input, 6);
    }
}

void ScenePanel_AdvanceTetherReturn(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    panel->effectFrame++;
    switch (panel->effectFrame) {
        case 33: {
            struct TetherActor *tether = gUnk_02000488;
            s32 sourceX = tether->position.x;
            struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
            s32 x = sourceX - player->position.x;
            s32 z = tether->position.z - player->position.z;
            s32 direction = (((u16)Math_Atan2(x, z) + 112) >> 5) & 7;
            PlayerCue_Stand(player, direction);
            break;
        }
        case 64:
            Sound_PlaySong(453);
            break;
        case 300:
            sub_08226380(16, &SCENE_PANEL_PLAYER->position);
            break;
        case 316:
            if (panel->warningSounds[3])
                Sound_PlaySong(panel->warningSounds[3]);
            PlayerCue_Kneel(SCENE_PANEL_PLAYER, -1, 0);
            break;
        case 360:
            ScenePanel_CallPendingScript(input, 7);
            break;
    }
}

/* ScenePanel_HoldInteraction: Empty callback for interaction state 14 that leaves the panel state unchanged */

#define ScenePanel_HoldInteraction sub_0805BCE8

void ScenePanel_HoldInteraction(struct ScenePanel *input)
{
}

void ScenePanel_InitPaletteSprite(struct ScenePanel *input)
{
    struct ScenePanel *parent = (void *)input;
    if (!parent->skipMainSprite) {
        struct ScenePanelPaletteSprite *sprite;
        struct ScenePanelPaletteSprite *positionSprite;
        struct ScenePanelPaletteGraphics *graphics;
        u16 *paletteCopy;
        u16 *destination;
        u16 *source;
        s32 i;
        sprite = &parent->paletteSprite;
        positionSprite = sprite;
        {
            graphics = &parent->paletteSprite.graphics;
        }
        AnimData_Load(graphics, 0xC039);
        Sprite_Init(sprite, graphics, 0);
        sprite->renderMode = 3;
        {
            paletteCopy = parent->paletteSprite.paletteCopy;
        }
        source = graphics->palette;
        destination = paletteCopy;
        for (i = 15; i >= 0; i--)
            *destination++ = *source++;
        graphics->palette = paletteCopy;
        sprite->transform[0] = 0;
        sprite->transform[1] = 0;
        sprite->transform[2] = 0;
        {
            struct ContactPosition position;
            {
                const struct ContactPosition *origin = &parent->origin;
                position = *origin;
            }
            positionSprite->position = position;
        }
        sprite->active = 1;
    }
}

void ScenePanel_InitAuxiliarySprite(struct ScenePanel *input)
{
    struct ScenePanel *parent = (void *)input;
    struct ScenePanelAuxSprite *sprite = &parent->auxiliary;
    void *graphics = sprite->graphics;
    AnimData_Load(graphics, 0x3641);
    Sprite_Init(sprite, graphics, 512);
    sprite->frame = 0;
    sprite->flags |= 2;
    sprite->parameter9 = 96;
    sprite->parameter8 = 96;
    sprite->renderMode = 1;
    sprite->position = parent->center;
    sprite->position.y += 72;
}

void ScenePanel_CreateSubjects(struct ScenePanel *parent)
{
    u32 initialStates[4];
    struct ScenePanelSpawnPosition position;
    s32 contactLimit, colliderSetting;
    u32 category = parent->initialMode;
    u32 kind = 0;
    s32 i;
    if (category == 1)
        kind = 1;
    if (!Script_SeekToKeyword('g')) {
        contactLimit = 100;
        colliderSetting = 0;
    } else {
        contactLimit = Script_GetValue();
        colliderSetting = Script_GetValue();
    }
    if (!Script_SeekToKeyword('a')) {
        for (i = 0; i < parent->subjectCount; i++) {
            parent->subjects[i] = 0;
            initialStates[i] = 0;
        }
    } else {
        for (i = 0; i < parent->subjectCount; i++) {
            parent->subjects[i] = 0;
            initialStates[i] = Script_GetValue();
        }
    }
    parent->parameter = Script_GetKeywordValue('S', 0);
    {
        for (i = 0; i < parent->subjectCount; i++) {
            u32 row, variant, appearDelay;
            s32 negative = -514;
            position = *(struct ScenePanelSpawnPosition *)&parent->origin;
            switch (i) {
                case 0:
                    position.x += negative;
                    position.z += 76;
                    row = 1;
                    variant = 0;
                    appearDelay = 6;
                    break;
                case 2:
                    position.x += 76;
                    position.z += negative;
                    row = 1;
                    variant = 1;
                    appearDelay = 3;
                    break;
                case 3:
                    position.x -= 76;
                    position.z += 514;
                    row = 0;
                    variant = 0;
                    appearDelay = 0;
                    break;
                case 1:
                    position.x += 514;
                    position.z -= 76;
                    row = 0;
                    variant = 1;
                    appearDelay = 9;
                    break;
                default:
                    row = 0;
                    variant = 0;
                    appearDelay = 0;
            }
            parent->subjects[i] = (struct ScenePanelSubject *)ScenePanelSubject_Create(
                (const void *)&position, row, variant, initialStates[i], kind, contactLimit,
                colliderSetting, appearDelay);
        }
    }
}

void ScenePanel_InitAnchorPositions(struct ScenePanel *parent)
{
    if (Script_SeekToKeyword('c')) {
        parent->origin.x = Script_GetValue();
        parent->origin.y = Script_GetValue();
        parent->origin.z = Script_GetValue();
    }
    parent->approachPosition = parent->origin;
    parent->approachPosition.x += 324;
    parent->approachPosition.z += 324;
    parent->center = parent->origin;
    parent->center.x -= 20;
    parent->center.z -= 20;
}

void ScenePanel_InitScriptProperty(struct ScenePanel *input)
{
    struct ScenePanel *parent = (void *)input;
    s32 value = Script_SeekToKeyword('p');
    if (value)
        parent->property = Script_GetValue();
    else
        parent->property = value;
}

void ScenePanel_InitResourceConfiguration(struct ScenePanel *parent)
{
    s32 found = Script_SeekToKeyword('b');
    if (found) {
        parent->resourceIndex = Script_GetValue();
        gGameState->unk618 = Script_GetValue();
        gGameState->unk61C = Script_GetValue();
    } else {
        parent->resourceIndex = found;
        {
            struct GameState *volatile *slot = &gGameState;
            (*slot)->unk618 = 6000;
            (*slot)->unk61C = 6000;
        }
    }
    if (gGameState->unk618 > (gGameState->unk61C >> 2))
        parent->warningStage = 0;
    else
        parent->warningStage = 1;
    switch (parent->resourceIndex) {
        case 0:
            parent->warningSounds[0] = 0x534;
            parent->warningSounds[1] = 0x533;
            parent->warningSounds[2] = 0x359;
            parent->warningSounds[3] = 0x535;
            break;
        case 1:
            parent->warningSounds[0] = 0x4D5;
            parent->warningSounds[1] = 0x4D1;
            parent->warningSounds[2] = 0x4CF;
            parent->warningSounds[3] = 0x4D7;
            break;
        case 2:
            parent->warningSounds[0] = 0x53E;
            parent->warningSounds[1] = 0x53D;
            parent->warningSounds[2] = 0x2EB;
            parent->warningSounds[3] = 0x53F;
            break;
        case 3:
            parent->warningSounds[0] = 0x548;
            parent->warningSounds[1] = 0x549;
            parent->warningSounds[2] = 0x547;
            parent->warningSounds[3] = 0x54A;
            break;
        case 4:
            parent->warningSounds[0] = 0x551;
            parent->warningSounds[1] = 0x553;
            parent->warningSounds[2] = 0x552;
            parent->warningSounds[3] = 0x554;
            break;
        default:
            parent->warningSounds[0] = 0;
            parent->warningSounds[1] = 0;
            parent->warningSounds[2] = 0;
            parent->warningSounds[3] = 0;
    }
    if (Script_SeekToKeyword('T'))
        parent->resourceId = Script_GetValue();
    else
        parent->resourceId = 10;
    ScenePanel_InitScheduledAction((void *)parent);
}

s32 ScenePanel_AreSubjectsIdle(struct ScenePanel *input)
{
    struct ScenePanel *parent = (void *)input;
    s32 i = 0, count = parent->subjectCount;
    if (i < count) {
        s32 limit = count;
        struct ScenePanelSubject **subject = parent->subjects;
        do {
            if ((*subject)->state != 0)
                return 0;
            subject++;
            i++;
        } while (i < limit);
    }
    return 1;
}

void ScenePanel_InitializeInteraction(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    u32 mode = panel->initialMode;
    switch (mode) {
        case 0: {
            s32 option;
            if (ScenePanel_AreSubjectsIdle((void *)panel)) {
                ScenePanel_InitPaletteSprite((void *)panel);
                ScenePanel_SetEffectState((void *)panel, 0);
                return;
            }
            if (Script_SeekToKeyword('O'))
                option = Script_GetValue();
            else
                option = 0;
            if (option && gUnk_02000488) {
                s32 active;
                u32 count;
                u8 *tetherState = &gUnk_02000488->state;
                active = 0;
                *tetherState = 12;
                gUnk_02000488->position = panel->center;
                gUnk_02000488->heading = 0;
                count = panel->subjectCount;
                if (active < (s32)count) {
                    struct ScenePanelSubject **subject = panel->subjects;
                    s32 remaining = count;
                    do {
                        if ((*subject)->state == 4)
                            active = 1;
                        subject++;
                        remaining--;
                    } while (remaining != 0);
                }
                if (active) {
                    ScenePanel_SetEffectState((void *)panel, 4);
                    return;
                }
            } else
                ScenePanel_InitPaletteSprite((void *)panel);
            ScenePanel_SetEffectState((void *)panel, 3);
            break;
        }
        case 1: {
            ScenePanel_InitRig((void *)panel);
            ScenePanel_InitResourceConfiguration((void *)panel);
            gUnk_02000488->state = 12;
            gUnk_02000488->position = panel->center;
            gUnk_02000488->heading = 0;
            gUnk_02000488->unknown1B = mode;
            if (gUnk_02000488->kind == 6)
                panel->resourceScale = 80;
            else if (gUnk_02000488->kind == 7)
                panel->resourceScale = 120;
            else
                panel->resourceScale = 100;
            ScenePanel_InitBackground((void *)panel);
            ScenePanel_InitOverlay((void *)panel);
            ScenePanel_InitAuxiliarySprite((void *)panel);
            ScenePanel_SetEffectState((void *)panel, 8);
        }
    }
}

void ScenePanel_ReadInitialMode(struct ScenePanel *input)
{
    struct ScenePanel *panel = (void *)input;
    s32 mode = Script_SeekToKeyword('m');
    if (mode)
        mode = Script_GetValue();
    panel->initialMode = mode;
}

void ScenePanel_SuppressMainPresentation(void)
{
    if (gScenePanel) {
        struct ScenePanel *panel;
        u8 mode;
        SCENE_PANEL_GLOBAL->skipMainSprite = 1;
        panel = SCENE_PANEL_GLOBAL;
        mode = panel->paletteMode;
        panel->savedMode = mode;
    }
}

void ScenePanel_WriteFullScriptStatus(void)
{
    if (gScenePanel) {
        struct ScriptUtilityReference reference;
        s32 i;
        Script_ReadReference(&reference);
        Script_WriteReferenceValue(&reference, 0, SCENE_PANEL_GLOBAL->initialMode);
        Script_ReadReference(&reference);
        if (gUnk_02000488) {
            if (gUnk_02000488->state == 12)
                Script_WriteReferenceValue(&reference, 0, 1);
            else
                Script_WriteReferenceValue(&reference, 0, 0);
        } else
            Script_WriteReferenceValue(&reference, 0, 0);
        i = 0;
        do {
            struct ScenePanelSubject *subject;
            Script_ReadReference(&reference);
            subject = SCENE_PANEL_GLOBAL->subjects[i];
            if (!subject || subject->state <= 1)
                Script_WriteReferenceValue(&reference, 0, 0);
            else if (subject->state == 4)
                Script_WriteReferenceValue(&reference, 0, 4);
            else
                Script_WriteReferenceValue(&reference, 0, 2);
            i++;
        } while (i <= 3);
    }
}

void ScenePanel_WriteScriptStatus(void)
{
    if (gScenePanel) {
        struct ScriptUtilityReference reference;
        s32 i;
        Script_ReadReference(&reference);
        if (gUnk_02000488) {
            if (gUnk_02000488->state == 12)
                Script_WriteReferenceValue(&reference, 0, 1);
            else
                Script_WriteReferenceValue(&reference, 0, 0);
        } else
            Script_WriteReferenceValue(&reference, 0, 0);
        i = 0;
        do {
            struct ScenePanelSubject *subject;
            Script_ReadReference(&reference);
            subject = SCENE_PANEL_GLOBAL->subjects[i];
            if (!subject || subject->state <= 1)
                Script_WriteReferenceValue(&reference, 0, 0);
            else if (subject->state == 4)
                Script_WriteReferenceValue(&reference, 0, 4);
            else
                Script_WriteReferenceValue(&reference, 0, 2);
            i++;
        } while (i <= 3);
    }
}

void ScenePanel_BeginArrivalRamp(void)
{
    struct ScenePanel *panel = SCENE_PANEL_GLOBAL;
    if (panel) {
        u8 *enabled = &panel->arrivalRampEnabled;
        u32 zero = 0;
        *enabled = 1;
        SCENE_PANEL_GLOBAL->arrivalRampFrame = zero;
    }
}

void ScenePanel_SetTransitionGate(void)
{
    struct ScenePanel *panel = SCENE_PANEL_GLOBAL;
    if (panel)
        panel->gateC12 = 1;
}

void ScenePanel_ClearTransitionGate(void)
{
    struct ScenePanel *panel = SCENE_PANEL_GLOBAL;
    if (panel)
        panel->gateC12 = 0;
}

void ScenePanel_DispatchCommands(struct ScenePanel *owner)
{
    struct ScenePanel *panel = (void *)owner;
    struct ScenePanelReceiver *receiver = &((struct ScenePanel *)owner)->receiver;
    s32 i = 0;
    if (i < receiver->count[gUnk_0200047C->bank]) {
        do {
            struct ScenePanelCommandGlobals *globals = gUnk_0200047C;
            struct ScenePanelCommand *command = receiver->commands[globals->bank][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            switch (command->opcode) {
                case 0:
                    panel->savedMode = command->mode;
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 1:
                    ScenePanel_SetState((void *)panel, ScenePanel_BeginForwardSequence);
                    Trigger_ConsumePending(receiver, 1);
                    break;
            }
            i++;
        } while (i < receiver->count[gUnk_0200047C->bank]);
    }
}

void ScenePanel_UnregisterReceiver(struct ScenePanel *panel)
{
    SceneMessages_UnregisterReceiver(&panel->receiver);
}

void ScenePanel_RegisterReceiver(struct ScenePanel *panel)
{
    SceneMessages_RegisterReceiver(&panel->receiver, panel->actorId, 9);
}

/* ScenePanel_ClearGlobal: Clears the global panel pointer used by ScenePanel_IsPresent */

#define ScenePanel_ClearGlobal sub_0805C754

void ScenePanel_ClearGlobal(void)
{
    gScenePanel = NULL;
}

u32 ScenePanel_IsPresent(void)
{
    u32 present = (u32)gScenePanel;
    if (present != 0)
        present = 1;
    return present;
}

void ScenePanel_SelectMode(struct ScenePanel *panel)
{
    s32 mode;
    if (panel->skipMainSprite == 0)
        mode = gGameState->unk876;
    else
        mode = panel->savedMode;
    panel->paletteMode = mode;
}

s32 ScenePanel_CountActiveSubjects(struct ScenePanel *panel)
{
    s32 active = 0;
    s32 remaining = panel->subjectCount;
    if (active < remaining) {
        struct ScenePanelSubject **subject = panel->subjects;
        do {
            if ((*subject)->state == 4)
                active++;
            subject++;
            remaining--;
        } while (remaining != 0);
    }
    return active;
}

void ScenePanel_UpdateApproachSprite(struct ScenePanel *panel)
{
    if (panel->paletteMode && panel->activeCount &&
        (panel->effectState == 10 || panel->effectState == 14)) {
        u32 scale, offset;
        struct ScenePanelAuxSprite *sprite = &panel->auxiliary;
        sprite->flags &= ~1;
        sprite->frame = 1;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        offset = gRandomIndex * 2;
        scale = *(const u16 *)(offset + (u32)gUnk_0203B400);
        scale &= 15;
        scale += 72;
        panel->auxiliary.parameter9 = scale;
        panel->auxiliary.parameter8 = scale;
    } else
        panel->auxiliary.flags |= 1;
}

void ScenePanel_SubmitPlayerMidpoint(struct ScenePanel *panel)
{
    struct ScenePanelInteractionPackedPosition position;
    struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
    position.x = ((u32)(player->position.x + panel->origin.x) << 15) >> 16;
    position.y = (u16)player->position.y + 150;
    position.z = ((u32)(player->position.z + panel->origin.z) << 15) >> 16;
    Camera_SetPosition(&position);
}

s32 ScenePanel_TickLifecycle(struct ScenePanel *parent)
{
    u32 playerKind;
    ScenePanel_DispatchCommands((void *)parent);
    ScenePanel_SelectMode((void *)parent);
    parent->activeCount = ScenePanel_CountActiveSubjects((void *)parent);
    playerKind = SCENE_PANEL_PLAYER->kindValue;
    if (playerKind == 4)
        return 0;
    if (parent->initialMode == 1 && parent->effectState != 14 && playerKind == 1 &&
        !gGameState->unk618) {
        ScenePanel_SetEffectState((void *)parent, 14);
        ScenePanelRig_SetState(&parent->rig, 9);
        ScenePanel_NoOpAction((void *)parent);
        ScenePanel_CallPendingScript((void *)parent, 8);
    }
    gUnk_08605ABC[parent->effectState]((void *)parent);
    if (parent->initialMode == 0)
        ScenePanel_UpdatePaletteState((void *)parent);
    else {
        ScenePanel_TickRig((void *)parent);
        ScenePanel_SubmitPlayerMidpoint((void *)parent);
        ScenePanel_DrawBackgroundLayers((void *)parent);
        ScenePanel_LoadModePalette((void *)parent);
        ScenePanel_UpdateApproachSprite((void *)parent);
        if (parent->activated) {
            if (!parent->skipMainSprite) {
                if (gGameState->unk618 == 0) {
                    if (parent->warningSounds[2])
                        Sound_PlaySong(parent->warningSounds[2]);
                } else {
                    if (parent->warningSounds[parent->warningStage])
                        Sound_PlaySong(parent->warningSounds[parent->warningStage]);
                }
                Sound_PlaySong(0x29B);
            }
            parent->activated = 0;
        }
    }
    /* The non-void callback preserves the original fall-through return ABI. */
    ScenePanel_UpdateParticles((void *)parent);
}

s32 ScenePanel_DestroyLifecycle(struct ScenePanel *parent)
{
    if (parent->initialMode == 0) {
        if (parent->paletteSprite.active)
            Sprite_Remove(&parent->paletteSprite);
    } else {
        ScenePanelRig_RemoveSprites(&parent->rig);
        Sprite_Remove(&parent->auxiliary);
        Bg_SetPriority(1, 1);
    }
    ScenePanel_DestroyParticles((void *)parent);
    ScenePanel_UnregisterReceiver((void *)parent);
    gScenePanel = 0;
    return 0;
}

s32 ScenePanel_InitializeLifecycle(struct ScenePanel *parent)
{
    s32 found = Script_SeekToKeyword('d');
    if (found)
        parent->skipMainSprite = Script_GetValue();
    else
        parent->skipMainSprite = found;
    {
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        parent->savedPlayer = player;
    }
    parent->savedTether = gUnk_02000488;
    ScenePanel_InitAnchorPositions((void *)parent);
    ScenePanel_ReadInitialMode((void *)parent);
    if (Script_SeekToKeyword('n'))
        parent->subjectCount = Script_GetValue();
    else
        parent->subjectCount = 4;
    ScenePanel_CreateSubjects((void *)parent);
    ScenePanel_InitializeInteraction((void *)parent);
    ScenePanel_InitParticles((void *)parent);
    ScenePanel_InitTimePalette((void *)parent);
    ScenePanel_InitScriptProperty((void *)parent);
    ScenePanel_RegisterReceiver((void *)parent);
    {
        s32 mode = gGameState->unk876;
        parent->paletteMode = mode;
    }
    gScenePanel = (void *)parent;
    return 0;
}

struct ScenePanel *ScenePanel_Create(u32 actorId)
{
    struct ScenePanel *panel;
    if (gScenePanel)
        return gScenePanel;
    panel = Task_Create(9, 0x17E4);
    if (panel) {
        Task_SetCallbacks(panel, (void (*)(void *))ScenePanel_TickLifecycle,
                          (void (*)(void *))ScenePanel_DestroyLifecycle);
        panel->actorId = actorId;
        if (ScenePanel_InitializeLifecycle((void *)panel) < 0) {
            Task_Kill(panel);
            return NULL;
        }
    }
    return panel;
}
