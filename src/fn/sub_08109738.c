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
void TextWindow_Clear(u32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
u8 Save_CopySelectedStateToAlternateSlot(void);
void MessagePanel_WaitForInput(struct TimedMessage *);
void TimedMessage_SelectNextState(struct TimedMessage *);
void TimedMessage_ShowFollowup(struct TimedMessage *p)
{
    if (p->entering == 1) {
        TextWindow_Clear(p->window);
        TextWindow_SetScript(p->window, p->followupScript);
        TextWindow_Refresh(p->window, 4);
        TextWindow_StartPlayback(p->window);
        p->timer = 0;
        p->entering = 0;
    }
    if (p->timer == 120) {
        if (!Save_CopySelectedStateToAlternateSlot()) {
            void (*update)(struct TimedMessage *) = MessagePanel_WaitForInput;
            u32 state = 8;
            p->update = update;
            p->entering = 1;
            p->state = state;
        } else {
            void (*update)(struct TimedMessage *) = TimedMessage_SelectNextState;
            u32 state = 9;
            p->update = update;
            p->entering = 1;
            p->state = state;
        }
    } else {
        p->timer++;
    }
}
