#include "global.h"
#include "render.h"
#define gScreenMoteCallbacks gUnk_086108EC
#define gEffectPauseFlags    gUnk_030053F4
#define gScenePauseFlags     gUnk_030053F0
#define gFrameCounter        gEnginePauseFlags
/* State options at 0x14 control updates while paused or throttled. */
struct ScreenMoteState {
    u8 unknown0[0x14];
    u16 options;
    u8 phase;
    s8 spriteIndex;
    u8 unknown18[8];
    u32 active;
};
struct ScreenMoteManager {
    u8 unknown0[0x51c];
    struct ScreenMoteState states[32];
};
extern u32 gUnk_030053F4, gUnk_030053F0;
extern void *gUnk_086108EC[];
s32 sub_08249240(void *, void *, void *);
s32 ScreenMote_UpdateManager(struct ScreenMoteManager *m)
{
    s32 i;
    u32 pauseMask = 0x200;
    u8 *fields = (u8 *)&m->states[0].options;
    struct ScreenMoteState *s = m->states;
    i = 31;
    do {
        if (*(u32 *)(fields + 12)) {
            if (((gUnk_030053F4 | gUnk_030053F0) & pauseMask) ||
                ((gUnk_030053F4 & 0x800) && (gEnginePauseFlags & 7))) {
                if (*(u16 *)fields & 1)
                    goto next;
            }
            sub_08249240(m, s, gUnk_086108EC[fields[2]]);
        }
    next:
        fields += 0x24;
        s++;
        i--;
    } while (i >= 0);
    return 0;
}
