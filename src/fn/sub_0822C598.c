#include "global.h"
/* Solar exposure tick, called from Solar_UpdateMeasurement after each
 * sample: accumulates exposure totals in the game-state block (offsets
 * 0x504, 0x520, 0x528, 0x77C) every 64 frames. Field roles are tentative. */
struct Obj {
    u8 pad[0x1e];
    u16 gain;
    u8 pad20[4];
    u16 frame;
    u16 idle;
    u16 frac;
};
extern u8 *gGameState;
extern u8 *gUnk_02000580;
extern u32 gEngineFlags;
extern u16 gPadState;
void Solar_SaveExposureTimestamp(void);
s32 SolarFruit_GetInventorySlot(s32);
s32 Div(s32, s32);
void sub_0822D2E0(s32);
void sub_0822C4D8(struct Obj *);

static inline u32 Test(u32 *p, u32 mask)
{
    return *p & mask;
}
#define GS32(o) (*(s32 *)(gGameState + (o)))
#define GS16(o) (*(s16 *)(gGameState + (o)))

void sub_0822C598(struct Obj *obj)
{
    if (gUnk_02000580 == NULL)
        return;
    if (gEngineFlags & 0x1800)
        return;
    if ((gEngineFlags & 0x200) || *(s32 *)(gUnk_02000580 + 0x1c) == 2) {
        s32 *p = &GS32(0x504);
        if (*p <= 29999) {
            if (GS16(0x874) > 0 && !(obj->frame & 0x3f)) {
                if ((*p += obj->gain) > 29999) {
                    GS32(0x504) = 30000;
                    Solar_SaveExposureTimestamp();
                }
            }
        } else if (GS16(0x876) > 2) {
            Solar_SaveExposureTimestamp();
        }
    } else {
        if (gPadState)
            obj->idle = 0;
        else if (obj->idle <= 0x383)
            obj->idle++;
        if (GS16(0x874) > 0 && !(obj->frame & 0x3f)) {
            if (obj->idle <= 0x383) {
                {
                    s32 *p = &GS32(0x520);
                    if (*p <= 0x7FFFFFFE) {
                        s32 t = *p + GS16(0x876);
                        if (t < 0)
                            *p = 0x7FFFFFFF;
                        else
                            *p = t;
                    }
                }
                if (GS32(0x77C) <= 9998) {
                    u32 q;
                    if (gUnk_02000580 != NULL && Test((u32 *)(gUnk_02000580 + 0x444), 0x80000))
                        obj->frac += ((GS16(0x876) >> 1) + 5) * 2;
                    else
                        obj->frac += (GS16(0x876) >> 1) + 5;
                    if (GS16(0x50) > 7 && SolarFruit_GetInventorySlot(GS16(0x54)) == 6) {
                        s32 m = Div(GS16(0x40), 10) + 2;
                        if (m > 7)
                            m = 7;
                        obj->frac *= m;
                    }
                    q = obj->frac >> 4;
                    obj->frac -= q << 4;
                    if ((GS32(0x77C) += q) > 9999)
                        GS32(0x77C) = 9999;
                }
                {
                    s32 *p = &GS32(0x528);
                    if (*p <= 0x7FFFFFFE) {
                        s32 t = *p + GS16(0x876);
                        if (t < 0)
                            *p = 0x7FFFFFFF;
                        else
                            *p = t;
                        GS32(0x52C)++;
                    }
                }
                if (!(obj->frame & 0x7f))
                    sub_0822D2E0(GS16(0x876));
            }
            {
                s32 *p = &GS32(0x504);
                if (*p <= 29999) {
                    if ((*p += obj->gain) > 29999) {
                        GS32(0x504) = 30000;
                        Solar_SaveExposureTimestamp();
                    }
                }
            }
        }
        sub_0822C4D8(obj);
    }
    obj->frame++;
}
