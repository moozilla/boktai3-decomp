#include "global.h"
struct Slot7Advance {
    u8 unknown0[0xAF];
    u8 finished, unknownB0, initialize;
    u8 unknownB2[0x268 - 0xB2];
    s32 position;
    u8 unknown26C[0x294 - 0x26C];
    s32 limit;
    u8 unknown298[0x398 - 0x298];
    s32 phase;
};
void ActorDisplay_UpdateScreenAnchor(void *);
void ActorDisplay_UpdateForegroundSprite(void *);
void ActorDisplay_UpdateAccentSprite(void *);
// confidence=low: advances a display coordinate by four until the configured limit.
#define SingletonSlot7_UpdateCoordinateAdvance sub_081EEDDC
void SingletonSlot7_UpdateCoordinateAdvance(struct Slot7Advance *p)
{
    if (p->initialize) {
        p->initialize = 0;
        p->phase = 0;
    }
    p->position += 4;
    if (p->limit < p->position)
        p->finished = 1;
    ActorDisplay_UpdateScreenAnchor(p);
    ActorDisplay_UpdateForegroundSprite(p);
    ActorDisplay_UpdateAccentSprite(p);
}
