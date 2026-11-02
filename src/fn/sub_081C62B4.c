#include "challenge_result.h"

extern const u16 gUnk_0824FA00[12];
s32 Div(s32, s32);
void *memcpy(void *dst, const void *src, unsigned long bytes);
s32 Mod(s32, s32);

union DigitLayoutSource {
    s32 hours;
    u16 *firstY;
};

void sub_081C62B4(struct ChallengeTimeResult *result)
{
    u16 positions[12];
    union DigitLayoutSource source;
    s32 minutes;
    s32 seconds;
    s32 i;

    memcpy(positions, gUnk_0824FA00, sizeof(positions));
    source.hours = Div(result->elapsedFrames, 216000);
    minutes = Div(result->elapsedFrames, 3600) - source.hours * 60;
    seconds = Div(result->elapsedFrames, 60) - source.hours * 3600 - minutes * 60;
    if (source.hours > 99) {
        ChallengeResult_LoadDigit(&result->digits[5], &result->digitGraphics, 9);
        ChallengeResult_LoadDigit(&result->digits[4], &result->digitGraphics, 9);
        ChallengeResult_LoadDigit(&result->digits[3], &result->digitGraphics, 5);
        ChallengeResult_LoadDigit(&result->digits[2], &result->digitGraphics, 9);
        ChallengeResult_LoadDigit(&result->digits[0], &result->digitGraphics, 5);
        ChallengeResult_LoadDigit(&result->digits[2], &result->digitGraphics, 9);
    } else {
        ChallengeResult_LoadDigit(&result->digits[4], &result->digitGraphics,
                                  Mod(source.hours, 10));
        source.hours = Div(source.hours, 10);
        ChallengeResult_LoadDigit(&result->digits[5], &result->digitGraphics, source.hours);
        ChallengeResult_LoadDigit(&result->digits[2], &result->digitGraphics, Mod(minutes, 10));
        minutes = Div(minutes, 10);
        ChallengeResult_LoadDigit(&result->digits[3], &result->digitGraphics, minutes);
        ChallengeResult_LoadDigit(&result->digits[0], &result->digitGraphics, Mod(seconds, 10));
        seconds = Div(seconds, 10);
        ChallengeResult_LoadDigit(&result->digits[1], &result->digitGraphics, seconds);
    }
    {
        u16 *sourceY;
        s16 *destinationX;
        u16 *sourceX;
        u8 *widgetOffset;
        u16 *firstY;

        firstY = &positions[1];
        source.firstY = firstY;
        sourceY = source.firstY;
        destinationX = &result->digits[0].position.x;
        sourceX = firstY - 1;
        widgetOffset = (u8 *)result;
        for (i = 0; i < 6; i++) {
            *destinationX = *sourceX + 0x90;
            ((struct ChallengeTimeResult *)widgetOffset)->digits[0].position.y = 0x38 + *sourceY;
            sourceY += 2;
            destinationX += sizeof(struct SceneInputWidget) / sizeof(*destinationX);
            sourceX += 2;
            widgetOffset += sizeof(struct SceneInputWidget);
        }
    }
}
