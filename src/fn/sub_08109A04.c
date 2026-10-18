#define MessagePanel_InitFromScript   sub_08109A04
#define MessagePanel_ResetDisplay     sub_08109948
#define MessagePanel_SetupBackground LinkConnectionMenu_LoadBackground
#define MessagePanel_CreateTextWindow LinkConnectionMenu_CreateTextWindow
#include "global.h"
struct MessagePanel {
    u8 unk0[0x20];
    void (*state)(struct MessagePanel *);
    u32 script;
    u32 closeScript;
    u32 flags;
    u32 window;
    u8 mode;
    u8 entering;
    u16 timer;
    u32 unk38;
    u32 key;
    u32 unk40;
    u32 event;
    u32 index;
};
s32 Script_SeekToKeyword(u32);
s32 Script_GetKeywordValue(u32, s32);
u32 Script_GetTextSourcePc(void);
void LinkConnectionMenu_LoadBackground(struct MessagePanel *);
void LinkConnectionMenu_CreateTextWindow(struct MessagePanel *);
void sub_08109948(struct MessagePanel *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
s32 sub_08109A04(struct MessagePanel *p)
{
    if (Script_SeekToKeyword('r')) {
        p->script = Script_GetTextSourcePc();
        if (!p->script)
            return -1;
    }
    if (Script_SeekToKeyword('d')) {
        p->closeScript = Script_GetTextSourcePc();
        if (!p->closeScript)
            return -1;
    }
    p->key = Script_GetKeywordValue('k', 0);
    p->event = Script_GetKeywordValue('e', 0);
    p->index = Script_GetKeywordValue('i', 0);
    LinkConnectionMenu_LoadBackground(p);
    LinkConnectionMenu_CreateTextWindow(p);
    {
        void (*state)(struct MessagePanel *) = sub_08109948;
        u32 zero = 0;
        p->state = state;
        p->entering = 1;
        p->mode = zero;
        if (Script_GetKeywordValue('f', 0))
            ScreenTransition_SendCommand(2, 4, 4, 4, 4, 0xFFFF, zero);
    }
    p->flags |= 1;
    return 1;
}
