#include "global.h"
#include "scene_vec.h"
#define SceneNpc_DispatchCutsceneCommands sub_081CD484
#ifdef NONMATCHING
// fuzzy: 93.8%
// Remaining differences: facing register allocation, variant motion setup, and kind dispatch copies.
struct PackedVec {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16, pad : 16;
};
struct Command {
    u8 pad[6];
    u8 kind, unknown7;
    s16 args[4];
};
struct Receiver {
    u8 pad[6];
    u8 count[2];
    struct Command *pending[2][4];
};
struct Npc {
    u8 unknown0, kind, state, entered, mode, unknown5[3], changed;
    u8 unknown9[0x14 - 9];
    u32 timer;
    u8 unknown18, facing4, facing8, angle;
    u8 unknown1C[0x26 - 0x1C];
    s16 variant;
    u8 unknown28[0x54 - 0x28];
    struct SceneVec target;
    u8 unknown5C[9];
    u8 motionAngle;
    u8 unknown66[2];
    struct SceneVec pos;
    u8 unknown70[0x168 - 0x70];
    struct Receiver receiver;
    u8 unknown190[0x208 - 0x168 - sizeof(struct Receiver)];
    s32 speed, speedSquared;
};
extern u8 *gUnk_0200047C, *gUnk_02000580, *gUnk_030042E4;
void SceneMessages_SetPendingTrigger(void *, void *);
s32 Math_Atan2(s32, s32);
void *memset(void *, int, u32);
void *SceneMessages_FindScript(s32);
void Trigger_CreateDrop(void *, s32, s32, void *, void *, s32, s32, s32);
void sub_08017044(s32, s32, s32, s32, void *, void *, s32, void *);
void Actor_SetAttachmentEffect(void *, s32, s32, s32);
void SunOrb_GetOrCreateManager(void);
s32 SunOrb_CreateSceneTargeted(struct PackedVec *);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
void Trigger_ConsumePending(void *, s32);
static inline void setState(struct Npc *p, u32 mode)
{
    p->state = 0;
    p->entered = 0;
    p->mode = mode;
    p->timer = 0;
}
static inline void setFacing(struct Npc *p, u32 a)
{
    p->motionAngle = a;
    p->facing8 = 7 & ((s32)(a + 0x70) >> 5);
    p->facing4 = 3 & ((s32)(a + 0x60) >> 6);
}
static inline void aim(struct Npc *p, struct Command *m)
{
    struct SceneVec *t, *v;
    p->target.x = m->args[0];
    p->target.y = m->args[1];
    p->target.z = m->args[2];
    t = &p->target;
    v = &p->pos;
    p->angle = Math_Atan2(t->x - v->x, t->z - v->z);
}
static inline void setSpeed(struct Npc *p, s32 speed)
{
    p->speed = speed;
    p->speedSquared = speed * speed;
}
static inline void place(struct Npc *p, struct Command *m)
{
    p->pos.x = m->args[0];
    p->pos.y = m->args[1];
    p->pos.z = m->args[2];
}
void SceneNpc_DispatchCutsceneCommands(struct Npc *p)
{
    struct Receiver *receiver = &p->receiver;
    s32 i = 0;
    if (i < receiver->count[gUnk_0200047C[0x24]])
        do {
            u8 *controller = gUnk_0200047C;
            struct Command *m = receiver->pending[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(receiver, m);
            switch (m->kind) {
                case 0:
                    setState(p, 0);
                    p->changed = 1;
                    break;
                case 1:
                    setFacing(p, (u8)((((u8 *)m)[8] + 5) << 5));
                    if (p->kind == 29) {
                        if (p->variant == 1) {
                            setState(p, 0x5D);
                            p->changed = 1;
                        } else {
                            setState(p, 1);
                            p->changed = 1;
                        }
                    } else {
                        setState(p, 1);
                        p->changed = 1;
                    }
                    break;
                case 2:
                    setState(p, 1);
                    p->changed = 1;
                    break;
                case 68: {
                    s32 zero[3];
                    memset(zero, 0, 12);
                    if (m->args[0] == 0)
                        Trigger_CreateDrop(&p->pos, m->args[1], m->args[2], 0, 0, 0, m->args[2], 0);
                    else {
                        void *script = SceneMessages_FindScript(m->args[0]);
                        Trigger_CreateDrop(&p->pos, m->args[1], m->args[2], script, zero, 0,
                                           m->args[2], 0);
                    }
                } break;
                case 69: {
                    s32 a = m->args[0], b = m->args[1];
                    struct SceneVec *pos = &p->pos;
                    struct PackedVec v = *(struct PackedVec *)(gUnk_02000580 + 0x30);
                    v.y = v.y + 200;
                    sub_08017044(a, b, 3, 0, &v, pos, 24, pos);
                } break;
                case 4:
                    setSpeed(p, 10);
                    aim(p, m);
                    setState(p, 16);
                    p->changed = 1;
                    break;
                case 5:
                    setSpeed(p, 20);
                    aim(p, m);
                    setState(p, 16);
                    p->changed = 1;
                    break;
                case 36:
                    setSpeed(p, 10);
                    aim(p, m);
                    setState(p, 2);
                    p->changed = 1;
                    break;
                case 3:
                    p->angle = (((u8 *)m)[8] + 5) << 5;
                    setState(p, 3);
                    p->changed = 1;
                    break;
                case 6:
                    setState(p, 4);
                    p->changed = 1;
                    break;
                case 7:
                    setState(p, 5);
                    p->changed = 1;
                    break;
                case 25:
                    Actor_SetAttachmentEffect(p, m->args[0], 180, 180);
                    break;
                case 65:
                    setState(p, 0x14);
                    p->changed = 1;
                    break;
                case 66:
                    setState(p, 0x15);
                    p->changed = 1;
                    break;
                case 67:
                    setState(p, 0x16);
                    p->changed = 1;
                    break;
                case 64:
                    place(p, m);
                    break;
                case 62:
                    setState(p, 0x1d);
                    p->changed = 1;
                    break;
                case 59:
                    setState(p, 0x1a);
                    p->changed = 1;
                    break;
                case 60:
                    setState(p, 0x1b);
                    p->changed = 1;
                    break;
                case 61:
                    setState(p, 0x1c);
                    p->changed = 1;
                    break;
                case 29:
                    setState(p, 0x20);
                    p->changed = 1;
                    break;
                case 34:
                    setState(p, 0x23);
                    p->changed = 1;
                    break;
                case 35:
                    setState(p, 0x24);
                    p->changed = 1;
                    break;
                case 32:
                    setState(p, 0x28);
                    p->changed = 1;
                    break;
                case 33:
                    setState(p, 0x29);
                    p->changed = 1;
                    break;
                case 39:
                    setState(p, 0x2a);
                    p->changed = 1;
                    break;
                case 52:
                    setState(p, 0x2c);
                    p->changed = 1;
                    break;
                case 38:
                    setState(p, 0x2d);
                    p->changed = 1;
                    break;
                case 49:
                    setState(p, 0x2f);
                    p->changed = 1;
                    break;
                case 50:
                    setState(p, 0x30);
                    p->changed = 1;
                    break;
                case 51:
                    setState(p, 0x31);
                    p->changed = 1;
                    break;
                case 53:
                    setState(p, 0x32);
                    p->changed = 1;
                    break;
                case 54:
                    setState(p, 0x33);
                    p->changed = 1;
                    break;
                case 55:
                    setState(p, 0x34);
                    p->changed = 1;
                    break;
                case 28:
                    setState(p, 0x36);
                    p->changed = 1;
                    break;
                case 70:
                    setState(p, 0x37);
                    p->changed = 1;
                    break;
                case 71:
                    setState(p, 0x38);
                    p->changed = 1;
                    break;
                case 72:
                    aim(p, m);
                    setSpeed(p, 10);
                    setState(p, 0x39);
                    p->changed = 1;
                    break;
                case 73:
                    setState(p, 0x3a);
                    p->changed = 1;
                    break;
                case 75:
                    p->angle = (((u8 *)m)[8] + 5) << 5;
                    setFacing(p, (p->angle + 128) & 255);
                    setSpeed(p, m->args[1]);
                    setState(p, 0x3C);
                    p->changed = 1;
                    break;
                case 76:
                    setState(p, 0x3d);
                    p->changed = 1;
                    break;
                case 77:
                    place(p, m);
                    setSpeed(p, 10);
                    setState(p, 0x3E);
                    p->changed = 1;
                    break;
                case 78:
                    place(p, m);
                    setSpeed(p, 10);
                    setState(p, 0x3F);
                    p->changed = 1;
                    break;
                case 79:
                    place(p, m);
                    setState(p, 0x40);
                    p->changed = 1;
                    break;
                case 80:
                    setState(p, 0x41);
                    p->changed = 1;
                    break;
                case 81:
                    setState(p, 0x42);
                    p->changed = 1;
                    break;
                case 82:
                    setState(p, 0x43);
                    p->changed = 1;
                    break;
                case 84:
                    setState(p, 0x46);
                    p->changed = 1;
                    break;
                case 85:
                    setState(p, 0x47);
                    p->changed = 1;
                    break;
                case 86:
                    setState(p, 0x48);
                    p->changed = 1;
                    break;
                case 87:
                    setState(p, 0x49);
                    p->changed = 1;
                    break;
                case 88:
                    setState(p, 0x4a);
                    p->changed = 1;
                    break;
                case 89:
                    setState(p, 0x4b);
                    p->changed = 1;
                    break;
                case 90:
                    setState(p, 0x4c);
                    p->changed = 1;
                    break;
                case 91:
                    setState(p, 0x4d);
                    p->changed = 1;
                    break;
                case 92:
                    setState(p, 0x4f);
                    p->changed = 1;
                    break;
                case 93:
                    setFacing(p, (u8)((((u8 *)m)[8] + 5) << 5));
                    setState(p, 83);
                    p->changed = 1;
                    break;
                case 94:
                    setState(p, 0x54);
                    p->changed = 1;
                    break;
                case 95:
                    setState(p, 0x55);
                    p->changed = 1;
                    break;
                case 96:
                    setFacing(p, (u8)((((u8 *)m)[8] + 5) << 5));
                    setState(p, 86);
                    p->changed = 1;
                    break;
                case 97:
                    setState(p, 0x59);
                    p->changed = 1;
                    break;
                case 98:
                    setState(p, 0x5a);
                    p->changed = 1;
                    break;
                case 99:
                    setSpeed(p, m->args[3]);
                    aim(p, m);
                    setState(p, 0x5B);
                    p->changed = 1;
                    break;
                case 100: {
                    u8 *extra = (u8 *)p + 0x20;
                    struct PackedVec v;
                    v.x = m->args[0];
                    v.y = m->args[1];
                    v.z = m->args[2];
                    SunOrb_GetOrCreateManager();
                    *(s16 *)(extra + 4) = SunOrb_CreateSceneTargeted(&v);
                    Trigger_ConsumePending(&p->receiver, 1);
                } break;
                case 101:
                    setState(p, 0x5c);
                    p->changed = 1;
                    break;
                case 102:
                    setState(p, 0x13);
                    p->changed = 1;
                    break;
                case 103: {
                    u8 *motion = (u8 *)p + 0x5C;
                    s32 y;
                    p->variant = m->args[0];
                    if (p->variant == 1) {
                        setState(p, 0x5D);
                        p->changed = 1;
                        y = 528;
                    } else
                        y = 400;
                    *(s16 *)(motion + 0x34) = 0;
                    *(s16 *)(motion + 0x36) = y;
                    *(s16 *)(motion + 0x38) = 0;
                    Trigger_ConsumePending(&p->receiver, 1);
                } break;
                case 104: {
                    u8 *extra = (u8 *)p + 0x20;
                    s32 bank;
                    *(s16 *)(extra + 0x12) = 0;
                    *(s16 *)(extra + 0x10) = m->args[0];
                    if (m->args[1] == 1)
                        bank = 372;
                    else if (m->args[1] == 9)
                        bank = 367;
                    else
                        bank = 52;
                    *(s32 *)(extra + 0x18) = bank;
                    if (*(s16 *)(extra + 0x10) == 0) {
                        u8 *pal = gUnk_030042E4 + bank * 32;
                        Palette_InterpolateBanks((u8 *)p + 0x1CC, pal, pal, 2, 1);
                        *(s32 *)(extra + 0x14) = *(s32 *)(extra + 0x18);
                    }
                    Trigger_ConsumePending(&p->receiver, 1);
                } break;
                case 105:
                    setState(p, 0x5e);
                    p->changed = 1;
                    break;
                case 106:
                    setState(p, 0x5f);
                    p->changed = 1;
                    break;
                case 107:
                    setState(p, 0x60);
                    p->changed = 1;
                    break;
                case 30:
                    if (p->kind == 9) {
                        setState(p, 0x19);
                        p->changed = 1;
                    } else if (p->kind == 4) {
                        setState(p, 0x21);
                        p->changed = 1;
                    }
                    break;
                case 27: {
                    u32 kind = p->kind;
                    if (kind == 5) {
                        setState(p, 0x26);
                        p->changed = 1;
                    } else if (kind == 4) {
                        setState(p, 0x1F);
                        p->changed = 1;
                    } else if (kind == 27) {
                        setState(p, 0x35);
                        p->changed = 1;
                    } else if ((u8)(kind - 9) <= 1) {
                        setState(p, 0x18);
                        p->changed = 1;
                    } else if (kind == 29) {
                        setState(p, 0x58);
                        p->changed = 1;
                    }
                } break;
                case 26: {
                    u32 kind = p->kind;
                    if ((u8)(kind - 9) <= 1) {
                        setState(p, 0x17);
                        p->changed = 1;
                    } else if ((u8)kind == 4) {
                        setState(p, 0x1E);
                        p->changed = 1;
                    } else if ((u8)kind == 5) {
                        setState(p, 0x25);
                        p->changed = 1;
                    } else if ((u8)kind == 27) {
                        setState(p, 0x45);
                        p->changed = 1;
                    } else if ((u8)kind == 28) {
                        setState(p, 0x50);
                        p->changed = 1;
                    } else if ((u8)kind == 29) {
                        setState(p, 0x57);
                        p->changed = 1;
                    }
                } break;
                case 83:
                    if (p->kind == 27) {
                        setState(p, 68);
                        p->changed = 1;
                    } else if (p->kind == 28) {
                        setState(p, 81);
                        p->changed = 1;
                    }
                    break;
                case 31:
                    if (p->kind == 4) {
                        setState(p, 34);
                        p->changed = 1;
                    } else if (p->kind == 5) {
                        setState(p, 39);
                        p->changed = 1;
                    }
                    break;
                case 40:
                    if (p->kind == 5) {
                        setState(p, 43);
                        p->changed = 1;
                    } else if (p->kind == 15) {
                        setState(p, 46);
                        p->changed = 1;
                    }
                    break;
                case 74:
                    p->angle = (((u8 *)m)[8] + 5) << 5;
                    setFacing(p, (p->angle + 128) & 255);
                    setSpeed(p, m->args[1]);
                    if (p->kind == 27) {
                        setState(p, 0x3B);
                        p->changed = 1;
                    } else if (p->kind == 28) {
                        setState(p, 0x52);
                        p->changed = 1;
                    }
                    break;
                case 108:
                    setState(p, 0x63);
                    p->changed = 1;
                    break;
                default:
                    break;
            }
            i++;
        } while (i < receiver->count[gUnk_0200047C[0x24]]);
}

#else
INCLUDE_ASM("asm/nonmatching", SceneNpc_DispatchCommands_081CD484);
#endif
