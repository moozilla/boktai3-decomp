#include "global.h"
#include "monsters/glove/glove.h"
extern struct GloveCamera {
    u8 unknown0[0x44];
    s32 x;
} *gSceneCamera;
extern struct GloveGlobals {
    u8 unknown0[0x24], bank;
} *gSceneEventDispatchState;
void Glove_UpdateRecoil(void *);
void Glove_UpdateImpactRecoil(void *);
void Glove_UpdateStaggerRecoil(void *);
void Glove_UpdateEmitterRecoil(void *);
void Glove_UpdateRecoilReturn(void *);
void Glove_UpdateStaggerReturn(void *);
void Sound_PlaySong(u32);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, s32, s32, s32, s32, s32, s32);
void *ActorAttachment_ReleaseResource(void *, s32, u32);
void *ActorAttachment_ReleasePartsResource(void *, s32, u32);
void SceneMessages_SetPendingTrigger(void *, void *);
static inline u8 IsActive(struct GloveDraw *motion)
{
    u16 bit = 1;
    if (motion->flags & bit)
        return 1;
    return 0;
}
static inline void SetState(struct Glove *p, void (*callback)(void *))
{
    p->active = 1;
    p->update = callback;
    p->timer = 0;
}
static inline void ReadMotion(struct GloveDraw *motion, struct GloveCommand *cmd)
{
    motion->x = cmd->x << 12;
    motion->y = 0;
    motion->z = cmd->y;
    motion->a = cmd->c;
    motion->b = cmd->d;
    motion->c = cmd->e;
    motion->d = cmd->f;
}
// confidence=low: message kinds select glove recoil states and attachment effects.
s32 Glove_DispatchCommands(struct Glove *p)
{
    struct GloveMotion *state = &p->motion;
    struct GloveDraw *motion = &p->draw;
    struct GloveQueue *queue;
    s32 i;
    struct GloveCommand *cmd;
    if (!IsActive(motion))
        return 0;
    p->offsetX = 4;
    p->offsetY = -24;
    {
        struct GloveAttachment *desc = &p->attachmentDesc;
        desc->flags |= 4;
    }
    {
        struct GlovePosition *pos = &p->screenPosition;
        pos->x = (state->position.x >> 12) - gSceneCamera->x;
        pos->y = ((state->position.z + (state->position.y + state->unk38)) >> 12) + 90;
        pos->z = state->position.z >> 12;
    }
    queue = &p->queue;
    i = 0;
    if (i < queue->count[gSceneEventDispatchState->bank])
        do {
            {
                struct GloveGlobals *g = gSceneEventDispatchState;
                cmd = queue->commands[g->bank][i];
            }
            switch (cmd->type) {
                case 2:
                    ReadMotion(motion, cmd);
                    SetState(p, Glove_UpdateRecoilReturn);
                    break;
                case 6:
                    ReadMotion(motion, cmd);
                    SetState(p, Glove_UpdateStaggerReturn);
                    break;
                case 5:
                    SetState(p, Glove_UpdateRecoil);
                    break;
                case 0:
                    SetState(p, Glove_UpdateImpactRecoil);
                    break;
                case 1:
                    SetState(p, Glove_UpdateStaggerRecoil);
                    break;
                case 3:
                    state->emitter.kind = cmd->x;
                    state->emitter.type = cmd->y;
                    state->emitter.enter = 1;
                    SetState(p, Glove_UpdateEmitterRecoil);
                    break;
                case 4:
                    switch ((u32)cmd->x) {
                        case 2:
                            p->attachment = ActorAttachment_UpdateFromDescriptor(
                                p->attachment, &p->attachmentDesc, 0, 0, 0, 0, 0, -20);
                            Sound_PlaySong(0x1EC);
                            break;
                        case 1:
                            p->attachment = ActorAttachment_UpdateFromDescriptor(
                                p->attachment, &p->attachmentDesc, 0, 3, 0, 0, 0, -20);
                            Sound_PlaySong(0x1EC);
                            break;
                        case 0:
                            p->attachment = ActorAttachment_UpdateFromDescriptor(
                                p->attachment, &p->attachmentDesc, 0, 1, 0, 0, 0, -20);
                            Sound_PlaySong(0x1B1);
                            break;
                        case 3:
                            p->attachment = ActorAttachment_UpdateFromDescriptor(
                                p->attachment, &p->attachmentDesc, 0, 2, 0, 0, 0, -20);
                            Sound_PlaySong(0x1ED);
                            break;
                        case 8:
                            p->attachment = ActorAttachment_ReleaseResource(
                                p->attachment, 0, p->attachmentDesc.resource);
                            p->attachment = ActorAttachment_ReleaseResource(
                                p->attachment, 1, p->attachmentDesc.resource);
                            p->attachment = ActorAttachment_ReleaseResource(
                                p->attachment, 2, p->attachmentDesc.resource);
                            p->attachment = ActorAttachment_ReleaseResource(
                                p->attachment, 4, p->attachmentDesc.resource);
                            p->attachment = ActorAttachment_ReleaseResource(
                                p->attachment, 5, p->attachmentDesc.resource);
                            p->attachment = ActorAttachment_ReleasePartsResource(
                                p->attachment, 3, p->attachmentDesc.resource);
                            break;
                    }
                    break;
            }
            SceneMessages_SetPendingTrigger(queue, cmd);
        } while (++i < queue->count[gSceneEventDispatchState->bank]);
}
