#include "global.h"

struct MotionTweenObject {
    u8 unknown0[0x18];
    s32 coordinate;
    u8 unknown1C[0xC];
    s32 position;
    u8 unknown2C[0x580];
    s32 stepScale;
    u8 unknown5B0[4];
    s32 velocity;
    u8 unknown5B8[0x24];
    s32 value;
};
struct MotionTween {
    s32 targetValue;
    s32 unknown4;
    s32 targetCoordinate;
    s32 unknownC;
    u8 valueFrames, coordinateFrames, valueDelay, coordinateDelay;
};
s32 Div(s32, s32);
s32 MotionTween_Advance(struct MotionTweenObject *object, struct MotionTween *tween)
{
    s32 changed = 0;
    if (tween->valueDelay) {
        tween->valueDelay--;
    } else if (tween->valueFrames) {
        object->value += Div(tween->targetValue - object->value, tween->valueFrames);
        if (--tween->valueFrames == 0)
            object->value = tween->targetValue;
        changed = 1;
    }
    if (tween->coordinateDelay) {
        tween->coordinateDelay--;
    } else if (tween->coordinateFrames) {
        object->velocity +=
            Div(tween->targetCoordinate - (object->coordinate >> 12), tween->coordinateFrames) << 8;
        if (--tween->coordinateFrames == 0)
            object->position += (tween->targetCoordinate - (object->coordinate >> 12)) << 8;
        changed = 1;
    }
    object->stepScale = 0x3000;
    return changed;
}
