#define TimedMessage_ShowNotice sub_081096CC
#define TimedMessage_SelectTextIndex LinkConnectionMenu_GetErrorMessageIndex
#define TimedMessage_ShowFollowup sub_08109738
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
s32 LinkConnectionMenu_GetErrorMessageIndex(void *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
void Sound_PlaySong(u32);
void sub_08109738(struct TimedMessage *);
void sub_081096CC(struct TimedMessage *p)
{
    if (p->entering == 1) {
        TextWindow_Clear(p->window);
        TextWindow_SetScript(p->window, p->script);
        TextWindow_Refresh(p->window, LinkConnectionMenu_GetErrorMessageIndex(p));
        TextWindow_StartPlayback(p->window);
        Sound_PlaySong(0x123);
        p->timer = 0;
        p->entering = 0;
    }
    if (p->timer > 120) {
        void (*update)(struct TimedMessage *) = sub_08109738;
        u32 state = 6;
        p->update = update;
        p->entering = 1;
        p->state = state;
    }
    p->timer++;
}
