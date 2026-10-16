// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
struct PackedVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
struct Rig {
    u8 pad0[4];
    u32 handle;
    u8 pad8[4];
    u8 fieldC;
    u8 attached;
    u8 padE[2];
};
struct Motion {
    u8 pad0[9];
    u8 angle;
    u8 padA[0x2A];
    u16 field34;
    u16 field36;
    u16 field38;
    u8 pad3A[0x3E];
};
struct SceneNpc {
    u8 pad0[2];
    u8 field2;
    u8 field3;
    u8 mode;
    u8 pad5;
    u8 state;
    u8 pad7;
    u8 field8;
    u8 field9;
    u8 padA[2];
    u16 fieldC;
    u8 padE[2];
    u32 field10;
    u32 field14;
    u8 field18;
    u8 facing4;
    u8 facing8;
    u8 pad1B[5];
    struct Rig rig;
    u8 pad30[0x2C];
    struct Motion motion;
    u8 blockD4[0x8C];
    u32 field160;
    u8 pad164[4];
    u8 block168[0x8C];
    u16 field1F4;
    u16 field1F6;
    u32 field1F8;
    u32 field1FC;
    u32 field200;
    u32 field204;
    u8 pad208[0x13];
    u8 field21B;
    u8 pad21C[0xB0];
    void *update;
    void *callbacks[6];
};
extern void *gUnk_02000480;
extern struct SceneNpc *gUnk_02000274;
struct SceneNpc *sub_0802D6F4(s32 name, s32 variant);
s32 sub_0802D728(struct SceneNpc *, s32 name, struct PackedVec *, s32 angle);
s32 sub_0802D760(struct SceneNpc *, s32 kind, s32 a, s32 b, s32 c);
void sub_0802D7E0(struct SceneNpc *, u32, s32, s32, s32);
void SceneMessages_RegisterReceiver(void *, s32, s32);
void sub_0802D614(void *, struct SceneNpc *, s32, s32);
void sub_0802D654(void *, struct SceneNpc *);
void sub_0802E698(void *, struct SceneNpc *);
void DisplayAnimation_SetFlags(void *);
void sub_0802D88C(struct SceneNpc *);
s32 sub_0802DB78(struct SceneNpc *, s32, s32, s32, s32, s32);
u32 sub_08163E24(void);
void sub_081D07D8(void *, struct SceneNpc *);
void sub_081CFFAC(void);
void sub_081D0864(void);
void sub_081D0BC4(void);
void sub_081D0BD0(void);
void sub_081D0928(void);
void sub_081D0B8C(void);
void sub_0802EA4C(void);
void sub_0802EA08(void);
void sub_081CEFCC(void);
void sub_081CF84C(void);
void sub_0802F0C0(void);
static inline void setHandler(void *fn, void **slot)
{
    *slot = fn;
}

static inline void setBox(struct SceneNpc *npc, s32 width, s32 height)
{
    npc->field1F4 = width;
    npc->field1F6 = height;
    npc->field1F8 = width;
    npc->field1FC = height;
}

/* Registry routine BB34: creates a scene NPC (0x384-byte object from
   sub_0802D6F4) named -n, variant -k (default 1), at -p x y z facing -d, with
   behavior -i (0-4: update callback, mode and state), records it in
   gUnk_02000274 and registers it with the scene actor manager; -K also runs
   sub_081D07D8 on it. The facing computation needs register pins and an empty
   asm barrier (agbcc otherwise turns the arithmetic shift into a logical one). */
void SceneNpc_CreateFromScript(void)
{
    void (*update)(void);
    u8 mode;
    void (*updateB)(void);
    void *manager = gUnk_02000480;
    s32 name;
    s32 variant;
    s32 angle;
    s32 extra;
    struct SceneNpc *npc;
    void (*updateC)(void);
    struct PackedVec position;
    u32 zero;
    s32 model;
    struct Rig *rig;
    struct Motion *motion;
    if (!manager) {
        return;
    }
    name = Script_GetKeywordValue('n', 0);
    variant = Script_GetKeywordValue('k', 1);
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    angle = Script_GetKeywordValue('d', 0);
    extra = Script_GetKeywordValue('K', 0);
    npc = sub_0802D6F4(name, variant);
    if (!npc) {
        return;
    }
    gUnk_02000274 = npc;
    if (sub_0802D728(npc, name, &position, angle) < 0) {
        return;
    }
    model = 0x97D3;
    if (sub_0802D760(npc, 1, model, 0xC6A0, zero = 0) < 0) {
        return;
    }
    motion = &npc->motion;
    {
        s32 speed = 400;
        motion->field34 = zero;
        motion->field36 = speed;
        motion->field38 = zero;
    }
    sub_0802D7E0(npc, npc->field160, 1, 0, 0x40);
    SceneMessages_RegisterReceiver(npc->block168, name, 5);
    npc->motion.angle = angle;
    {
        u16 a = angle;
        {
            register s32 t asm("r2");
            u32 m;
            t = a + 0x70;
            asm("" : "+r"(t));
            t >>= 5;
            m = 7;
            m &= t;
            npc->facing8 = m;
        }
        {
            s32 t = (a + 0x60) >> 6;
            u32 m;
            m = 3;
            m &= t;
            npc->facing4 = m;
        }
    }
    setHandler(sub_081CFFAC, &npc->callbacks[5]);
    setHandler(sub_081D0864, &npc->callbacks[0]);
    setHandler(sub_081D0BC4, &npc->callbacks[1]);
    setHandler(sub_081D0BD0, &npc->callbacks[2]);
    setHandler(sub_081D0928, &npc->callbacks[3]);
    setHandler(sub_081D0B8C, &npc->callbacks[4]);
    sub_0802D614(manager, npc, model, 0);
    sub_0802D654(manager, npc);
    sub_0802E698(manager, npc);
    rig = &npc->rig;
    npc->fieldC = zero;
    rig->fieldC = 0;
    switch (Script_GetKeywordValue('i', 0)) {
        case 0: {
            u8 one = 1;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = one;
            npc->field14 = 0;
            npc->field8 = 1;
            update = sub_0802EA4C;
            npc->state = 2;
            npc->update = update;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;
        }

        case 1:
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = 0;
            npc->field14 = 0;
            npc->field8 = 1;
            update = sub_0802EA08;
            npc->state = 1;
            npc->update = update;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            DisplayAnimation_SetFlags(npc->blockD4);
            sub_0802D88C(npc);
            break;

        case 2:
            npc->field21B = 1;
            mode = 0x12;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = mode;
            npc->field14 = 0;
            npc->field8 = 1;
            update = sub_081CEFCC;
            npc->state = 0x10;
            npc->update = update;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;

        case 3:
            mode = 0x19;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = mode;
            npc->field14 = 0;
            npc->field8 = 1;
            updateC = sub_081CF84C;
            npc->state = 0x16;
            npc->update = updateC;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;

        case 4:
            mode = 6;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = mode;
            npc->field14 = 0;
            npc->field8 = 1;
            updateB = sub_0802F0C0;
            npc->state = 7;
            npc->update = updateB;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;
    }

    sub_0802DB78(npc, 0, 0, 0, 0, 0);
    setBox(npc, 16, 20);
    npc->field200 = 0x100;
    npc->field204 = 0x190;
    rig->handle = sub_08163E24();
    if (extra) {
        rig->attached = 1;
        sub_081D07D8(manager, npc);
    }
}
