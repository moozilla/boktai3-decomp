#include "global.h"
struct SceneNpc {
    u8 unknown0[4];
    u8 mode, commandPhase, unknown6, pending, entered;
    u8 unknown9[0x14 - 9];
    u32 counter;
    u8 unknown18[0x168 - 0x18];
    u8 trigger[0x227 - 0x168];
    u8 previousMode;
};
u32 Trigger_ConsumePending(void *, u32);
void SceneNpc_EnterTriggeredMode(void *context, struct SceneNpc *p)
{
    if (p->entered) {
        p->entered = 0;
        p->pending = 0;
        p->commandPhase = 16;
        Trigger_ConsumePending(p->trigger, 1);
    }
    p->previousMode = p->mode;
    if (p->pending) {
        p->mode = 10;
        p->counter = 0;
        p->entered = 1;
    }
}
