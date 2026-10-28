#include "global.h"
/* Provisional names and field interpretations; confidence low. */
#include "area_effect_link.h"
struct LinkSessionData {
    u8 unknown0[0x84];
    s8 playerKinds[5];
};
static inline struct LinkSessionData *LinkSession_GetData(void)
{
    if (!gLinkSession || gLinkSession->unavailable)
        return 0;
    return (struct LinkSessionData *)gLinkSession->sessionData;
}
s32 LinkSession_ClearFirstPlayerKind(void *unused, s32 value)
{
    struct LinkSessionData *data = LinkSession_GetData();
    s32 i;
    if (!data)
        return -1;
    for (i = 0; i < 5; i++) {
        if (data->playerKinds[i] == value) {
            data->playerKinds[i] = 0xFF;
            break;
        }
    }
    return 0;
}
