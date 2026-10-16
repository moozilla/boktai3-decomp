#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 66.8%
// Open: the 0xFFFF0000 mask stays in r4 across the -p reads (the original
// reloads it per write), and register choices in the facing/callback stores.
struct PackedVec {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
struct Rig {
    u8 pad0[4];
    u32 handle;
    u8 pad8[4];
    u8 fieldC, attached;
    u8 padE[2];
};
struct Motion {
    u8 pad0[9];
    u8 angle;
    u8 padA[0x2A];
    u16 field34, field36, field38;
    u8 pad3A[0x3E];
};
struct SceneNpc {
    u8 pad0[2];
    u8 field2, field3, mode;
    u8 pad5;
    u8 state;
    u8 pad7;
    u8 field8, field9;
    u8 padA[2];
    u16 fieldC;
    u8 padE[2];
    u32 field10, field14;
    u8 field18, facing4, facing8;
    u8 pad1B[5];
    struct Rig rig;
    u8 pad30[0x2C];
    struct Motion motion;
    u8 blockD4[0x8C];
    u32 field160;
    u8 pad164[4];
    u8 block168[0x8C];
    u16 field1F4, field1F6;
    u32 field1F8, field1FC, field200, field204;
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
/* Script: creates scene NPC -n (variant -k, default 1) at -p x y z facing -d,
   in behavior -i (0-4), and stores it in gUnk_02000274; -K also runs
   sub_081D07D8 on it. */
void SceneNpc_CreateFromScript(void)
{
    void *manager = gUnk_02000480;
    s32 name, variant, angle, extra;
    struct SceneNpc *npc;
    struct PackedVec position;
    u32 zero;
    s32 model;
    struct Rig *rig;
    struct Motion *motion;
    if (!manager)
        return;
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
    if (!npc)
        return;
    gUnk_02000274 = npc;
    if (sub_0802D728(npc, name, &position, angle) < 0)
        return;
    model = 0x97D3;
    zero = 0;
    if (sub_0802D760(npc, 1, model, 0xC6A0, zero) < 0)
        return;
    motion = &npc->motion;
    motion->field34 = zero;
    motion->field36 = 400;
    motion->field38 = zero;
    sub_0802D7E0(npc, npc->field160, 1, 0, 0x40);
    SceneMessages_RegisterReceiver(npc->block168, name, 5);
    npc->motion.angle = angle;
    {
        u16 a = angle;
        npc->facing8 = 7 & ((a + 0x70) >> 5);
        npc->facing4 = 3 & ((a + 0x60) >> 6);
    }
    npc->callbacks[5] = sub_081CFFAC;
    npc->callbacks[0] = sub_081D0864;
    npc->callbacks[1] = sub_081D0BC4;
    npc->callbacks[2] = sub_081D0BD0;
    npc->callbacks[3] = sub_081D0928;
    npc->callbacks[4] = sub_081D0B8C;
    sub_0802D614(manager, npc, model, 0);
    sub_0802D654(manager, npc);
    sub_0802E698(manager, npc);
    rig = &npc->rig;
    npc->fieldC = zero;
    rig->fieldC = 0;
    switch (Script_GetKeywordValue('i', 0)) {
        case 0:
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = 1;
            npc->field14 = 0;
            npc->field8 = 1;
            npc->state = 2;
            npc->update = sub_0802EA4C;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;
        case 1:
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = 0;
            npc->field14 = 0;
            npc->field8 = 1;
            npc->state = 1;
            npc->update = sub_0802EA08;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            DisplayAnimation_SetFlags(npc->blockD4);
            sub_0802D88C(npc);
            break;
        case 2:
            npc->field21B = 1;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = 0x12;
            npc->field14 = 0;
            npc->field8 = 1;
            npc->state = 0x10;
            npc->update = sub_081CEFCC;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;
        case 3:
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = 0x19;
            npc->field14 = 0;
            npc->field8 = 1;
            npc->state = 0x16;
            npc->update = sub_081CF84C;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;
        case 4:
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = 6;
            npc->field14 = 0;
            npc->field8 = 1;
            npc->state = 7;
            npc->update = sub_0802F0C0;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;
    }
    sub_0802DB78(npc, 0, 0, 0, 0, 0);
    npc->field1F4 = 16;
    npc->field1F6 = 20;
    npc->field1F8 = 16;
    npc->field1FC = 20;
    npc->field200 = 0x100;
    npc->field204 = 0x190;
    rig->handle = sub_08163E24();
    if (extra) {
        rig->attached = 1;
        sub_081D07D8(manager, npc);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneNpc_CreateFromScript);
#endif
