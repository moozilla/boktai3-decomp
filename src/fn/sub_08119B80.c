#include "global.h"
struct GloveCommand {
    u8 unknown0[6], type, unknown7;
    s16 x, y;
    u16 c, d, e, f;
};
struct GloveQueue {
    u8 unknown0[6], count[2];
    struct GloveCommand *commands[2][4];
};
struct GloveMotion {
    s32 x, y, z;
    u8 unknownC[4];
    u8 b, d, a, c;
    u8 unknown14[2];
    u16 flags;
};
struct GloveState {
    u8 unknown0[0x10];
    s32 x, y, z;
    u8 unknown1C[0x1C];
    s32 height;
    u8 unknown3C[8], emitter;
    u8 unknown45, emitType, emitEnter;
};
struct GloveAttachment {
    u32 resource, flags, unknown8;
};
struct GlovePosition {
    s16 x, y, z;
};
struct Glove {
    u8 unknown0[0x18];
    struct GloveState state;
    u8 unknown60[0x804];
    u32 timer;
    u8 enter;
    u8 unknown869[3];
    void (*update)(void *);
    struct GloveQueue queue;
    u8 unknown898[0x30];
    struct GloveMotion motion;
    u8 unknown8E0[0x3C];
    struct GloveAttachment attachmentDesc;
    struct GlovePosition screenPosition;
    u8 unknown930[0x20];
    s16 offsetX, offsetY;
    u8 unknown954[0x10];
    void *attachment;
};
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
static inline u8 IsActive(struct GloveMotion *motion)
{
    u16 bit = 1;
    if (motion->flags & bit)
        return 1;
    return 0;
}
static inline void SetState(struct Glove *p, void (*callback)(void *))
{
    p->enter = 1;
    p->update = callback;
    p->timer = 0;
}
static inline void ReadMotion(struct GloveMotion *motion, struct GloveCommand *cmd)
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
    struct GloveState *state = &p->state;
    struct GloveMotion *motion = &p->motion;
    struct GloveQueue *queue;
    s32 i;
    struct GloveCommand *cmd;
    if (!IsActive(motion))
        return 0;
    p->offsetX = 4;
    p->offsetY = -24;
    {
        struct GloveAttachment *desc = &p->attachmentDesc;
        u32 bit = 4;
        desc->flags |= bit;
    }
    {
        struct GlovePosition *pos = &p->screenPosition;
        pos->x = (state->x >> 12) - gSceneCamera->x;
        pos->y = ((state->z + (state->y + state->height)) >> 12) + 90;
        pos->z = state->z >> 12;
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
                    state->emitter = cmd->x;
                    state->emitType = cmd->y;
                    state->emitEnter = 1;
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
