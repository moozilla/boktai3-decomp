#define ScriptPanel_IsCallbackActive sub_081BD914
#include "global.h"

struct ScriptPanel {
    u8 pad0[0x930];
    void (*callback)(void);
};
void sub_081BCB14(void);

s32 sub_081BD914(struct ScriptPanel *panel)
{
    if (panel->callback == sub_081BCB14)
        return 1;
    return 0;
}
