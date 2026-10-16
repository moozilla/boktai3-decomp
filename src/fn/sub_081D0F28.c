// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
struct Motion {
    u8 pad0[9];
    u8 angle;
    u8 padA[2];
    s16 x, y, z;
    u8 pad12[0x66];
};
struct SceneNpc {
    u8 pad0[2];
    u8 field2, field3, mode;
    u8 pad5;
    u8 state;
    u8 pad7;
    u8 field8, field9;
    u8 padA[2];
    u16 flagsC;
    u8 padE[2];
    u32 field10, field14;
    u8 field18, facing4, facing8, angle;
    u8 pad1C[0x40];
    struct Motion motion;
    u8 padD4[0x1F8];
    void *update;
};
struct Player {
    u8 pad0[0x30];
    s16 x, y, z;
};
extern void *gUnk_02000480;
extern struct SceneNpc *gUnk_02000274;
extern struct Player *gUnk_02000580[];
struct SceneNpc *sub_0802D6D0(void *manager, s32 name);
s32 Math_Atan2(s32 dx, s32 dz);
void sub_081CEE50(void);
/* Sets the motion angle and the derived 8- and 4-way facings. The facing math
   needs register pins and an empty asm barrier (register-pinning concession):
   agbcc otherwise ties the AND result to the shift instead of the constant. */
static inline void setAngle(struct SceneNpc *npc, s32 angle)
{
    u16 a;
    npc->motion.angle = angle;
    a = angle;
    {
        register s32 t asm("r2");
        register u32 m asm("r1");
        t = a + 0x70;
        asm("" : "+r"(t));
        t >>= 5;
        m = 7;
        m &= t;
        npc->facing8 = m;
    }
    {
        register s32 t asm("r0") = a + 0x60;
        u32 m;
        asm("" : "+r"(t));
        t >>= 6;
        m = 3;
        m &= t;
        npc->facing4 = m;
    }
    npc->angle = npc->motion.angle;
}
/* Registry routine 8918: puts scene NPC -n (default: the last one created,
   gUnk_02000274) into the scripted state (mode 0x10, state 14, update
   sub_081CEE50), then -p x y z, -d angle, -w face the player, -s sets flag 1. */
void SceneNpc_CommandFromScript(void)
{
    struct SceneNpc *npc;
    u8 mode;
    void (*update)(void);
    if (!gUnk_02000274)
        return;
    if (Script_SeekToKeyword('n')) {
        void *manager = gUnk_02000480;
        if (!manager)
            return;
        npc = sub_0802D6D0(manager, Script_GetKeywordValue('n', 0));
        if (!npc)
            return;
    } else
        npc = gUnk_02000274;
    mode = 0x10;
    npc->field2 = 0;
    npc->field3 = 0;
    npc->mode = mode;
    npc->field14 = 0;
    npc->field8 = 1;
    update = sub_081CEE50;
    npc->state = 14;
    npc->update = update;
    npc->field10 = 0;
    npc->field9 = 1;
    npc->field18 = 0;
    if (Script_SeekToKeyword('p')) {
        npc->motion.x = Script_GetValue();
        npc->motion.y = Script_GetValue();
        npc->motion.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('d'))
        setAngle(npc, Script_GetValue());
    if (Script_GetKeywordValue('w', 0) > 0) {
        struct Player *player = gUnk_02000580[0];
        s16 *pos = &npc->motion.x;
        setAngle(npc, Math_Atan2(player->x - pos[0], player->z - pos[2]));
    }
    if (Script_GetKeywordValue('s', 0) > 0)
        npc->flagsC |= 1;
}
