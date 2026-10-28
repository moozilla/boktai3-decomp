#include "global.h"

/* tentative: dispatches queued scene messages by command type */

struct Cmd {
    u8 pad0[6];
    u8 type;
    u8 pad7;
    s16 x, y; /* 0x8 */
    s16 c, d; /* 0xC */
    u16 e, f; /* 0x10 */
};

struct Queue {
    u8 pad[6], count[2];
    struct Cmd *commands[2][4];
};

struct Motion {
    s32 x, y, z; /* 0x88C */
    u8 pad0C[4];
    u8 b, d, a, c; /* 0x89C */
    u8 pad14[2];
    u16 flags; /* 0x8A2 */
};

struct Att {
    u32 unk0, flags, unk8;
};

struct Vec3s {
    s16 x, y, z;
};

struct Obj {
    u8 pad0[0x10];
    s32 x, y, z;
    u8 pad1C[0x1C];
    s32 h;
    u8 pad3C[0x42];
    u16 unk7E;
    u8 pad80[0x34];
    void *src;
    u8 padB8[4];
    u8 slot;
    u8 padBD[0x5F8 - 0xBD];
    u8 model[0x84C - 0x5F8];
    u32 timer;
    u8 state;
    u8 pad851[3];
    void *fn;
    struct Queue queue;
    u8 pad880[0x88C - 0x880];
    struct Motion motion;
    u8 pad8A4[0x8BC - 0x8A4];
    struct Att att;
    struct Vec3s pos;
    u8 pad8D0[0x8F0 - 0x8D0];
    s16 ofsX, ofsY;
    u8 pad8F4[0x904 - 0x8F4];
    void *attachment;
};

extern struct Camera {
    u8 pad[0x44];
    s32 x;
} *gSceneCamera;
extern struct Globals {
    u8 pad[36], bank;
} *gSceneEventDispatchState;

s32 CharacterModel_GetPrimaryPaletteIndex(u8 *);
void PalFade_Start(struct Obj *, s32, s32, s32, s32, s32, s32, s32);
s8 SpriteSlots_AttachSource(void *);
void Sound_PlaySong(u32);
void TimedSpriteEffect_RequestStop(s32);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, int, int, int, int);
void SceneMessages_SetPendingTrigger(void *, void *);
void SceneActor_UpdateKnockbackFall(void);
void SceneActor_UpdateFallState(void);

static inline u8 IsActive(struct Motion *m)
{
    u16 mask = 1;
    if (m->flags & mask)
        return TRUE;
    return FALSE;
}

static inline void SetState(struct Obj *p, void *fn)
{
    p->state = 1;
    p->fn = fn;
    p->timer = 0;
}

int SceneActor_DispatchCommands(struct Obj *p)
{
    struct Obj *self = p;
    struct Motion *m = &p->motion;
    struct Queue *queue;
    s32 i;
    struct Cmd *cmd;

    if (!IsActive(m))
        return 0;
    p->ofsX = 8;
    p->ofsY = -16;
    {
        struct Att *a = &p->att;
        u32 bit = 4;
        a->flags |= bit;
    }
    {
        struct Vec3s *v = &p->pos;
        v->x = (self->x >> 12) - gSceneCamera->x;
        v->y = ((self->z + (self->y + self->h)) >> 12) + 90;
        v->z = self->z >> 12;
    }
    queue = &p->queue;
    i = 0;
    if (i < queue->count[gSceneEventDispatchState->bank])
        do {
            {
                struct Globals *g = gSceneEventDispatchState;
                cmd = queue->commands[g->bank][i];
            }
            switch (cmd->type) {
                case 0:
                    m->x = cmd->x << 12;
                    m->y = 0;
                    m->z = cmd->y;
                    m->a = cmd->c;
                    m->b = cmd->d;
                    m->c = cmd->e;
                    m->d = cmd->f;
                    SetState(p, SceneActor_UpdateKnockbackFall);
                    break;
                case 1: {
                    s32 x = cmd->x;
                    s32 y = cmd->y;
                    PalFade_Start(self, x, y, CharacterModel_GetPrimaryPaletteIndex(p->model),
                                  self->unk7E, cmd->c, 4, 0);
                    SetState(p, SceneActor_UpdateFallState);
                    break;
                }
                case 2:
                    self->slot = SpriteSlots_AttachSource(self->src);
                    Sound_PlaySong(0x8C);
                    break;
                case 3: {
                    s8 slot = self->slot;
                    if (slot >= 0)
                        TimedSpriteEffect_RequestStop(slot);
                } break;
                case 4:
                    if (cmd->x == 0) {
                        p->attachment = ActorAttachment_UpdateFromDescriptor(p->attachment, &p->att,
                                                                             0, 1, 0, 0, 0, -20);
                        Sound_PlaySong(0x1B1);
                    }
                    break;
            }
            SceneMessages_SetPendingTrigger(queue, cmd);
        } while (++i < queue->count[gSceneEventDispatchState->bank]);
}
