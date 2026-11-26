#include "objects/pile_driver/pile_driver_interaction.h"
#include "objects/pile_driver/pile_driver_config.h"
#include "objects/pile_driver/pile_driver_orchestration.h"
#include "objects/pile_driver/pile_driver_commands.h"
#include "system/game_state/game_state.h"
#include "objects/pile_driver/pile_driver_lifecycle.h"
#include "objects/pile_driver/pile_driver_subject.h"

s32 PileDriver_IsWithinHorizontalRadius(const struct ContactPosition *first,
                                        const struct ContactPosition *second, s32 radius)
{
    s32 x = first->x - second->x;
    s32 z = first->z - second->z;
    if (x > radius || z > radius || (s32)((u32)x * x + (u32)z * z) > (s32)((u32)radius * radius))
        return 0;
    else
        return 1;
}

s32 PileDriver_CanAcquireTether(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    struct TetherActor *tether = gTetherActor;
    if (tether) {
        struct PileDriverInteractionPlayer *player = SCENE_PANEL_PLAYER;
        if (player->kindValue == 1 && player->tetherMode == 6) {
            if (PileDriver_IsWithinHorizontalRadius(&tether->position, &panel->center, 70))
                return 1;
        }
    }
    return 0;
}

s32 PileDriver_AreSubjectsSettled(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    s32 i = 0;
    s32 count = panel->subjectCount;
    if (i < count) {
        s32 limit = count;
        struct PileDriverSubject **subject = panel->subjects;
        do {
            if ((*subject)->state != 2)
                return 0;
            subject++;
            i++;
        } while (i < limit);
    }
    return 1;
}

s32 PileDriver_CanAcceptPlayer(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    struct PileDriverInteractionPlayer *player = SCENE_PANEL_PLAYER;
    if (player->kindValue != 1)
        return 0;
    if ((u8)(player->actionMode - 3) <= 2)
        return 0;
    if (player->tetherMode == 7) {
        if (PileDriver_IsWithinHorizontalRadius(&player->position, &panel->approachPosition, 100))
            return 1;
        return 0;
    }
    return 0;
}

s32 PileDriver_IsTetherDistant(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    if (PileDriver_IsWithinHorizontalRadius(&gTetherActor->position, &panel->center, 800) != 0)
        return 0;
    return 1;
}

void PileDriver_WaitInitialTether(struct PileDriver *panel)
{
    if (!PileDriver_CanAcquireTether(panel))
        return;
    PlayerCue_MoveTo(SCENE_PANEL_PLAYER, 0, -1, 0, 0);
    panel->variant = 0;
    PileDriver_SetEffectState(panel, 1);
}

void PileDriver_AdvanceTetherAcquisition(struct PileDriver *panel)
{
    u16 *framePointer = &panel->effectFrame;
    u32 frame;
    {
        u32 value = *framePointer;
        value++;
        *framePointer = value;
        frame = (u16)value;
    }
    if (frame <= 5) {
        struct PileDriverInteractionPackedPosition position;
        s32 divisor;
        s32 previousWeight;
        s32 sourceX;
        u32 direction;
        {
            u16 *sourceFrame = framePointer;
            divisor = 6 - *sourceFrame;
        }
        sourceX = gTetherActor->position.x;
        previousWeight = divisor - 1;
        position.x = Div(sourceX * previousWeight + panel->center.x, divisor);
        position.z = Div(gTetherActor->position.z * previousWeight + panel->center.z, divisor);
        direction = gTetherActor->heading;
        if (*framePointer & 1) {
            u8 facing = direction;
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
        {
            u8 *state = &gTetherActor->state;
            *state = 12;
        }
        gTetherActor->position = panel->center;
        {
            u8 *heading = &gTetherActor->heading;
            *heading = 0;
        }
        panel->paletteSprite.flags |= 1;
        Sound_PlaySong(249);
    } else if (frame <= 69) {
        if (panel->variant != 2 && frame == 20) {
            PileDriver_SetState((void *)panel, PileDriver_BeginLongSequence);
            Sound_PlaySong(294);
        }
        Camera_RequestShake((70 - panel->effectFrame) >> 1);
    } else if (panel->variant == 0) {
        s32 i = 0;
        if (i < panel->subjectCount) {
            struct PileDriverSubject **subject = panel->subjects;
            do {
                PileDriverSubject_SetState((void *)*subject++, 1);
                i++;
            } while (i < panel->subjectCount);
        }
        PlayerCue_Stand(SCENE_PANEL_PLAYER, -1);
        Sound_PlaySong(250);
        PileDriver_SetEffectState((void *)panel, 2);
    } else if (panel->variant == 1) {
        s32 anyActive;
        u32 count;
        PlayerCue_Stand(SCENE_PANEL_PLAYER, -1);
        PlayerCue_ReleaseControl(SCENE_PANEL_PLAYER);
        anyActive = 0;
        count = panel->subjectCount;
        if (anyActive < (s32)count) {
            struct PileDriverSubject **subject = panel->subjects;
            s32 remaining = count;
            do {
                if ((*subject)->state == 4)
                    anyActive = 1;
                subject++;
                remaining--;
            } while (remaining != 0);
        }
        if (anyActive)
            PileDriver_SetEffectState((void *)panel, 4);
        else
            PileDriver_SetEffectState((void *)panel, 3);
    } else {
        PlayerCue_Stand(SCENE_PANEL_PLAYER, -1);
        PlayerCue_ReleaseControl(SCENE_PANEL_PLAYER);
        Sound_PlaySong(490);
        gTetherActor->state = 14;
        PileDriverRig_SetState(&panel->rig, 1);
        PileDriver_SetEffectState((void *)panel, 10);
    }
}

void PileDriver_WaitSettledSubjects(struct PileDriver *input)
{
    if (PileDriver_AreSubjectsSettled(input) != 0) {
        PlayerCue_ReleaseControl(SCENE_PANEL_PLAYER);
        PileDriver_SetEffectState(input, 3);
        PileDriver_CallPendingScript(input, 0);
    }
}

void PileDriver_WaitTetherOrActiveSubject(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    s32 i, count;
    if (gTetherActor->state != 12 && PileDriver_CanAcquireTether((void *)panel)) {
        PlayerCue_MoveTo(SCENE_PANEL_PLAYER, 0, -1, 0, 0);
        panel->variant = 1;
        PileDriver_SetEffectState((void *)panel, 1);
    }
    i = 0;
    count = panel->subjectCount;
    for (; i < count; i++) {
        if (panel->subjects[i]->state == 4) {
            PileDriver_SetEffectState((void *)panel, 4);
            break;
        }
    }
}

void PileDriver_UpdateActiveSubjects(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    s32 active = 0;
    s32 i = 0;
    for (; i < panel->subjectCount; i++) {
        struct PileDriverSubject **subject;
        u32 offset;
        struct PileDriverSubject **base;
        /* Keep the indexed slot calculation: direct indexing becomes a different
         * induction loop in agbcc. The slot is reloaded after callbacks. */
        offset = i * 4;
        base = panel->subjects;
        subject = (void *)((u8 *)base + offset);
        if ((*subject)->state == 4) {
            active++;
            if ((*subject)->changed) {
                Sound_PlaySong(294);
                PileDriver_SetState((void *)panel, PileDriver_BeginLongSequence);
                (*subject)->changed = 0;
            }
        }
    }
    if (!panel->allSubjectsNotified && active >= panel->subjectCount) {
        panel->allSubjectsNotified = 1;
        PileDriver_CallPendingScript((void *)panel, 1);
    }
    if (!panel->state)
        PileDriver_SetState((void *)panel, PileDriver_BeginPulseSequence);
    if (PileDriver_CanAcceptPlayer((void *)panel)) {
        PlayerCue_WalkTo(SCENE_PANEL_PLAYER, &panel->approachPosition, 0, 4);
        PileDriver_SetEffectState((void *)panel, 5);
    }
}

void PileDriver_AdvancePlayerArrival(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    struct PileDriverInteractionPlayer *player = SCENE_PANEL_PLAYER;
    if (player->tetherMode == 0) {
        struct PileDriverInteractionPackedPosition position;
        position.x = (u32)(panel->origin.x * 3 + player->position.x) >> 2;
        position.y = (u32)(panel->origin.y * 3 + player->position.y) >> 2;
        position.z = (u32)(panel->origin.z * 3 + player->position.z) >> 2;
        sub_08226380(16, &position);
        if (panel->paletteMode) {
            PlayerCue_Motion7Front(SCENE_PANEL_PLAYER, 1, 0);
            Sound_PlaySong(5);
            PileDriver_SetState(input, PileDriver_BeginForwardSequence);
            PileDriver_SetEffectState(input, 7);
        } else {
            PlayerCue_Motion7Front(SCENE_PANEL_PLAYER, 0, 0);
            Sound_PlaySong(407);
            PileDriver_SetEffectState(input, 6);
        }
    }
}

void PileDriver_AdvanceSubjectDeparture(struct PileDriver *panel)
{
    u32 frame;
    frame = (u16)(++panel->effectFrame);
    if (frame == 30) {
        s32 i = 0;
        for (; i < panel->subjectCount; i++) {
            struct PileDriverSubject *subject = panel->subjects[i];
            if (subject->state == 4) {
                PileDriverSubject_SetState((void *)subject, 5);
            }
        }
        PileDriver_SetState((void *)panel, PileDriver_BeginLongSequence);
        Sound_FadeOutChannel10(4);
    } else if (frame == 90) {
        Camera_FollowPlayerWithoutCompletion(16);
        PlayerCue_Motion7End(SCENE_PANEL_PLAYER, 0);
    } else if (frame > 105) {
        struct PileDriverInteractionPlayer *player = SCENE_PANEL_PLAYER;
        if (!player->tetherMode) {
            PlayerCue_ReleaseControl(player);
            PileDriver_SetEffectState((void *)panel, 3);
            panel->allSubjectsNotified = 0;
            PileDriver_CallPendingScript((void *)panel, 2);
        }
    }
}

void PileDriver_AdvanceArrivalRamp(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    u16 *frame = &panel->effectFrame;
    if (*frame <= 69) {
        *frame += 1;
        if (*frame == 70)
            PileDriver_CallPendingScript(input, 3);
    }
    if (panel->arrivalRampEnabled) {
        u16 *fade = &panel->arrivalRampFrame;
        if (*fade <= 59)
            *fade += 1;
        Camera_RequestShake(*fade >> 1);
    }
}

void PileDriver_WaitTransitionGate(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    u16 *frame = &panel->effectFrame;
    if (*frame <= 29)
        *frame += 1;
    else if (!panel->gateC12) {
        Sound_PlaySong(490);
        gTetherActor->state = 14;
        PileDriver_SetEffectState(input, 9);
    }
}

void PileDriver_WaitTetherDirectedState(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    if (gTetherActor->state == 13) {
        PileDriverRig_SetState(&panel->rig, 1);
        PileDriver_SetEffectState(input, 10);
    }
}

void PileDriver_WaitRigRelease(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    s32 anyActive = 0;
    if (panel->paletteMode) {
        u32 count = panel->subjectCount;
        if (anyActive < (s32)count) {
            struct PileDriverSubject **subject = panel->subjects;
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
        PileDriver_RunScheduledAction((void *)panel);
    if (anyActive)
        panel->effectFrame = 0;
    else if (++panel->effectFrame > 119) {
        PileDriverRig_SetState(&panel->rig, 6);
        PileDriver_SetEffectState((void *)panel, 11);
    }
}

void PileDriver_WaitRigIdle(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    if (panel->rig.state == 0) {
        Sound_PlaySong(491);
        TetherActor_BeginDirectedState(&panel->center);
        PileDriver_SetEffectState(input, 12);
        PileDriver_CallPendingScript(input, 5);
    }
}

void PileDriver_WaitTetherReturn(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    if (PileDriver_CanAcquireTether(input)) {
        PlayerCue_MoveTo(SCENE_PANEL_PLAYER, 0, -1, 0, 0);
        panel->variant = 2;
        PileDriver_SetEffectState(input, 1);
    } else if (PileDriver_IsTetherDistant(input)) {
        struct TetherActor *tether = gTetherActor;
        s32 sourceX = tether->position.x;
        struct PileDriverInteractionPlayer *player = SCENE_PANEL_PLAYER;
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
        TetherActor_BeginAuxiliaryState(gTetherActor);
        sub_08226380(16, &gTetherActor->position);
        PileDriver_SetEffectState(input, 13);
        PileDriver_CallPendingScript(input, 6);
    }
}

void PileDriver_AdvanceTetherReturn(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    panel->effectFrame++;
    switch (panel->effectFrame) {
        case 33: {
            struct TetherActor *tether = gTetherActor;
            s32 sourceX = tether->position.x;
            struct PileDriverInteractionPlayer *player = SCENE_PANEL_PLAYER;
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
            PileDriver_CallPendingScript(input, 7);
            break;
    }
}

/* PileDriver_HoldInteraction: Empty callback for interaction state 14 that leaves the panel state unchanged */


void PileDriver_HoldInteraction(struct PileDriver *input)
{
}

void PileDriver_InitPaletteSprite(struct PileDriver *input)
{
    struct PileDriver *parent = (void *)input;
    if (!parent->skipMainSprite) {
        struct PileDriverPaletteSprite *sprite;
        struct PileDriverPaletteSprite *positionSprite;
        struct PileDriverPaletteGraphics *graphics;
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

void PileDriver_InitAuxiliarySprite(struct PileDriver *input)
{
    struct PileDriver *parent = (void *)input;
    struct PileDriverAuxSprite *sprite = &parent->auxiliary;
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

void PileDriver_CreateSubjects(struct PileDriver *parent)
{
    u32 initialStates[4];
    struct ContactPosition position;
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
            position = parent->origin;
            switch (i) {
                case 0:
                    position.x -= 514;
                    position.z += 76;
                    row = 1;
                    variant = 0;
                    appearDelay = 6;
                    break;
                case 2:
                    position.x += 76;
                    position.z -= 514;
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
            parent->subjects[i] = (struct PileDriverSubject *)PileDriverSubject_Create(
                &position, row, variant, initialStates[i], kind, contactLimit, colliderSetting,
                appearDelay);
        }
    }
}

void PileDriver_InitAnchorPositions(struct PileDriver *parent)
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

void PileDriver_InitScriptProperty(struct PileDriver *input)
{
    struct PileDriver *parent = (void *)input;
    s32 value = Script_SeekToKeyword('p');
    if (value)
        parent->property = Script_GetValue();
    else
        parent->property = value;
}

void PileDriver_InitResourceConfiguration(struct PileDriver *parent)
{
    s32 found = Script_SeekToKeyword('b');
    if (found) {
        parent->resourceIndex = Script_GetValue();
        gGameState->unk618 = Script_GetValue();
        gGameState->unk61C = Script_GetValue();
    } else {
        s32 *resource;
        parent->resourceIndex = found;
        resource = &gGameState->unk618;
        *resource = 6000;
        gGameState->unk61C = 6000;
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
    PileDriver_InitScheduledAction((void *)parent);
}

s32 PileDriver_AreSubjectsIdle(struct PileDriver *input)
{
    struct PileDriver *parent = (void *)input;
    s32 i = 0, count = parent->subjectCount;
    if (i < count) {
        s32 limit = count;
        struct PileDriverSubject **subject = parent->subjects;
        do {
            if ((*subject)->state != 0)
                return 0;
            subject++;
            i++;
        } while (i < limit);
    }
    return 1;
}

void PileDriver_InitializeInteraction(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    u32 mode = panel->initialMode;
    switch (mode) {
        case 0: {
            s32 option;
            if (PileDriver_AreSubjectsIdle((void *)panel)) {
                PileDriver_InitPaletteSprite((void *)panel);
                PileDriver_SetEffectState((void *)panel, 0);
                return;
            }
            if (Script_SeekToKeyword('O'))
                option = Script_GetValue();
            else
                option = 0;
            if (option && gTetherActor) {
                s32 active;
                u32 count;
                u8 *tetherState = &gTetherActor->state;
                active = 0;
                *tetherState = 12;
                gTetherActor->position = panel->center;
                gTetherActor->heading = 0;
                count = panel->subjectCount;
                if (active < (s32)count) {
                    struct PileDriverSubject **subject = panel->subjects;
                    s32 remaining = count;
                    do {
                        if ((*subject)->state == 4)
                            active = 1;
                        subject++;
                        remaining--;
                    } while (remaining != 0);
                }
                if (active) {
                    PileDriver_SetEffectState((void *)panel, 4);
                    return;
                }
            } else
                PileDriver_InitPaletteSprite((void *)panel);
            PileDriver_SetEffectState((void *)panel, 3);
            break;
        }
        case 1: {
            PileDriver_InitRig((void *)panel);
            PileDriver_InitResourceConfiguration((void *)panel);
            gTetherActor->state = 12;
            gTetherActor->position = panel->center;
            gTetherActor->heading = 0;
            gTetherActor->unknown1B = mode;
            if (gTetherActor->kind == 6)
                panel->resourceScale = 80;
            else if (gTetherActor->kind == 7)
                panel->resourceScale = 120;
            else
                panel->resourceScale = 100;
            PileDriver_InitBackground((void *)panel);
            PileDriver_InitOverlay((void *)panel);
            PileDriver_InitAuxiliarySprite((void *)panel);
            PileDriver_SetEffectState((void *)panel, 8);
        }
    }
}

void PileDriver_ReadInitialMode(struct PileDriver *input)
{
    struct PileDriver *panel = (void *)input;
    s32 mode = Script_SeekToKeyword('m');
    if (mode)
        mode = Script_GetValue();
    panel->initialMode = mode;
}

void PileDriver_SuppressMainPresentation(void)
{
    if (gScenePanel) {
        struct PileDriver *panel;
        u8 mode;
        SCENE_PANEL_GLOBAL->skipMainSprite = 1;
        panel = SCENE_PANEL_GLOBAL;
        mode = panel->paletteMode;
        panel->savedMode = mode;
    }
}

void PileDriver_WriteFullScriptStatus(void)
{
    if (gScenePanel) {
        struct ScriptUtilityReference reference;
        s32 i;
        Script_ReadReference(&reference);
        Script_WriteReferenceValue(&reference, 0, SCENE_PANEL_GLOBAL->initialMode);
        Script_ReadReference(&reference);
        if (gTetherActor) {
            if (gTetherActor->state == 12)
                Script_WriteReferenceValue(&reference, 0, 1);
            else
                Script_WriteReferenceValue(&reference, 0, 0);
        } else
            Script_WriteReferenceValue(&reference, 0, 0);
        i = 0;
        do {
            struct PileDriverSubject *subject;
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

void PileDriver_WriteScriptStatus(void)
{
    if (gScenePanel) {
        struct ScriptUtilityReference reference;
        s32 i;
        Script_ReadReference(&reference);
        if (gTetherActor) {
            if (gTetherActor->state == 12)
                Script_WriteReferenceValue(&reference, 0, 1);
            else
                Script_WriteReferenceValue(&reference, 0, 0);
        } else
            Script_WriteReferenceValue(&reference, 0, 0);
        i = 0;
        do {
            struct PileDriverSubject *subject;
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

void PileDriver_BeginArrivalRamp(void)
{
    struct PileDriver *panel = SCENE_PANEL_GLOBAL;
    if (panel) {
        panel->arrivalRampEnabled = 1;
        SCENE_PANEL_GLOBAL->arrivalRampFrame = 0;
    }
}

void PileDriver_SetTransitionGate(void)
{
    struct PileDriver *panel = SCENE_PANEL_GLOBAL;
    if (panel)
        panel->gateC12 = 1;
}

void PileDriver_ClearTransitionGate(void)
{
    struct PileDriver *panel = SCENE_PANEL_GLOBAL;
    if (panel)
        panel->gateC12 = 0;
}

void PileDriver_DispatchCommands(struct PileDriver *owner)
{
    struct PileDriver *panel = (void *)owner;
    struct PileDriverReceiver *receiver = &((struct PileDriver *)owner)->receiver;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState->bank]) {
        do {
            struct PileDriverCommandGlobals *globals = gSceneEventDispatchState;
            struct PileDriverCommand *command = receiver->commands[globals->bank][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            switch (command->opcode) {
                case 0:
                    panel->savedMode = command->mode;
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 1:
                    PileDriver_SetState((void *)panel, PileDriver_BeginForwardSequence);
                    Trigger_ConsumePending(receiver, 1);
                    break;
            }
            i++;
        } while (i < receiver->count[gSceneEventDispatchState->bank]);
    }
}

void PileDriver_UnregisterReceiver(struct PileDriver *panel)
{
    SceneMessages_UnregisterReceiver(&panel->receiver);
}

void PileDriver_RegisterReceiver(struct PileDriver *panel)
{
    SceneMessages_RegisterReceiver(&panel->receiver, panel->actorId, 9);
}

/* PileDriver_ClearGlobal: Clears the global panel pointer used by PileDriver_IsPresent */


void PileDriver_ClearGlobal(void)
{
    gScenePanel = NULL;
}

u32 PileDriver_IsPresent(void)
{
    u32 present = (u32)gScenePanel;
    if (present != 0)
        present = 1;
    return present;
}

void PileDriver_SelectMode(struct PileDriver *panel)
{
    s32 mode;
    if (panel->skipMainSprite == 0)
        mode = gGameState->unk876;
    else
        mode = panel->savedMode;
    panel->paletteMode = mode;
}

s32 PileDriver_CountActiveSubjects(struct PileDriver *panel)
{
    s32 active = 0;
    s32 remaining = panel->subjectCount;
    if (active < remaining) {
        struct PileDriverSubject **subject = panel->subjects;
        do {
            if ((*subject)->state == 4)
                active++;
            subject++;
            remaining--;
        } while (remaining != 0);
    }
    return active;
}

void PileDriver_UpdateApproachSprite(struct PileDriver *panel)
{
    if (panel->paletteMode && panel->activeCount &&
        (panel->effectState == 10 || panel->effectState == 14)) {
        u32 scale, offset;
        struct PileDriverAuxSprite *sprite = &panel->auxiliary;
        sprite->flags &= ~1;
        sprite->frame = 1;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        offset = gRandomIndex * 2;
        scale = *(const u16 *)(offset + (u32)gRandomTable);
        scale &= 15;
        scale += 72;
        panel->auxiliary.parameter9 = scale;
        panel->auxiliary.parameter8 = scale;
    } else
        panel->auxiliary.flags |= 1;
}

void PileDriver_SubmitPlayerMidpoint(struct PileDriver *panel)
{
    struct PileDriverInteractionPackedPosition position;
    struct PileDriverInteractionPlayer *player = SCENE_PANEL_PLAYER;
    position.x = ((u32)(player->position.x + panel->origin.x) << 15) >> 16;
    position.y = (u16)player->position.y + 150;
    position.z = ((u32)(player->position.z + panel->origin.z) << 15) >> 16;
    Camera_SetPosition(&position);
}

s32 PileDriver_TickLifecycle(struct PileDriver *parent)
{
    u32 playerKind;
    PileDriver_DispatchCommands((void *)parent);
    PileDriver_SelectMode((void *)parent);
    parent->activeCount = PileDriver_CountActiveSubjects((void *)parent);
    playerKind = SCENE_PANEL_PLAYER->kindValue;
    if (playerKind == 4)
        return 0;
    if (parent->initialMode == 1 && parent->effectState != 14 && playerKind == 1 &&
        !gGameState->unk618) {
        PileDriver_SetEffectState((void *)parent, 14);
        PileDriverRig_SetState(&parent->rig, 9);
        PileDriver_NoOpAction((void *)parent);
        PileDriver_CallPendingScript((void *)parent, 8);
    }
    gUnk_08605ABC[parent->effectState]((void *)parent);
    if (parent->initialMode == 0)
        PileDriver_UpdatePaletteState((void *)parent);
    else {
        PileDriver_TickRig((void *)parent);
        PileDriver_SubmitPlayerMidpoint((void *)parent);
        PileDriver_DrawBackgroundLayers((void *)parent);
        PileDriver_LoadModePalette((void *)parent);
        PileDriver_UpdateApproachSprite((void *)parent);
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
    PileDriver_UpdateParticles((void *)parent);
}

s32 PileDriver_DestroyLifecycle(struct PileDriver *parent)
{
    if (parent->initialMode == 0) {
        if (parent->paletteSprite.active)
            Sprite_Remove(&parent->paletteSprite);
    } else {
        PileDriverRig_RemoveSprites(&parent->rig);
        Sprite_Remove(&parent->auxiliary);
        Bg_SetPriority(1, 1);
    }
    PileDriver_DestroyParticles((void *)parent);
    PileDriver_UnregisterReceiver((void *)parent);
    gScenePanel = 0;
    return 0;
}

s32 PileDriver_InitializeLifecycle(struct PileDriver *parent)
{
    s32 found = Script_SeekToKeyword('d');
    if (found)
        parent->skipMainSprite = Script_GetValue();
    else
        parent->skipMainSprite = found;
    {
        struct PileDriverInteractionPlayer *player = SCENE_PANEL_PLAYER;
        parent->savedPlayer = player;
    }
    parent->savedTether = gTetherActor;
    PileDriver_InitAnchorPositions((void *)parent);
    PileDriver_ReadInitialMode((void *)parent);
    if (Script_SeekToKeyword('n'))
        parent->subjectCount = Script_GetValue();
    else
        parent->subjectCount = 4;
    PileDriver_CreateSubjects((void *)parent);
    PileDriver_InitializeInteraction((void *)parent);
    PileDriver_InitParticles((void *)parent);
    PileDriver_InitTimePalette((void *)parent);
    PileDriver_InitScriptProperty((void *)parent);
    PileDriver_RegisterReceiver((void *)parent);
    {
        s32 mode = gGameState->unk876;
        parent->paletteMode = mode;
    }
    gScenePanel = (void *)parent;
    return 0;
}

struct PileDriver *PileDriver_Create(u32 actorId)
{
    struct PileDriver *panel;
    if (gScenePanel)
        return gScenePanel;
    panel = Task_Create(9, 0x17E4);
    if (panel) {
        Task_SetCallbacks(panel, (void (*)(void *))PileDriver_TickLifecycle,
                          (void (*)(void *))PileDriver_DestroyLifecycle);
        panel->actorId = actorId;
        if (PileDriver_InitializeLifecycle((void *)panel) < 0) {
            Task_Kill(panel);
            return NULL;
        }
    }
    return panel;
}
