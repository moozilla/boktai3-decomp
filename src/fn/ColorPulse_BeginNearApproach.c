#include "global.h"
struct ColorPulse {
    u8 pad0[0x4B4];
    u16 timer4B4;
    u8 pad4B6[0xCD8 - 0x4B6];
    void (*update)(struct ColorPulse *);
    u8 padCDC[0xCF0 - 0xCDC];
    void (*callbackCF0)(struct ColorPulse *);
    u8 padCF4[0xD53 - 0xCF4];
    s8 approaches;
};
u8 Callback_InvokeActor(void *, void *);
void Sound_PlaySong(u32);
void Ivy_UpdatePositionTransition(struct ColorPulse *);

void ColorPulse_BeginNearApproach(struct ColorPulse *p)
{
    if (p->callbackCF0 && Callback_InvokeActor(p, p->callbackCF0))
        p->approaches++;
    if (p->approaches > 1) {
        p->approaches = 0;
        p->timer4B4 = 0;
        p->update = Ivy_UpdatePositionTransition;
        Sound_PlaySong(0x18F);
    }
}
