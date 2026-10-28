#include "resource_station.h"
#include "render.h"
// MATCHING_CONCESSION: register-pinning
s32 Div(s32, s32);
void Anim_LoadKeyframe(void *, void *, u16, u32);
static inline s32 truncateScale(s32 value)
{
    s32 scaled = value;
    if (scaled >= 0)
        scaled >>= 5;
    else
        scaled = -((-scaled) >> 5);
    return scaled;
}
struct ResourceStationDigitsState {
    s32 digits[4];
    struct ResourceStationPosition screen;
};
void ResourceStation_UpdateDigits(struct ResourceStation *station)
{
    register struct ResourceStation *object asm("r8") = station;
    if (*(u16 *)&object->nearby) {
        struct ResourceStationDigitsState local;
        s32 remainder, height;
        s32 depth;
        struct ResourceStationPosition *position;
        u32 *flags;
        void *graphics;
        struct ResourceStationDigit *animation;
        struct ResourceStationPosition *destination;
        s32 *digit;
        register s32 i asm("r5");
        if (gGameState->unk77C > 9999)
            gGameState->unk77C = 9999;
        local.digits[0] = Div(gGameState->unk77C, 1000);
        remainder = gGameState->unk77C - local.digits[0] * 1000;
        local.digits[1] = Div(remainder, 100);
        remainder -= local.digits[1] * 100;
        local.digits[2] = Div(remainder, 10);
        remainder -= local.digits[2] * 10;
        local.digits[3] = remainder;
        {
            struct ResourceStationPosition *screenPoint = &local.screen;
            s32 x;
            s32 z;
            position = &object->sprite.position;
            x = object->sprite.position.x;
            z = position->z;
            screenPoint->x = truncateScale((x - z) * 3);
            {
                s32 sum = x + z;
                depth = truncateScale(sum * 3);
            }
            height = truncateScale(position->y * 3);
            {
                s32 screenY = depth - height;
                depth += height;
                screenPoint->x = (u16)screenPoint->x -
                                 (u16)(*(struct ResourceStationPosition *)gCameraOrigin).x + 120;
                screenPoint->y =
                    screenY - (u16)(*(struct ResourceStationPosition *)gCameraOrigin).y + 80;
                depth -= (u16)(*(struct ResourceStationPosition *)gCameraOrigin).z;
                screenPoint->z = depth;
            }
        }
        {
            register struct ResourceStationDigitsState *read asm("r1") = &local;
            u32 x = (u16)read->screen.x;
            x -= 16;
            {
                struct ResourceStationPosition *field = &local.screen;
                asm("" : "+r"(field));
                field->x = x;
                field->y -= 44;
            }
        }
        {
            u32 offset = 0x60;
            asm("" : "+r"(offset));
            graphics = (u8 *)object + offset;
        }
        destination = &object->digits[0].position;
        digit = local.digits;
        animation = &object->digits[0];
        flags = &object->digits[0].flags;
        i = 3;
        do {
            *flags &= ~1;
            {
                register s32 *read asm("r1") = digit + 1;
                digit = read;
                read--;
                {
                    u32 frame = *read++ + 30;
                    asm("" : : "r"(read));
                    Anim_LoadKeyframe(animation, graphics, frame, 1);
                }
            }
            *destination = local.screen;
            {
                register struct ResourceStationDigitsState *read asm("r1") = &local;
                u32 x = (u16)read->screen.x + 8;
                register struct ResourceStationPosition *field asm("r1") = &local.screen;
                asm("" : "+r"(field));
                field->x = x;
            }
            destination = (void *)((u8 *)destination + 0x60);
            animation++;
            flags = (void *)((u8 *)flags + 0x60);
        } while (--i >= 0);
    } else {
        u32 mask = 1;
        u32 *flags = &object->digits[0].flags;
        register s32 i asm("r5") = 3;
        do {
            *flags |= mask;
            flags = (void *)((u8 *)flags + 0x60);
        } while (--i >= 0);
    }
}
