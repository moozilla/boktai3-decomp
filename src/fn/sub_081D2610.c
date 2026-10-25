#include "global.h"
typedef struct {
    u16 id;
    u8 unk_02[4];
    u8 count[2];
    s16 *messages[2][4];
    u8 unk_28[8];
    void *next;
} SceneMessageReceiver;

struct NpcVec {
    s16 x, y, z, w;
};

/* The variant and arc parameter names are provisional. */
struct TaggedNpcState {
    u8 unk_00[0xE];
    s16 duration;
    s16 startX, startY, startZ, variant;
    u8 unk_18[4];
};

struct NpcMotion {
    u8 unk_00[9];
    u8 angle;
    u8 unk_0A[2];
    struct NpcVec pos;
    u8 unk_14[0x20];
    u16 x, y, z;
};
struct TaggedSceneNpc {
    u8 unk_00;
    u8 kind;
    u8 state, entered, sub;
    u8 unk_05[3];
    u8 changed;
    u8 unk_09[3];
    u16 flags;
    u8 unk_0E[0x14 - 0xE];
    u32 timer;
    u8 unk_18;
    u8 facing4, facing8, angle;
    u8 unk_1C[0x20 - 0x1C];
    struct TaggedNpcState sub20;
    u8 unk_3C[0x54 - 0x3C];
    struct NpcVec target;
    struct NpcMotion motion;
    u8 unk_96[0xEC - 0x98];
    u8 *palette;
    u8 unk_F0[0x168 - 0xF0];
    SceneMessageReceiver entry;
    u8 unk_19C[0x1CC - 0x168 - sizeof(SceneMessageReceiver)];
    u8 paletteBuf[0x208 - 0x1CC];
    s32 speed;
    s32 speed2;
};

extern u8 *gUnk_0200047C;

void SceneMessages_SetPendingTrigger(void *, void *);
s32 Math_Atan2(s32, s32);

void Actor_SetAttachmentEffect(struct TaggedSceneNpc *, s32, s32, s32);

static inline void setState(struct TaggedSceneNpc *npc, u32 sub)
{
    npc->state = 0;
    npc->entered = 0;
    npc->sub = sub;
    npc->timer = 0;
}

static inline void setFacing(struct TaggedSceneNpc *npc, s16 a)
{
    npc->motion.angle = a;
    {
        s32 t = (s32)(a + 0x70) >> 5;
        u32 m = 7;
        m &= t;
        npc->facing8 = m;
    }
    {
        s32 t = (s32)(a + 0x60) >> 6;
        u32 m = 3;
        m &= t;
        npc->facing4 = m;
    }
}

static inline void aim(struct TaggedSceneNpc *npc, s16 *m)
{
    struct NpcVec *t, *p;
    npc->target.x = m[4];
    npc->target.y = m[5];
    npc->target.z = m[6];
    t = &npc->target;
    p = &npc->motion.pos;
    npc->angle = Math_Atan2(t->x - p->x, t->z - p->z);
}

static inline void setSpeed(struct TaggedSceneNpc *npc, s32 speed)
{
    npc->speed = speed;
    npc->speed2 = speed * speed;
}

void TaggedSceneNpc_DispatchCutsceneCommands(u8 *arg0)
{
    struct TaggedSceneNpc *npc = (struct TaggedSceneNpc *)arg0;
    SceneMessageReceiver *entry = &npc->entry;
    struct TaggedNpcState *sub = &npc->sub20;
    s32 i = 0;
    if (i < entry->count[gUnk_0200047C[0x24]])
        do {
            u8 *controller = gUnk_0200047C;
            s16 *m = entry->messages[controller[0x24]][i];
            SceneMessages_SetPendingTrigger(entry, m);
            switch (((u8 *)m)[6]) {
                case 0:
                    setState(npc, 0);
                    npc->changed = 1;
                    break;
                case 1:
                    setFacing(npc, (u8)((((u8 *)m)[8] + 5) << 5));
                    setState(npc, 1);
                    npc->changed = 1;
                    break;
                case 3:
                    npc->angle = (((u8 *)m)[8] + 5) << 5;
                    setState(npc, 3);
                    npc->changed = 1;
                    break;
                case 36:
                    aim(npc, m);
                    setState(npc, 2);
                    npc->changed = 1;
                    break;
                case 4:
                    aim(npc, m);
                    setSpeed(npc, 10);
                    setState(npc, 16);
                    npc->changed = 1;
                    break;
                case 5:
                    aim(npc, m);
                    setSpeed(npc, 20);
                    setState(npc, 16);
                    npc->changed = 1;
                    break;
                case 37:
                    setState(npc, 18);
                    npc->changed = 1;
                    sub->variant = 0;
                    break;
                case 38:
                    setState(npc, 21);
                    npc->changed = 1;
                    sub->variant = 0;
                    break;
                case 39:
                    setState(npc, 19);
                    npc->changed = 1;
                    sub->variant = 1;
                    break;
                case 40:
                    setState(npc, 20);
                    npc->changed = 1;
                    break;
                case 41:
                    setState(npc, 21);
                    npc->changed = 1;
                    sub->variant = 1;
                    break;
                case 42:
                    npc->target.x = m[4];
                    npc->target.y = m[5];
                    npc->target.z = m[6];
                    setState(npc, 22);
                    npc->changed = 1;
                    break;
                case 43:
                    sub->startX = m[4];
                    sub->startY = m[5];
                    sub->startZ = m[6];
                    npc->target.x = m[7];
                    npc->target.y = m[8];
                    npc->target.z = m[9];
                    sub->duration = m[10];
                    setState(npc, 23);
                    npc->changed = 1;
                    break;
                case 44:
                    setState(npc, 24);
                    npc->changed = 1;
                    break;
                case 45:
                    setState(npc, 25);
                    npc->changed = 1;
                    break;
                case 25:
                    Actor_SetAttachmentEffect(npc, m[4], m[5], m[6]);
                    break;
                case 64:
                    npc->motion.pos.x = m[4];
                    npc->motion.pos.y = m[5];
                    npc->motion.pos.z = m[6];
                    break;
            }
            i++;
        } while (i < entry->count[gUnk_0200047C[0x24]]);
}
