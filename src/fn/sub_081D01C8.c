#include "global.h"
#ifdef NONMATCHING
// fuzzy: 72.5%
// fix.py best version (reprinted C)
// NPC cutscene dispatcher: most commands set state 0 with a sub-state;
// 36/4/5 walk towards a point (aim + speed), 3/8/17/18 set the angle,
// 13-16 swap the palette, 64 places the NPC, 24 runs sub_081D010C.
// Still differs: sb/r8 swapped for sub/i; the facing math wants the AND
// constant as destination (see SceneNpc_CommandFromScript, which pins it);
// case 0 shares a tail store the original repeats.

typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    s16 *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;

struct Vec {
    s16 x, y, z, w;
};

struct Sub {
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
    struct Sub sub20;
    u8 pad30[0x54 - 0x30];
    struct Vec target;
    u8 pad5C[0x65 - 0x5C];
    u8 motionAngle;
    u8 pad66[2];
    struct Vec pos;
    u8 pad70[0xEC - 0x70];
    u8 *palette;
    u8 padF0[0x168 - 0xF0];
    Entry entry;
    u8 pad1A0[0x1CC - 0x168 - sizeof(Entry)];
    u8 paletteBuf[0x208 - 0x1CC];
    s32 speed;
    s32 speed2;
};

extern u8 *gUnk_0200047C;
extern u8 *gUnk_030042E4;

void SceneMessages_SetPendingTrigger(void *, void *);
s32 Math_Atan2(s32, s32);
void SceneNpc_PlayAttachmentEffect(struct SceneNpc *, s32);
void Actor_SetAttachmentEffect(struct SceneNpc *, s32, s32, s32);
void sub_081D010C(struct SceneNpc *, s16 *);
void CpuSet(const void *, void *, u32);

static inline void setState(struct SceneNpc *npc, u32 sub)
{
    npc->state = 0;
    npc->entered = 0;
    npc->sub = sub;
    npc->timer = 0;
}

static inline void setFacing(struct SceneNpc *npc, u32 a)
{
    npc->motionAngle = a;
    npc->facing8 = 7 & ((s32)(a + 0x70) >> 5);
    npc->facing4 = 3 & ((s32)(a + 0x60) >> 6);
}

static inline void aim(struct SceneNpc *npc, s16 *m)
{
    struct Vec *t, *p;
    npc->target.x = m[4];
    npc->target.y = m[5];
    npc->target.z = m[6];
    t = &npc->target;
    p = &npc->pos;
    npc->angle = Math_Atan2(t->x - p->x, t->z - p->z);
}

static inline void setSpeed(struct SceneNpc *npc, u8 speed)
{
    npc->speed = speed;
    npc->speed2 = speed * speed;
}

void SceneNpc_DispatchCommands(struct SceneNpc *npc)
{
    Entry *entry = &npc->entry;
    struct Sub *sub = &npc->sub20;
    s32 i;
    i = 0;
    if (i < entry->count[gUnk_0200047C[0x24]])
        do {
            s16 *m;
            u8 *controller = gUnk_0200047C;
            m = entry->messages[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(entry, m);
            switch (((u8 *)m)[6]) {
                case 0:
                    if (sub->flag)
                        setState(npc, 0x22);
                    else
                        setState(npc, 0);
                    npc->changed = 1;
                    break;

                case 1:
                    setFacing(npc, (u8)((m[4] + 5) << 5));
                    if (sub->flag) {
                        setState(npc, 0x23);
                        npc->changed = 1;
                    } else {
                        setState(npc, 1);
                        npc->changed = 1;
                    }
                    break;

                case 36:
                    aim(npc, m);
                    setSpeed(npc, 10);
                    setState(npc, 2);
                    npc->changed = 1;
                    break;

                case 4:
                    aim(npc, m);
                    setSpeed(npc, 10);
                    setState(npc, 0x15);
                    npc->changed = 1;
                    break;

                case 5:
                    aim(npc, m);
                    setSpeed(npc, 20);
                    setState(npc, 0x15);
                    npc->changed = 1;
                    break;

                case 3:
                    npc->angle = (m[4] + 5) << 5;
                    setState(npc, 3);
                    npc->changed = 1;
                    break;

                case 6:
                    setState(npc, 4);
                    npc->changed = 1;
                    break;

                case 7:
                    setState(npc, 5);
                    npc->changed = 1;
                    break;

                case 8:
                    npc->angle = (m[4] + 5) << 5;
                    npc->flags |= ((u16 *)m)[2] & 3;
                    if (((u16 *)m)[2] & 1)
                        npc->flags |= 1;
                    setFacing(npc, npc->angle);
                    setState(npc, 0x10);
                    npc->changed = 1;
                    break;

                case 9:
                    npc->flags |= (u16)(((u16 *)m)[2] & 2);
                    setState(npc, 0x11);
                    npc->changed = 1;
                    break;

                case 10:
                    setState(npc, 0x13);
                    npc->changed = 1;
                    break;

                case 25:
                    if (sub->flag)
                        SceneNpc_PlayAttachmentEffect(npc, m[4]);
                    else
                        Actor_SetAttachmentEffect(npc, m[4], 0xB4, 0xB4);
                    break;

                case 11:
                    setState(npc, 0x14);
                    npc->changed = 1;
                    break;

                case 12:
                    setState(npc, 0x1E);
                    npc->changed = 1;
                    break;

                case 65:
                    setState(npc, 0x1F);
                    npc->changed = 1;
                    break;

                case 66:
                    setState(npc, 0x20);
                    npc->changed = 1;
                    break;

                case 67:
                    setState(npc, 0x21);
                    npc->changed = 1;
                    break;

                case 13:
                    npc->flags |= 4;
                    CpuSet(gUnk_030042E4 + 0x2B00, npc->paletteBuf, 0x04000008);
                    npc->palette = npc->paletteBuf;
                    break;

                case 14:
                    npc->flags &= ~4;
                    CpuSet(gUnk_030042E4 + 0x2B60, npc->paletteBuf, 0x04000008);
                    npc->palette = npc->paletteBuf;
                    break;

                case 16:
                    npc->flags &= ~4;
                    CpuSet(gUnk_030042E4 + 0x2AE0, npc->paletteBuf, 0x04000008);
                    npc->palette = npc->paletteBuf;
                    break;

                case 15:
                    npc->flags |= 4;
                    sub->h2E = 0;
                    CpuSet(gUnk_030042E4 + 0x2AE0, npc->paletteBuf, 0x04000008);
                    npc->palette = npc->paletteBuf;
                    break;

                case 64:
                    npc->pos.x = m[4];
                    npc->pos.y = m[5];
                    npc->pos.z = m[6];
                    break;

                case 17:
                    npc->angle = (m[4] + 5) << 5;
                    setFacing(npc, (u8)(npc->angle + 0x80));
                    setSpeed(npc, m[5]);
                    setState(npc, 0x17);
                    npc->changed = 1;
                    break;

                case 18:
                    npc->angle = (m[4] + 5) << 5;
                    setFacing(npc, (u8)(npc->angle + 0x80));
                    setSpeed(npc, m[5]);
                    setState(npc, 0x18);
                    npc->changed = 1;
                    break;

                case 19:
                    setFacing(npc, (u8)((m[4] + 5) << 5));
                    setState(npc, 0x19);
                    npc->changed = 1;
                    break;

                case 20:
                    setState(npc, 0x1A);
                    npc->changed = 1;
                    break;

                case 21:
                    setState(npc, 0x1B);
                    npc->changed = 1;
                    break;

                case 22:
                    setState(npc, 0x1C);
                    npc->changed = 1;
                    break;

                case 23:
                    setFacing(npc, (u8)((m[4] + 5) << 5));
                    setState(npc, 0x1D);
                    npc->changed = 1;
                    break;

                case 24:
                    sub_081D010C(npc, m);
                    break;
            }

            i++;
        } while (i < entry->count[gUnk_0200047C[0x24]]);
}
#else
INCLUDE_ASM("asm/nonmatching", SceneNpc_DispatchCommands);
#endif
