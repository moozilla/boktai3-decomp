#include "global.h"
#include "engine.h"
struct RegionEntry {
    u16 group;
    u16 name;
    s32 longitude;
    s32 latitude;
};
struct TimeState {
    u8 pad0[0x18];
    u32 region;
};
extern struct TimeState *gSessionState;
extern u8 gSunClock[];
s32 Script_GetValueSafe(void);
u8 *Script_SetPc(u8 *pc);
s32 Script_ReadValue(const void *);
void Time_CalculateSunriseSunset(s32 latitude, s32 longitude, s32 timezone);
/* Registry routine 7978: -d points at a region table (groups of entries:
   group id or -1 to end, count, then per entry a name string, longitude and
   latitude; at most 64 entries). Computes sunrise and sunset for the saved
   region (1-53, else 6). */
void Time_OnLoadSave(void)
{
    struct RegionEntry entries[64];
    s32 n, k, count, group;
    u32 region;
    u8 *pc;
    if (!Script_SeekToKeyword('d'))
        return;
    Script_SetPc((u8 *)Script_GetValueSafe());
    n = 0;
    while (n <= 64 && Script_GetPc() && (group = Script_GetValue()) != -1) {
        count = Script_GetValue();
        for (k = 0; k < count && (pc = Script_GetPc()); n++, k++) {
            entries[n].group = group;
            entries[n].name = Script_ReadValue(pc);
            entries[n].longitude = Script_GetValue();
            entries[n].latitude = Script_GetValue();
        }
    }
    region = gSessionState->region;
    if (region - 1 > 52) {
        gSessionState->region = 6;
        region = 6;
    }
    Time_CalculateSunriseSunset(entries[region].latitude, entries[region].longitude, 0);
    gSunClock[9] = 0;
}
