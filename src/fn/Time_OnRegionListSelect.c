#include "global.h"
#include "runtime.h"
struct RegionEntry {
    u16 group;
    u16 name;
    s32 longitude;
    s32 latitude;
};
struct RegionMenu {
    u8 pad0[0x26];
    s8 selected;
    u8 pad27[0x280 - 0x27];
    struct RegionEntry entries[1];
};
struct TimeState {
    u8 pad0[0x18];
    u32 region;
};
extern u8 gSunClock[];
void Time_CalculateSunriseSunset(s32 latitude, s32 longitude, s32 timezone);
void Save_WriteHeader(void);
void Time_OnRegionListSelect(struct RegionMenu *menu)
{
    Time_CalculateSunriseSunset(menu->entries[menu->selected].latitude,
                                menu->entries[menu->selected].longitude, 0);
    gSunClock[9] = 0;
    {
        s32 sel = menu->selected;
        ((struct TimeState *)gSessionState)->region = sel;
    }
    Save_WriteHeader();
}
