#include "global.h"
#define SessionMenu_IncrementFirstCounter sub_0810BB4C
/* Two saturating pairs in the session payload; meaning of the pairs unknown. */
struct SessionMenuCounters {
    u8 unknown0[0x154];
    u16 first[2];
    u16 second[2];
};
static inline u8 SessionMenu_IsCounterIndex(s32 index, s32 low, s32 high)
{
    if (index < low || index > high)
        return 0;
    return 1;
}
static inline void SessionMenu_SetFirstCounter(s32 value, struct SessionMenuCounters *p, s32 index)
{
    if (value > 20)
        value = 20;
    p->first[index] = value;
}
void sub_0810BB4C(struct SessionMenuCounters *p, s32 index)
{
    if (p && SessionMenu_IsCounterIndex(index, 0, 1)) {
        s32 value;
        p->first[index]++;
        value = p->first[index];
        SessionMenu_SetFirstCounter(value, p, index);
    }
}
