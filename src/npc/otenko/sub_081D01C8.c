#include "npc/otenko/otenko_actor.h"
#include "engine/video/render.h"

extern u8 *gSceneEventDispatchState;

void SceneMessages_SetPendingTrigger(void *, void *);
s32 Math_Atan2(s32, s32);
void Otenko_PlayAttachmentEffect(struct OtenkoActor *, s32);
void Actor_SetAttachmentEffect(struct OtenkoActor *, s32, s32, s32);
void sub_081D010C(struct OtenkoActor *, s16 *);
void CpuSet(const void *, void *, u32);

static inline void SetNpcState(struct OtenkoActor *npc, u32 sub)
{
    npc->stage = 0;
    npc->stageEntered = 0;
    npc->state = sub;
    npc->timer = 0;
}

static inline void SetNpcFacing(struct OtenkoActor *npc, u32 a)
{
    npc->body.direction = a;
    {
        s32 shifted = (s32)(a + 0x70) >> 5;
        u32 mask = 7;
        mask &= shifted;
        npc->facing8 = mask;
    }
    {
        s32 shifted = (s32)(a + 0x60) >> 6;
        u32 mask = 3;
        mask &= shifted;
        npc->facing4 = mask;
    }
}
static inline void ApplyCommandFacing(struct OtenkoActor *npc, u32 a)
{
    npc->body.direction = a;
    a = (u8)a;
    {
        s32 shifted = (s32)(a + 0x70) >> 5;
        u32 mask = 7;
        mask &= shifted;
        npc->facing8 = mask;
    }
    {
        s32 shifted = (s32)(a + 0x60) >> 6;
        u32 mask = 3;
        mask &= shifted;
        npc->facing4 = mask;
    }
}
static inline u16 FacingBits(s8 mask, u32 shifted)
{
    mask &= shifted;
    return mask;
}
static inline void SetFlaggedFacing(struct OtenkoActor *npc, u32 a, u8 directionMask)
{
    npc->body.direction = a;
    {
        s32 shifted = (s32)(a + 0x70) >> 5;
        u32 mask = 7;
        mask &= shifted;
        npc->facing8 = mask;
    }
    {
        npc->facing4 = FacingBits(directionMask, (s32)(a + 0x60) >> 6);
    }
}
struct CmdFlags {
    u8 pad[4];
    u16 flags;
};
static inline u16 MessageBits(struct CmdFlags *msg, u32 bits)
{
    return msg->flags & bits;
}
static inline u16 MaskedMessageFlags(u16 value, s8 mask)
{
    return (((u32)value << 16) & ((u32)mask << 16)) >> 16;
}
static inline void BindWorkPalette(struct SceneAnimation *graphics, u16 *palette)
{
    graphics->palette = palette;
}

static inline void AimAtCommandTarget(struct OtenkoActor *npc, s16 *m)
{
    struct ContactPosition *t, *p;
    npc->target.x = m[4];
    npc->target.y = m[5];
    npc->target.z = m[6];
    t = &npc->target;
    p = &npc->body.position;
    npc->targetAngle = Math_Atan2(t->x - p->x, t->z - p->z);
}

static inline void SetNpcSpeed(struct OtenkoActor *npc, s32 speed)
{
    npc->speed = speed;
    npc->speedSquared = speed * speed;
}

/* Dispatch the active message bank and acknowledge each command before handling it. */
void Otenko_DispatchCommands(struct OtenkoActor *npc)
{
    struct OtenkoEffectState *effect;
    struct SceneEventReceiver *receiver;
    u32 modeBit = 2;
    u32 copyWords = 0x04000008;
    s32 i;
    receiver = &npc->receiver;
    effect = &npc->effect;
    i = 0;
    if (i < receiver->count[gSceneEventDispatchState[0x24]])
        do {
            s16 *m;
            u8 *controller = gSceneEventDispatchState;
            m = (s16 *)receiver->pending[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(receiver, m);
            switch (((u8 *)m)[6]) {
                case 0:
                    if (effect->attached) {
                        SetNpcState(npc, 0x22);
                        npc->commandEntered = 1;
                    } else {
                        SetNpcState(npc, 0);
                        npc->commandEntered = 1;
                    }
                    break;

                case 1:
                    ApplyCommandFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    if (effect->attached) {
                        SetNpcState(npc, 0x23);
                        npc->commandEntered = 1;
                    } else {
                        SetNpcState(npc, 1);
                        npc->commandEntered = 1;
                    }
                    break;

                case 36:
                    AimAtCommandTarget(npc, m);
                    SetNpcSpeed(npc, 10);
                    SetNpcState(npc, 2);
                    npc->commandEntered = 1;
                    break;

                case 4:
                    AimAtCommandTarget(npc, m);
                    SetNpcSpeed(npc, 10);
                    SetNpcState(npc, 0x15);
                    npc->commandEntered = 1;
                    break;

                case 5:
                    AimAtCommandTarget(npc, m);
                    SetNpcSpeed(npc, 20);
                    SetNpcState(npc, 0x15);
                    npc->commandEntered = 1;
                    break;

                case 3:
                    npc->targetAngle = (((u8 *)m)[8] + 5) << 5;
                    SetNpcState(npc, 3);
                    npc->commandEntered = 1;
                    break;

                case 6:
                    SetNpcState(npc, 4);
                    npc->commandEntered = 1;
                    break;

                case 7:
                    SetNpcState(npc, 5);
                    npc->commandEntered = 1;
                    break;

                case 8: {
                    npc->targetAngle = (((u8 *)m)[8] + 5) << 5;
                    npc->commandFlags |= MaskedMessageFlags(((struct CmdFlags *)m)->flags, 3);
                    if (((u16 *)m)[2] & 1)
                        npc->commandFlags |= 1;
                    SetFlaggedFacing(npc, npc->targetAngle, 3);
                    SetNpcState(npc, 0x10);
                    npc->commandEntered = 1;
                    break;
                }
                case 9: {
                    u16 bits = MessageBits((void *)m, modeBit);
                    bits |= npc->commandFlags;
                    npc->commandFlags = bits;
                }
                    SetNpcState(npc, 0x11);
                    npc->commandEntered = 1;
                    break;

                case 10:
                    SetNpcState(npc, 0x13);
                    npc->commandEntered = 1;
                    break;

                case 25:
                    if (effect->attached)
                        Otenko_PlayAttachmentEffect(npc, m[4]);
                    else
                        Actor_SetAttachmentEffect(npc, m[4], 0xB4, 0xB4);
                    break;

                case 11:
                    SetNpcState(npc, 0x14);
                    npc->commandEntered = 1;
                    break;

                case 12:
                    SetNpcState(npc, 0x1E);
                    npc->commandEntered = 1;
                    break;

                case 65:
                    SetNpcState(npc, 0x1F);
                    npc->commandEntered = 1;
                    break;

                case 66:
                    SetNpcState(npc, 0x20);
                    npc->commandEntered = 1;
                    break;

                case 67:
                    SetNpcState(npc, 0x21);
                    npc->commandEntered = 1;
                    break;

                case 13:
                    npc->commandFlags |= 4;
                    CpuSet(gPaletteBankData + 0x2B00, npc->workPalette, copyWords);
                    BindWorkPalette(&npc->display.resource.sequence.graphics, npc->workPalette);
                    break;

                case 14: {
                    u32 mask = ~4;
                    npc->commandFlags &= mask;
                }
                    CpuSet(gPaletteBankData + 0x2B60, npc->workPalette, copyWords);
                    BindWorkPalette(&npc->display.resource.sequence.graphics, npc->workPalette);
                    break;

                case 16: {
                    u32 mask = ~4;
                    npc->commandFlags &= mask;
                }
                    CpuSet(gPaletteBankData + 0x2AE0, npc->workPalette, copyWords);
                    BindWorkPalette(&npc->display.resource.sequence.graphics, npc->workPalette);
                    break;

                case 15:
                    npc->commandFlags |= 4;
                    npc->effect.paletteFrame = 0;
                    CpuSet(gPaletteBankData + 0x2AE0, npc->workPalette, copyWords);
                    BindWorkPalette(&npc->display.resource.sequence.graphics, npc->workPalette);
                    break;

                case 64:
                    npc->body.position.x = m[4];
                    npc->body.position.y = m[5];
                    npc->body.position.z = m[6];
                    break;

                case 17:
                    npc->targetAngle = (((u8 *)m)[8] + 5) << 5;
                    {
                        u32 angle = npc->targetAngle + 128;
                        angle &= 255;
                        SetNpcFacing(npc, angle);
                    }
                    SetNpcSpeed(npc, m[5]);
                    SetNpcState(npc, 0x17);
                    npc->commandEntered = 1;
                    break;

                case 18:
                    npc->targetAngle = (((u8 *)m)[8] + 5) << 5;
                    {
                        u32 angle = npc->targetAngle + 128;
                        angle &= 255;
                        SetNpcFacing(npc, angle);
                    }
                    SetNpcSpeed(npc, m[5]);
                    SetNpcState(npc, 0x18);
                    npc->commandEntered = 1;
                    break;

                case 19:
                    ApplyCommandFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    SetNpcState(npc, 0x19);
                    npc->commandEntered = 1;
                    break;

                case 20:
                    SetNpcState(npc, 0x1A);
                    npc->commandEntered = 1;
                    break;

                case 21:
                    SetNpcState(npc, 0x1B);
                    npc->commandEntered = 1;
                    break;

                case 22:
                    SetNpcState(npc, 0x1C);
                    npc->commandEntered = 1;
                    break;

                case 23:
                    ApplyCommandFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    SetNpcState(npc, 0x1D);
                    npc->commandEntered = 1;
                    break;

                case 24:
                    sub_081D010C(npc, m);
                    break;
            }

            i++;
        } while (i < receiver->count[gSceneEventDispatchState[0x24]]);
}
