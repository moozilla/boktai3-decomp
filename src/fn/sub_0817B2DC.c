#include "global.h"

#ifdef NONMATCHING
// fuzzy: 89.6%
// Differs: case 28 loads 3 before 2 in the ROM, the hoisted zero (r5) comes first, and i++ is
// reloaded into r4 and compared directly instead of reloaded again.
#include "scene_panel.h"
#include "scene_panel_commands.h"
struct BlinkCommand {
    u8 unknown0[6];
    u8 opcode, unknown7;
    u8 value;
};
struct BlinkActor {
    u8 unknown0[0xAC];
    u8 modeA, modeB, modeC, unknownAF;
    u8 active;
    u8 unknownB1[0xC4 - 0xB1];
    u32 timer;
    u8 unknownC8[0x114 - 0xC8];
    u8 level;
    u8 unknown115[3];
    struct ScenePanelReceiver receiver;
};
void sub_0817B2DC(struct BlinkActor *p)
{
    s32 i;
    struct ScenePanelReceiver *receiver;
    receiver = &p->receiver;
    i = 0;
    if (i < p->receiver.count[gUnk_0200047C->bank]) {
        do {
            struct ScenePanelCommandGlobals *globals = gUnk_0200047C;
            s32 op;
            struct BlinkCommand *command =
                (struct BlinkCommand *)receiver->commands[globals->bank][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            op = command->opcode;
            switch (op) {
                case 1:
                    p->modeA = 0;
                    p->modeB = 0;
                    p->modeC = 0;
                    p->timer = 0;
                    p->active = op;
                    break;
                case 5:
                    p->level = (command->value + 5) << 5;
                    p->modeA = 2;
                    p->modeB = 0;
                    p->modeC = 1;
                    p->timer = 0;
                    p->active = 1;
                    break;
                case 27:
                    p->modeA = 2;
                    p->modeB = 0;
                    p->modeC = 2;
                    p->timer = 0;
                    p->active = 1;
                    break;
                case 28:
                    p->modeA = 2;
                    p->modeB = 0;
                    p->modeC = 3;
                    p->timer = 0;
                    p->active = 1;
                    break;
            }
            i++;
        } while (i < receiver->count[gUnk_0200047C->bank]);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0817B2DC);
#endif
