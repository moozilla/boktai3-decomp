#include "global.h"

struct ScriptPanel {
    u8 pad0[0x930];
    void (*callback)(void);
};
void sub_081BCB14(void);

s32 ScriptPanel_IsCallbackActive(struct ScriptPanel *panel)
{
    if (panel->callback == sub_081BCB14)
        return 1;
    return 0;
}
