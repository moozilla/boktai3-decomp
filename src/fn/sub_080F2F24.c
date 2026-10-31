#include "global.h"
struct ColorPulse {
    u8 pad0[0xCE0];
    void (*updateCE0)(struct ColorPulse *);
    u8 padCE4[0xCF0 - 0xCE4];
    void (*callbackCF0)(struct ColorPulse *);
    u8 padCF4[0xD53 - 0xCF4];
    s8 approaches;
};
u8 Callback_InvokeActor(void *, void *);
void Sound_PlaySong(u32);
void PhaseObject_UpdateEntryScatter(struct ColorPulse *);

/* The far-approach twin of ColorPulse_BeginNearApproach. */
void sub_080F2F24(struct ColorPulse *p)
{
    if (p->callbackCF0 && Callback_InvokeActor(p, p->callbackCF0))
        p->approaches++;
    if (p->approaches > 1) {
        p->approaches = 0;
        p->updateCE0 = PhaseObject_UpdateEntryScatter;
        Sound_PlaySong(0x16B);
    }
}
