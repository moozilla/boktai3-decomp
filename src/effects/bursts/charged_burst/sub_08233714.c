#define TimedDisplay_SetState ChargedBurst_SetState
#include "global.h"
struct TimedDisplay {
    u8 pad0[0x74];
    u32 flags;
    u8 pad78[0x20a - 0x78];
    u16 timer;
    void (*state)(void);
};
void sub_0823350C(struct TimedDisplay *);
void ChargedBurst_EmitSpark(struct TimedDisplay *);
void ChargedBurst_SetState(struct TimedDisplay *, void (*)(void));
void ChargedBurst_EmitTerminalDebris(void);
void Sound_PlaySong(u32);
void TimedDisplay_AdvanceIntro(struct TimedDisplay *s)
{
    sub_0823350C(s);
    ChargedBurst_EmitSpark(s);
    if (++s->timer > 31) {
        s->flags &= ~1;
        Sound_PlaySong(0x133);
        ChargedBurst_SetState(s, ChargedBurst_EmitTerminalDebris);
    }
}
