/* Provisional names and object interpretation; offsets follow static accesses. */
#include "global.h"
#include "ui/results/stage_results.h"
void StageResults_InitCursorPositions(struct StageResults *results)
{
    s32 i;
    for (i = 0; i <= 22; i++) {
        switch (i) {
            case 8:
                results->sprites[i].x = 0xb0;
                results->sprites[i].y = 0x18;
                break;
            case 9:
                results->sprites[i].x = 0xb8;
                results->sprites[i].y = 0x18;
                break;
            case 10:
                results->sprites[i].x = 0xc0;
                results->sprites[i].y = 0x18;
                break;
            case 6:
                results->sprites[i].x = 0xc8;
                results->sprites[i].y = 0x18;
                break;
            case 11:
                results->sprites[i].x = 0xd0;
                results->sprites[i].y = 0x18;
                break;
            case 12:
                results->sprites[i].x = 0xd8;
                results->sprites[i].y = 0x18;
                break;
            case 13:
                results->sprites[i].x = 0xe0;
                results->sprites[i].y = 0x18;
                break;
            case 14:
                results->sprites[i].x = 0xd0;
                results->sprites[i].y = 0x50;
                break;
            case 15:
                results->sprites[i].x = 0xd8;
                results->sprites[i].y = 0x50;
                break;
            case 16:
                results->sprites[i].x = 0xe0;
                results->sprites[i].y = 0x50;
                break;
            case 17:
                results->sprites[i].x = 0xd0;
                results->sprites[i].y = 0x68;
                break;
            case 18:
                results->sprites[i].x = 0xd8;
                results->sprites[i].y = 0x68;
                break;
            case 19:
                results->sprites[i].x = 0xe0;
                results->sprites[i].y = 0x68;
                break;
            case 20:
                results->sprites[i].x = 0xd0;
                results->sprites[i].y = 0x88;
                break;
            case 21:
                results->sprites[i].x = 0xd8;
                results->sprites[i].y = 0x88;
                break;
            case 22:
                results->sprites[i].x = 0xe0;
                results->sprites[i].y = 0x88;
                break;
            default:
                results->sprites[i].x = 0;
                results->sprites[i].y = 0;
                break;
        }
    }
}
