#include "bosses/ratatosk/ratatosk.h"
#include "engine/video/render.h"
#include "scene/actor/scene_event_dispatch.h"

u32 Trigger_ConsumePending(void *, s32);
u32 ActorResource_GetFixedId(void *);
void CpuSet(const void *, void *, u32);
void PaletteEffect_RefreshPalette(void *);
void ScaledShadow_Enable(void *);
void Ratatosk_PlayAttachmentEffect(void *, s32, s32, s32, s32, s32);
void Ratatosk_FireTrackingShot(void *, void *, s32);
void SceneActors_FireTrackingProjectileSpread(void *, void *, s32);

/* A scene event command with its s16 arguments (SceneEventCommand::value onwards). */
struct RatatoskCommand {
    u16 id;
    u8 receiverKind, trigger;
    u16 unknown4;
    u8 kind, argumentCount;
    s16 args[3];
};
struct Anim {
    u32 pad[2];
    u32 flags;
};
static inline void clearAnim(struct Anim *a)
{
    a->flags &= ~1;
}
static inline void setupPalette(struct Ratatosk *p, u8 mode, u32 bank, u8 shift)
{
    p->blendBank = bank;
    p->blendMode = mode;
    p->blendShift = shift;
    if (p->blendMode == 2) {
        u32 value = 32;
        p->blendStep = value;
    } else if (p->blendMode == 1)
        p->blendStep = 0;
}
static inline void Palette_SelectResource(struct Ratatosk *p, u32 id)
{
    const u32 originalId = id;
    if (originalId == 0)
        p->palette = p->paletteAlternate;
    else if (originalId == 501)
        p->palette = p->paletteAlternate2;
    else {
        CpuSet(gPaletteBankData + (id *= 32), p->paletteBase, 0x04000008);
        p->palette = p->paletteBase;
    }
}
// confidence=low: dispatches queued scene-message commands to Ratatosk states and effects.
void Ratatosk_DispatchSceneCommands(struct Ratatosk *p)
{
    struct Ratatosk *actor = p;
    struct SceneEventReceiver *receiver = (struct SceneEventReceiver *)p->receiver2;
    struct ContactPosition position;
    s32 i;
    i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase])
        do {
            struct SceneEventDispatchState *dispatch = gSceneEventDispatchState;
            struct RatatoskCommand *command = (void *)receiver->pending[dispatch->phase][i];
            SceneMessages_SetPendingTrigger(receiver, (struct SceneEventCommand *)command);
            switch (command->kind) {
                case 0:
                    Ratatosk_StartAction(actor, 2, 0, 0);
                    break;
                case 1:
                    actor->savedAngle = ((u8)command->args[0] + 5) << 5;
                    Ratatosk_StartAction(actor, 2, 0, 1);
                    break;
                case 3:
                    actor->savedAngle = ((u8)command->args[0] + 5) << 5;
                    Ratatosk_StartAction(actor, 2, 0, 2);
                    break;
                case 4:
                    actor->savedAngle = ((u8)command->args[0] + 5) << 5;
                    Ratatosk_StartAction(actor, 2, 0, 3);
                    break;
                case 10:
                    Ratatosk_StartAction(actor, 2, 0, 5);
                    break;
                case 11:
                    Ratatosk_StartAction(actor, 2, 0, 6);
                    break;
                case 12:
                    p->destination.x = command->args[0];
                    p->destination.y = command->args[1];
                    p->destination.z = command->args[2];
                    Ratatosk_StartAction(actor, 2, 0, 4);
                    break;
                case 19:
                    Ratatosk_StartAction(actor, 2, 0, 7);
                    break;
                case 37:
                    p->paletteToggle = 0;
                    Ratatosk_StartAction(actor, 2, 0, 9);
                    break;
                case 38:
                    Ratatosk_StartAction(actor, 2, 0, 8);
                    break;
                case 39:
                    p->paletteToggle = 1;
                    Ratatosk_StartAction(actor, 2, 0, 9);
                    break;
                case 40:
                    Ratatosk_StartAction(actor, 2, 0, 10);
                    break;
                case 41:
                    p->stateParameter = command->args[0];
                    Ratatosk_StartAction(actor, 2, 0, 11);
                    break;
                case 42:
                    Ratatosk_StartAction(actor, 2, 0, 12);
                    break;
                case 43:
                    if (command->args[0] == 0)
                        Ratatosk_StartAction(actor, 2, 0, 13);
                    else
                        Ratatosk_StartAction(actor, 2, 0, 14);
                    break;
                case 15: {
                    s32 paletteValue;
                    if (command->args[0] == 0) {
                        u32 id;
                        p->blendStep = 0;
                        p->blendMode = 0;
                        p->blendBank = ActorResource_GetFixedId(p);
                        id = ActorResource_GetFixedId(p);
                        Palette_SelectResource(p, id);
                    } else {
                        paletteValue = command->args[1];
                        if (paletteValue == 0)
                            setupPalette(p, 3, 501, 5);
                        else
                            setupPalette(p, 3, 500, 5);
                    }
                    Trigger_ConsumePending(actor->receiver2, 1);
                    break;
                }
                case 16: {
                    s32 paletteValue = command->args[0];
                    if (paletteValue == 0)
                        setupPalette(p, 1, 498, 5);
                    else {
                        paletteValue = command->args[1];
                        if (paletteValue == 0)
                            setupPalette(p, 2, 501, 5);
                        else
                            setupPalette(p, 2, 500, 5);
                    }
                    Trigger_ConsumePending(actor->receiver2, 1);
                    break;
                }
                case 9:
                    Ratatosk_PlayAttachmentEffect(actor, command->args[0], 180, 180, 250, 100);
                    Trigger_ConsumePending(actor->receiver2, 1);
                    break;
                case 2:
                    actor->body.position.x = command->args[0];
                    actor->body.position.y = command->args[1];
                    actor->body.position.z = command->args[2];
                    p->anchor.x = actor->body.position.x;
                    p->anchor.y = actor->body.position.y + p->distance;
                    p->anchor.z = actor->body.position.z;
                    Trigger_ConsumePending(actor->receiver2, 1);
                    break;
                case 5:
                    actor->savedAngle = ((u8)command->args[0] + 5) << 5;
                    Ratatosk_StartAction(actor, 2, 0, 22);
                    break;
                case 7:
                    Ratatosk_StartAction(actor, 2, 0, 16);
                    break;
                case 8:
                    Ratatosk_StartAction(actor, 2, 0, 17);
                    break;
                case 44:
                    Ratatosk_StartAction(actor, 2, 0, 18);
                    break;
                case 45:
                    Ratatosk_StartAction(actor, 2, 0, 20);
                    break;
                case 46:
                    Ratatosk_StartAction(actor, 2, 0, 19);
                    break;
                case 47:
                    position.x = actor->body.position.x;
                    position.y = actor->body.position.y;
                    position.z = actor->body.position.z;
                    Ratatosk_FireTrackingShot(p, &position, command->args[0]);
                    Trigger_ConsumePending(actor->receiver2, 1);
                    break;
                case 48:
                    position.x = actor->body.position.x;
                    position.y = actor->body.position.y;
                    position.z = actor->body.position.z;
                    SceneActors_FireTrackingProjectileSpread(p, &position, command->args[0]);
                    Trigger_ConsumePending(actor->receiver2, 1);
                    break;
                case 49:
                    p->shotTarget.x = command->args[0];
                    p->shotTarget.y = command->args[1];
                    p->shotTarget.z = command->args[2];
                    Trigger_ConsumePending(actor->receiver2, 1);
                    break;
                case 50:
                    Ratatosk_StartAction(actor, 2, 0, 21);
                    break;
            }
            PaletteEffect_RefreshPalette(p);
            clearAnim((struct Anim *)p->animationData);
            ScaledShadow_Enable(p->shadow);
        } while (++i < receiver->count[gSceneEventDispatchState->phase]);
}
