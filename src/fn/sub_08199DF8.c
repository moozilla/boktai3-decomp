#define CannonBattery_CreateCannon sub_08199DF8
#define CannonBattery_InitCannon   sub_08199CF4
#include "global.h"
struct CannonBatteryPosition {
    s16 x, y, z, unk6;
};
struct CannonBatterySprite {
    u32 flags;
    u8 unk4[3];
    u8 mode;
    u8 unk8[8];
    u16 frame;
    u8 unk12[0xA];
};
struct CannonBatteryCannon {
    u32 update, destroy;
    u8 animation[0x1C];
    struct CannonBatterySprite sprite;
    struct CannonBatteryPosition position;
    u8 unk48[8];
    u8 collider[0x54];
    struct CannonBatteryPosition initialPosition;
    struct CannonBatteryPool *owner;
    u8 unkB0[0x20];
    u16 unkD0;
    u8 unkD2;
    u8 state;
};
struct CannonBatteryPool {
    u8 unk0[0x1C];
    u16 used;
    u8 unk1E[0xA];
    struct CannonBatteryCannon cannons[16];
};
void sub_08199CF4(struct CannonBatteryCannon *, s32, s32, s32, s32);
struct CannonBatteryCannon *sub_08199DF8(struct CannonBatteryPool *pool, s32 x, s32 y, s32 z,
                                         s32 flip)
{
    struct CannonBatteryCannon *cannon = NULL;
    s32 i;
    struct CannonBatteryCannon *result;
    for (i = 0; i < 16; i++) {
        if (!((pool->used >> i) & 1)) {
            cannon = &pool->cannons[i];
            pool->used |= 1 << i;
            break;
        }
    }
    if (cannon) {
        cannon->owner = pool;
        sub_08199CF4(cannon, x, y, z, flip);
        result = cannon;
    } else {
        result = NULL;
    }
    return result;
}
