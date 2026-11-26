#include "npc/sabata_rider/script_puppet.h"
#include "scene/actor/scene_event_dispatch.h"
#include "effects/particles/scene_debris_types.h"
#include "effects/screen/background_controller.h"

/* tentative: dispatches queued scene messages by command type */

struct RegistryActorCommand {
    u8 pad0[6];
    u8 type;
    u8 pad7;
    s16 x, y; /* 0x8 */
    s16 c, d; /* 0xC */
    u16 e, f; /* 0x10 */
};

struct RegistryActorAttachment {
    void *owner;
    u32 flags, parameter;
};

s32 CharacterModel_GetPrimaryPaletteIndex(struct CharacterPreviewModel *);
void PalFade_Start(struct RegistryActor *, s32, s32, s32, s32, s32, s32, s32);
s8 SpriteSlots_AttachSource(void *);
void Sound_PlaySong(u32);
void TimedSpriteEffect_RequestStop(s32);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, int, int, int, int);
void SceneActor_UpdateKnockbackFall(struct RegistryActor *);
void SceneActor_UpdateFallState(struct RegistryActor *);

static inline u8 RegistryActor_TestMotionFlags(u16 mask, struct RegistryActorMotion *m)
{
    if (m->flags & mask)
        return TRUE;
    return FALSE;
}

static inline void SetAttachmentFlags(struct RegistryActorAttachment *attachment, u32 bits)
{
    attachment->flags |= bits;
}

s32 SceneActor_DispatchCommands(struct RegistryActor *p)
{
    struct RegistryActor *self = p;
    struct RegistryActorMotion *m = &p->control.motion;
    struct SceneEventReceiver *queue;
    s32 i;
    struct RegistryActorCommand *cmd;

    if (!RegistryActor_TestMotionFlags(REGISTRY_ACTOR_MOTION_COMMANDS, m))
        return 0;
    p->control.attachmentOffsetX = 8;
    p->control.attachmentOffsetY = -16;
    SetAttachmentFlags((struct RegistryActorAttachment *)&p->control.owner, 4);
    {
        struct ContactPosition *v = (void *)&p->control.attachmentScreenX;
        v->x = (self->stats.position.x >> 12) - gSceneCamera->x;
        v->y =
            ((self->stats.position.z + (self->stats.position.y + self->stats.height)) >> 12) + 90;
        v->z = self->stats.position.z >> 12;
    }
    queue = (void *)p->control.messages;
    i = 0;
    if (i < queue->count[gSceneEventDispatchState->phase])
        do {
            {
                struct SceneEventDispatchState *g = gSceneEventDispatchState;
                cmd = (void *)queue->pending[g->phase][i];
            }
            switch (cmd->type) {
                case 0:
                    m->x = cmd->x << 12;
                    m->y = 0;
                    m->z = cmd->y;
                    ((u8 *)&m->remaining)[0] = cmd->c;
                    m->parameterB = cmd->d;
                    ((u8 *)&m->remaining)[1] = cmd->e;
                    m->parameterD = cmd->f;
                    RegistryActor_StartMotion(p, SceneActor_UpdateKnockbackFall);
                    break;
                case 1: {
                    s32 x = cmd->x;
                    s32 y = cmd->y;
                    PalFade_Start(self, x, y, CharacterModel_GetPrimaryPaletteIndex(&p->model),
                                  self->stats.recoveryValue, cmd->c, 4, 0);
                    RegistryActor_StartMotion(p, SceneActor_UpdateFallState);
                    break;
                }
                case 2:
                    self->stats.drain.unknownC =
                        SpriteSlots_AttachSource(self->stats.drain.displayPosition);
                    Sound_PlaySong(0x8C);
                    break;
                case 3: {
                    s8 slot = self->stats.drain.unknownC;
                    if (slot >= 0)
                        TimedSpriteEffect_RequestStop(slot);
                } break;
                case 4:
                    if (cmd->x == 0) {
                        p->control.attachment = ActorAttachment_UpdateFromDescriptor(
                            p->control.attachment,
                            (struct RegistryActorAttachment *)&p->control.owner, 0, 1, 0, 0, 0,
                            -20);
                        Sound_PlaySong(0x1B1);
                    }
                    break;
            }
            SceneMessages_SetPendingTrigger(queue, (struct SceneEventCommand *)cmd);
        } while (++i < queue->count[gSceneEventDispatchState->phase]);
}
