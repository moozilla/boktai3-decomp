#include "scene_presentation_commands.h"

void SceneCommands_ClearEffect(struct ScenePresentation *presentation, void *unused,
                               struct ScenePresentationMessage *message)
{
    if (presentation->effect != 0) {
        OrbitingParticles_Stop(presentation->effect);
        presentation->effect = 0;
    }
}

void SceneCommands_CreateEffect(struct ScenePresentation *presentation, void *unused,
                                struct ScenePresentationMessage *message)
{
    struct SceneVec position;
    s32 radius, duration, count, type;
    position.x = message->arguments[0];
    position.y = message->arguments[1];
    position.z = message->arguments[2];
    radius = message->arguments[3];
    duration = message->arguments[4];
    count = message->arguments[5];
    type = message->arguments[6];
    if (presentation->effect) {
        OrbitingParticles_Stop(presentation->effect);
        presentation->effect = 0;
    }
    presentation->effect = OrbitingParticles_Create(&position, radius, duration, count, type);
}

void SceneCommands_CreateOrbitingParticles(struct ScenePresentation *presentation, void *unused,
                                           struct ScenePresentationMessage *message)
{
    struct ScenePresentationPosition position;
    struct SceneBody *body = SceneBody_GetById(message->arguments[0]);
    if (body != 0) {
        position.x = (u16)body->position.x;
        position.y = (u16)body->position.y;
        position.z = (u16)body->position.z;
        OrbitingParticles_Create((void *)&position, message->arguments[1], message->arguments[2],
                                 message->arguments[3], message->arguments[4]);
    }
}

void SceneCommands_Fade(struct ScenePresentation *presentation, void *unused,
                        struct ScenePresentationMessage *message)
{
    s16 *args = message->arguments;
    u16 mask = 0;
    s32 mode;

    if (args[5])
        mask = 0xFFF;
    if (args[6])
        mask |= 0x1000;
    if (args[7])
        mask |= 0xE000;
    {
        s32 enabled = message->arguments[0];
        mode = 3;
        if (!enabled)
            mode = 2;
    }
    ScreenTransition_SendCommand(mode, args[1], args[2], args[3], args[4], mask, args[8]);
}

static inline u32 SceneCommands_PaletteBankMask(const s16 *args)
{
    u32 mask = 0;
    if (args[5])
        mask = 0xFFF;
    if (args[6])
        mask = (u16)mask | 0x1000;
    if (args[7])
        mask = (u16)mask | 0xE000;
    return mask;
}

void SceneCommands_FadePalette(struct ScenePresentation *presentation, void *unused,
                               struct ScenePresentationMessage *message)
{
    s16 *args = message->arguments;
    int out[7], mode;
    u32 mask = 0;
    mask = SceneCommands_PaletteBankMask(args);
    out[0] = 1;
    out[1] = args[1];
    out[2] = args[2];
    out[3] = args[3];
    out[4] = args[4];
    out[5] = (u16)mask;
    out[6] = 0;
    if (!out[2] && !out[3] && !out[4]) {
        out[2] = 4;
        out[3] = 4;
        out[4] = 4;
    }
    {
        int enabled = args[0];
        mode = 2;
        if (!enabled)
            mode = 1;
    }
    ColorFade_QueueCommand(mode, 7, out);
}

void SceneCommands_BlendPalette(struct ScenePresentation *presentation, void *unused,
                                struct ScenePresentationMessage *message)
{
    s16 *args = message->arguments;
    int out[7], mode;
    out[0] = 2;
    out[1] = args[1];
    out[2] = args[2];
    out[3] = args[3];
    out[4] = args[4];
    out[5] = 0;
    out[6] = message->arguments[5];
    if (message->arguments[6] == 1)
        gObjPaletteEffect = 1;
    if (!out[2] && !out[3] && !out[4]) {
        out[2] = 4;
        out[3] = 4;
        out[4] = 4;
    }
    {
        int enabled = args[0];
        mode = 2;
        if (!enabled)
            mode = 1;
    }
    ColorFade_QueueCommand(mode, 7, out);
}

void SceneCommands_SetShake(struct ScenePresentation *presentation, void *unused,
                            struct ScenePresentationMessage *message)
{
    presentation->shakeFrames = message->arguments[0];
    presentation->shakeStrength = message->arguments[1];
}

void SceneCommands_StopShake(struct ScenePresentation *presentation, void *unused,
                             struct ScenePresentationMessage *message)
{
    presentation->shakeFrames = 0;
    presentation->shakeStrength = 0;
}

void SceneCommands_ConfigureBlendWindow(struct ScenePresentation *presentation, void *unused,
                                        struct ScenePresentationMessage *message)
{
    s32 color;
    s32 mask;
    s32 bright = message->arguments[2];
    color = 0x7FFF;
    if (bright == 0)
        color = 0x1084;
    switch (message->arguments[1]) {
        case 0:
            mask = 0xFFF;
            break;
        case 1:
            mask = 0x1000;
            break;
        case 2:
            mask = 0xE000;
            break;
        case 3:
            mask = 0x1FFF;
            break;
        case 4:
        default:
            mask = 0xFFFF;
            break;
    }
    ColorFade_SetImmediate(message->arguments[0], mask, color);
}

s32 SceneCommands_Dispatch(struct ScenePresentation *presentation)
{
    struct SceneEventReceiver *entry = &presentation->receiver.messages;
    s32 i;

    i = 0;
    if (i < entry->count[gSceneEventDispatchState->phase])
        do {
            struct ScenePresentationMessage *message;
            struct SceneEventDispatchState *controller = gSceneEventDispatchState;
            message = (void *)entry->pending[controller->phase][i];
            SceneMessages_SetPendingTrigger(entry, (void *)message);
            switch (message->kind) {
                case 0:
                    SceneCommands_CreateEffect(presentation, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 1:
                    SceneCommands_ClearEffect(presentation, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 2:
                    SceneCommands_CreateOrbitingParticles(presentation, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 3:
                    SceneCommands_Fade(presentation, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 4:
                    SceneCommands_FadePalette(presentation, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 5:
                    SceneCommands_BlendPalette(presentation, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 6:
                    SceneCommands_SetShake(presentation, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 7:
                    SceneCommands_StopShake(presentation, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 8:
                    SceneCommands_ConfigureBlendWindow(presentation, entry, message);
                    Trigger_ConsumePending(entry, 1);
                    break;
                case 9:
                    gObjPaletteEffect = 0;
                    Trigger_ConsumePending(entry, 1);
                    break;
            }
            i++;
        } while (i < entry->count[gSceneEventDispatchState->phase]);
    return 0;
}

u32 SceneCommands_TaskUpdate(struct ScenePresentation *presentation)
{
    SceneCommands_Dispatch(presentation);
    if (presentation->shakeFrames != 0) {
        if (presentation->shakeFrames > 0)
            presentation->shakeFrames--;
        Camera_RequestShake(presentation->shakeStrength);
    }
    return 0;
}

u32 SceneCommands_TaskDestroy(struct ScenePresentation *presentation)
{
    SceneMessages_UnregisterReceiver(&presentation->receiver);
    return 0;
}

s32 SceneCommands_TaskInit(struct ScenePresentation *presentation, u32 receiverId)
{
    SceneMessages_RegisterReceiver(&presentation->receiver, receiverId, 7);
    return (u32)(presentation->effect = 0);
}

struct ScenePresentation *SceneCommands_StartTask(u32 receiverId)
{
    struct ScenePresentation *presentation = Task_Create(3, sizeof(*presentation));
    if (presentation != 0) {
        Task_SetCallbacks(presentation, (void *)SceneCommands_TaskUpdate,
                          (void *)SceneCommands_TaskDestroy);
        if (SceneCommands_TaskInit(presentation, receiverId) < 0) {
            Task_Kill(presentation);
            return 0;
        }
    }
    return presentation;
}
