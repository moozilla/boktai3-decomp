// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "runtime.h"
#include "ram.h"

// Tentative result panel and drawable offsets.
struct Drawing {
    u8 pad[8];
    u32 flags;
    u8 padC[0x14];
    u16 x, y;
};
struct P {
    u8 pad[0x18];
    u32 timer;
    u8 state, entered;
    u8 pad1E[6];
    u8 result;
    u8 pad25[0x93];
    struct Drawing drawing;
};
extern u16 gUnk_0203B400[];
void Sound_PlayTrackedSong(int);
void Anim_LoadKeyframe(void *, void *, int, int);

void WirelessResultPanel_UpdateOutcomeReveal(struct P *p)
{
    if (p->entered) {
        int icon;
        int count;
        register struct Drawing *drawing asm("r6");
        u8 *result;
        u8 **save;
        p->entered = 0;
        {
            u8 *address = &p->result;
            int resultValue = *address;
            icon = 30;
            result = address;
            if (resultValue != 0) {
                icon = 32;
                if (resultValue == 1)
                    icon = 31;
            }
        }
        {
            u8 **address = &gSessionState;
            int maximum;
            asm("" : "+r"(address));
            count = *(u16 *)(*address + 0x14) + 1;
            maximum = 9999;
            asm("" : "+r"(maximum));
            save = address;
            asm("" : "+r"(save));
            if (count > maximum)
                count = maximum;
            *(u16 *)(*save + 0x14) = count;
        }
        {
            int resultValue = *result;
            if (resultValue == 0) {
                Sound_PlayTrackedSong(0xB0);
                {
                    int old = *(u16 *)(*save + 0x16);
                    count = old + 1;
                }
                {
                    int maximum = 9999;
                    if (count > maximum)
                        count = maximum;
                }
                {
                    u8 **address = &gSessionState;
                    *(u16 *)(*address + 0x16) = count;
                }
            } else if (resultValue == 1) {
                Sound_PlayTrackedSong(0xB1);
            } else {
                Sound_PlayTrackedSong(0xB1);
            }
        }
        drawing = &p->drawing;
        drawing->flags &= ~1;
        {
            u8 *descriptor = (u8 *)p + 0x50;
            int resource = icon;
            asm("" : "+r"(descriptor), "+r"(resource));
            Anim_LoadKeyframe(drawing, descriptor, resource, 0);
        }
        drawing->x = 0;
        drawing->y = 0;
    }
    if (p->timer <= 7) {
        struct Drawing *drawing = &p->drawing;
        u16 *table = gUnk_0203B400;
        register u32 *indexPointer asm("r2");
        register u32 index asm("r0");
        u32 mask;
        int value;
        int smallMask;
        indexPointer = &gRandomIndex;
        index = *indexPointer + 1;
        mask = 1023;
        index &= mask;
        *indexPointer = index;
        value = table[index];
        smallMask = 3;
        value &= smallMask;
        value -= 2;
        drawing->x = value;
        index++;
        index &= mask;
        *indexPointer = index;
        index = table[index];
        index &= smallMask;
        index -= 2;
        drawing->y = index;
    } else if (p->timer == 8) {
        register struct Drawing *drawing asm("r6") = &p->drawing;
        drawing->x = 0;
        drawing->y = 0;
    }
    if (p->timer > 29) {
        int state = 3;
        int zero = 0;
        p->state = state;
        p->entered = 1;
        p->timer = zero;
    } else {
        p->timer++;
    }
}
