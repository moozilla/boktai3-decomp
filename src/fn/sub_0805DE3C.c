// MATCHING_CONCESSION: register-pinning
#include "scene_hud_display.h"
void SceneHud_DrawClock(u32 mode)
{
    u16 *output = SceneHud_GetMapCell(0, 20, 0);
    if (mode == 0) {
        {
            register u32 glyph asm("r2") = 0xF0BD;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            output++;
            glyph -= 12;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            glyph += 10;
            {
                register u32 stored asm("r0");
                asm("" : "+r"(glyph));
                stored = glyph;
                output[1] = stored;
            }
        }
        SceneHud_DrawDecimal(SunClock_GetBaseHour(), 10, 24, 0, mode);
        SceneHud_DrawDecimal(SunClock_GetBaseMinute(), 10, 27, 0, mode);
    } else if (mode == 1) {
        {
            register u32 glyph asm("r2") = 0xF07C;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            output++;
            glyph++;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            glyph++;
            {
                register u32 stored asm("r0");
                asm("" : "+r"(glyph));
                stored = glyph;
                output[1] = stored;
            }
        }
        SceneHud_DrawDecimal(gSunClock.hourA, 10, 24, 0, 0);
        SceneHud_DrawDecimal(gSunClock.minuteA, 10, 27, 0, 0);
    } else {
        {
            register u32 glyph asm("r2") = 0xF07C;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            output++;
            glyph++;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            glyph += 2;
            {
                register u32 stored asm("r0");
                asm("" : "+r"(glyph));
                stored = glyph;
                output[1] = stored;
            }
        }
        SceneHud_DrawDecimal(gSunClock.hourB, 10, 24, 0, 0);
        SceneHud_DrawDecimal(gSunClock.minuteB, 10, 27, 0, 0);
    }
}
