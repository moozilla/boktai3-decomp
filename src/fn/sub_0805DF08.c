// MATCHING_CONCESSION: register-pinning
#include "scene_hud_display.h"
void SceneHud_DrawNumericStatus(u32 mode)
{
    u16 *output = SceneHud_GetMapCell(0, 20, 1);
    if (mode == 0) {
        {
            register u32 glyph asm("r2") = 0xF0C0;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            output++;
            glyph -= 5;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            output++;
            glyph -= 9;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            output++;
            glyph -= 0x51;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            glyph -= 0x60;
            {
                register u32 stored asm("r0");
                asm("" : "+r"(glyph));
                stored = glyph;
                output[1] = stored;
            }
        }
        {
            struct GameState *state = gGameState;
            u32 offset;
            const s16 *field;
            s32 value;
            offset = (u32) & ((struct GameState *)0)->unk7B0;
            field = (const s16 *)((const u8 *)state + offset);
            value = *field;
            asm("" ::"r"(offset));
            SceneHud_DrawDecimal(value, 10000, 24, 1, mode);
        }
    } else if (mode == 1) {
        {
            register u32 glyph asm("r2") = 0xF0BA;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            output++;
            glyph += 0x16;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            output++;
            glyph -= 0x15;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            output++;
            glyph -= 0x5A;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            glyph -= 0x41;
            {
                register u32 stored asm("r0");
                asm("" : "+r"(glyph));
                stored = glyph;
                output[1] = stored;
            }
        }
        SceneHud_DrawDecimal(((struct SceneHudNumericView *)gGameState)->value50C, 10000, 24, 1,
                             mode);
    } else {
        {
            register u32 glyph asm("r2") = 0xF0B5;
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
            output++;
            glyph += 5;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            output++;
            glyph += 4;
            {
                u32 stored;
                stored = glyph;
                asm("" ::"r"(output), "r"(stored));
                output[0] = stored;
            }
            glyph -= 0x5E;
            {
                register u32 stored asm("r0");
                asm("" : "+r"(glyph));
                stored = glyph;
                output[1] = stored;
            }
        }
        {
            struct GameState *state = gGameState;
            u32 offset;
            const s32 *field;
            s32 value;
            offset = (u32) & ((struct GameState *)0)->unk77C;
            field = (const s32 *)((const u8 *)state + offset);
            value = *field;
            asm("" ::"r"(offset));
            SceneHud_DrawDecimal(value, 10000, 24, 1, 0);
        }
    }
}
