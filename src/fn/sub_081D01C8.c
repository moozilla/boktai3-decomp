#include "global.h"
#include "render.h"
#ifdef NONMATCHING
// fuzzy: 98.3%
typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    s16 *messages[2][4];
    u8 pad[8];
    void *next;
} NpcReceiver;

struct NpcPosition {
    s16 x, y, z, w;
};

struct NpcAuxState {
    u8 pad0[0xD];
    u8 flag;
    u16 h2E;
};

struct SceneNpc {
    u8 pad0[2];
    u8 state, entered, sub;
    u8 pad5[3];
    u8 changed;
    u8 pad9[3];
    u16 flags;
    u8 padE[0x14 - 0xE];
    u32 timer;
    u8 pad18;
    u8 facing4, facing8, angle;
    u8 pad1C[0x20 - 0x1C];
    struct NpcAuxState sub20;
    u8 pad30[0x54 - 0x30];
    struct NpcPosition target;
    u8 pad5C[0x65 - 0x5C];
    u8 motionAngle;
    u8 pad66[2];
    struct NpcPosition pos;
    u8 pad70[0xEC - 0x70];
    u8 *palette;
    u8 padF0[0x168 - 0xF0];
    NpcReceiver entry;
    u8 pad1A0[0x1CC - 0x168 - sizeof(NpcReceiver)];
    u8 paletteBuf[0x208 - 0x1CC];
    s32 speed;
    s32 speedSquared;
};

extern u8 *gSceneEventDispatchState;

void SceneMessages_SetPendingTrigger(void *, void *);
s32 Math_Atan2(s32, s32);
void SceneNpc_PlayAttachmentEffect(struct SceneNpc *, s32);
void Actor_SetAttachmentEffect(struct SceneNpc *, s32, s32, s32);
void sub_081D010C(struct SceneNpc *, s16 *);
void CpuSet(const void *, void *, u32);

static inline void SetNpcState(struct SceneNpc *npc, u32 sub)
{
    npc->state = 0;
    npc->entered = 0;
    npc->sub = sub;
    npc->timer = 0;
}

static inline void SetNpcFacing(struct SceneNpc *npc, u32 a)
{
    npc->motionAngle = a;
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
static inline void ApplyCommandFacing(struct SceneNpc *npc, u32 a)
{
    npc->motionAngle = a;
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
static inline void SetFlaggedFacing(struct SceneNpc *npc, u32 a, u32 fMask)
{
    npc->motionAngle = a;
    {
        s32 shifted = (s32)(a + 0x70) >> 5;
        u32 mask = 7;
        mask &= shifted;
        npc->facing8 = mask;
    }
    {
        s32 shifted = (s32)(a + 0x60) >> 6;
        u32 mask = fMask;
        mask &= shifted;
        npc->facing4 = mask;
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
struct PaletteResource {
    u8 pad[0xc];
    u8 *palette;
};
static inline void SetPalette(struct PaletteResource *resource, u8 *palette)
{
    resource->palette = palette;
}
static inline void AimAtCommandTarget(struct SceneNpc *npc, s16 *m)
{
    struct NpcPosition *t, *p;
    npc->target.x = m[4];
    npc->target.y = m[5];
    npc->target.z = m[6];
    t = &npc->target;
    p = &npc->pos;
    npc->angle = Math_Atan2(t->x - p->x, t->z - p->z);
}

static inline void SetNpcSpeed(struct SceneNpc *npc, s32 speed)
{
    npc->speed = speed;
    npc->speedSquared = speed * speed;
}

/* Dispatch the active message bank and acknowledge each command before handling it. */
void SceneNpc_DispatchCommands(struct SceneNpc *npc)
{
    struct NpcAuxState *sub;
    NpcReceiver *entry;
    u32 modeBit = 2;
    u32 messageMask = 3;
    u32 copyWords = 0x04000008;
    s32 i;
    entry = &npc->entry;
    sub = &npc->sub20;
    i = 0;
    if (i < entry->count[gSceneEventDispatchState[0x24]])
        do {
            s16 *m;
            u8 *controller = gSceneEventDispatchState;
            m = entry->messages[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(entry, m);
            switch (((u8 *)m)[6]) {
                case 0:
                    if (sub->flag) {
                        SetNpcState(npc, 0x22);
                        npc->changed = 1;
                    } else {
                        SetNpcState(npc, 0);
                        npc->changed = 1;
                    }
                    break;

                case 1:
                    ApplyCommandFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    if (sub->flag) {
                        SetNpcState(npc, 0x23);
                        npc->changed = 1;
                    } else {
                        SetNpcState(npc, 1);
                        npc->changed = 1;
                    }
                    break;

                case 36:
                    AimAtCommandTarget(npc, m);
                    SetNpcSpeed(npc, 10);
                    SetNpcState(npc, 2);
                    npc->changed = 1;
                    break;

                case 4:
                    AimAtCommandTarget(npc, m);
                    SetNpcSpeed(npc, 10);
                    SetNpcState(npc, 0x15);
                    npc->changed = 1;
                    break;

                case 5:
                    AimAtCommandTarget(npc, m);
                    SetNpcSpeed(npc, 20);
                    SetNpcState(npc, 0x15);
                    npc->changed = 1;
                    break;

                case 3:
                    npc->angle = (((u8 *)m)[8] + 5) << 5;
                    SetNpcState(npc, 3);
                    npc->changed = 1;
                    break;

                case 6:
                    SetNpcState(npc, 4);
                    npc->changed = 1;
                    break;

                case 7:
                    SetNpcState(npc, 5);
                    npc->changed = 1;
                    break;

                case 8: {
                    u32 directionMask;
                    npc->angle = (((u8 *)m)[8] + 5) << 5;
                    directionMask = 3;
                    npc->flags |= MessageBits((void *)m, messageMask);
                    if (((u16 *)m)[2] & 1)
                        npc->flags |= 1;
                    SetFlaggedFacing(npc, npc->angle, directionMask);
                    SetNpcState(npc, 0x10);
                    npc->changed = 1;
                    break;
                }
                case 9: {
                    u16 bits = MessageBits((void *)m, modeBit);
                    bits |= npc->flags;
                    npc->flags = bits;
                }
                    SetNpcState(npc, 0x11);
                    npc->changed = 1;
                    break;

                case 10:
                    SetNpcState(npc, 0x13);
                    npc->changed = 1;
                    break;

                case 25:
                    if (sub->flag)
                        SceneNpc_PlayAttachmentEffect(npc, m[4]);
                    else
                        Actor_SetAttachmentEffect(npc, m[4], 0xB4, 0xB4);
                    break;

                case 11:
                    SetNpcState(npc, 0x14);
                    npc->changed = 1;
                    break;

                case 12:
                    SetNpcState(npc, 0x1E);
                    npc->changed = 1;
                    break;

                case 65:
                    SetNpcState(npc, 0x1F);
                    npc->changed = 1;
                    break;

                case 66:
                    SetNpcState(npc, 0x20);
                    npc->changed = 1;
                    break;

                case 67:
                    SetNpcState(npc, 0x21);
                    npc->changed = 1;
                    break;

                case 13:
                    npc->flags |= 4;
                    CpuSet(gPaletteBankData + 0x2B00, npc->paletteBuf, copyWords);
                    SetPalette((void *)((u8 *)npc + 0xe0), npc->paletteBuf);
                    break;

                case 14: {
                    u32 mask = ~4;
                    npc->flags &= mask;
                }
                    CpuSet(gPaletteBankData + 0x2B60, npc->paletteBuf, copyWords);
                    SetPalette((void *)((u8 *)npc + 0xe0), npc->paletteBuf);
                    break;

                case 16: {
                    u32 mask = ~4;
                    npc->flags &= mask;
                }
                    CpuSet(gPaletteBankData + 0x2AE0, npc->paletteBuf, copyWords);
                    SetPalette((void *)((u8 *)npc + 0xe0), npc->paletteBuf);
                    break;

                case 15:
                    npc->flags |= 4;
                    npc->sub20.h2E = 0;
                    CpuSet(gPaletteBankData + 0x2AE0, npc->paletteBuf, copyWords);
                    SetPalette((void *)((u8 *)npc + 0xe0), npc->paletteBuf);
                    break;

                case 64:
                    npc->pos.x = m[4];
                    npc->pos.y = m[5];
                    npc->pos.z = m[6];
                    break;

                case 17:
                    npc->angle = (((u8 *)m)[8] + 5) << 5;
                    {
                        u32 angle = npc->angle + 128;
                        angle &= 255;
                        SetNpcFacing(npc, angle);
                    }
                    SetNpcSpeed(npc, m[5]);
                    SetNpcState(npc, 0x17);
                    npc->changed = 1;
                    break;

                case 18:
                    npc->angle = (((u8 *)m)[8] + 5) << 5;
                    {
                        u32 angle = npc->angle + 128;
                        angle &= 255;
                        SetNpcFacing(npc, angle);
                    }
                    SetNpcSpeed(npc, m[5]);
                    SetNpcState(npc, 0x18);
                    npc->changed = 1;
                    break;

                case 19:
                    ApplyCommandFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    SetNpcState(npc, 0x19);
                    npc->changed = 1;
                    break;

                case 20:
                    SetNpcState(npc, 0x1A);
                    npc->changed = 1;
                    break;

                case 21:
                    SetNpcState(npc, 0x1B);
                    npc->changed = 1;
                    break;

                case 22:
                    SetNpcState(npc, 0x1C);
                    npc->changed = 1;
                    break;

                case 23:
                    ApplyCommandFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    SetNpcState(npc, 0x1D);
                    npc->changed = 1;
                    break;

                case 24:
                    sub_081D010C(npc, m);
                    break;
            }

            i++;
        } while (i < entry->count[gSceneEventDispatchState[0x24]]);
}

#else
INCLUDE_ASM("asm/nonmatching", SceneNpc_DispatchCommands);
#endif
