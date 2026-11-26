#include "engine/shared/monster_particle_random.h"
#include "engine/shared/global.h"
#include "engine/shared/runtime.h"
#include "engine/shared/engine_globals.h"
#include "engine/shared/ram.h"

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
void Sound_PlayTrackedSong(int);
void Anim_LoadKeyframe(void *, void *, int, int);

void WirelessResultPanel_UpdateOutcomeReveal(struct P *p)
{
    struct Drawing *drawing;
    if (p->entered) {
        int icon;
        int count;
        u8 *result;
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
        count = *(u16 *)(gSessionState + 0x14) + 1;
        if (count > 9999)
            count = 9999;
        else if (count < 0)
            count = 0;
        *(u16 *)(gSessionState + 0x14) = count;
        if (*result == 0) {
            Sound_PlayTrackedSong(0xB0);
            count = *(u16 *)(gSessionState + 0x16) + 1;
            if (count > 9999)
                count = 9999;
            else if (count < 0)
                count = 0;
            *(u16 *)(gSessionState + 0x16) = count;
            // concession-ok(duplicate-branches): the ROM keeps the result-1 test and both calls, so the original had separate arms for the two non-win results that play the same jingle
        } else if (*result == 1) {
            Sound_PlayTrackedSong(0xB1);
        } else {
            Sound_PlayTrackedSong(0xB1);
        }
        drawing = &p->drawing;
        drawing->flags &= ~1;
        {
            u8 *descriptor = (u8 *)p + 0x50;
            u16 keyframe = icon;
            Anim_LoadKeyframe(drawing, descriptor, keyframe, 0);
        }
        drawing->x = 0;
        drawing->y = 0;
    }
    if (p->timer <= 7) {
        drawing = &p->drawing;
        drawing->x = (ParticleRandom_ReadDirectSample(gRandomTable) & 3) - 2;
        drawing->y = (ParticleRandom_ReadDirectSample(gRandomTable) & 3) - 2;
    } else if (p->timer == 8) {
        drawing = &p->drawing;
        drawing->x = 0;
        drawing->y = 0;
    }
    if (p->timer > 29) {
        int state = 3;
        p->state = state;
        p->entered = 1;
        p->timer = 0;
    } else {
        p->timer++;
    }
}
