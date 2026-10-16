#include "global.h"
#include "engine.h"
struct SceneNpc {
    u8 pad0[2];
    u8 field2, field3, mode;
    u8 pad5;
    u8 state;
    u8 pad7;
    u8 field8, field9;
    u8 padA[6];
    u32 field10, field14;
    u8 field18;
    u8 pad19[0xBB];
    u8 blockD4[0x1F8];
    void *update;
};
extern void *gUnk_02000480;
extern struct SceneNpc *gUnk_02000274;
struct SceneNpc *sub_0802D6D0(void *manager, s32 name);
void DisplayAnimation_SetFlags(void *, s32);
void sub_0802D88C(struct SceneNpc *);
void sub_081CEF3C(void);
/* Registry routine 6CBE: puts scene NPC -n (default: the last created) into
   mode 0x11, state 15 (update sub_081CEF3C) and resets its animation
   (tentative: stop and stand). */
void SceneNpc_StopFromScript(void)
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
    mode = 0x11;
    npc->field2 = 0;
    npc->field3 = 0;
    npc->mode = mode;
    npc->field14 = 0;
    npc->field8 = 1;
    update = sub_081CEF3C;
    npc->state = 15;
    npc->update = update;
    npc->field10 = 0;
    npc->field9 = 1;
    npc->field18 = 0;
    DisplayAnimation_SetFlags(npc->blockD4, 1);
    sub_0802D88C(npc);
}
