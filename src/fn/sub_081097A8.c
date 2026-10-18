#include "global.h"
struct TimedMessage {
    u8 unknown0[0x20];
    void (*update)(struct TimedMessage *);
    const u8 *followupScript;
    const u8 *script;
    u8 unknown2C[4];
    s32 window;
    u8 state, entering;
    u16 timer;
    u32 kind;
};
u8 Save_ToggleSelectedSlot(void);
void MessagePanel_WaitForDismiss(struct TimedMessage *);
void MessagePanel_WaitForInput(struct TimedMessage *);
void TimedMessage_SelectNextState(struct TimedMessage *p)
{
    if (Save_ToggleSelectedSlot()) {
        void (*update)(struct TimedMessage *) = MessagePanel_WaitForDismiss;
        u32 state = 7;
        p->update = update;
        p->entering = 1;
        p->state = state;
    } else {
        void (*update)(struct TimedMessage *) = MessagePanel_WaitForInput;
        u32 state = 8;
        p->update = update;
        p->entering = 1;
        p->state = state;
    }
}
