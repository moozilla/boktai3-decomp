#define gRandomCursor gRandomIndex
#include "global.h"
#include "engine.h"
#include "ram.h"
struct ScreenMotionPhase {
    u8 unknown0[0x1AE];
    u16 duration;
    u8 unknown1B0[0xD0];
    s32 velocity;
    u8 unknown284[0x33C];
    u16 elapsed;
    u16 delay;
};
void Sound_PlaySong(u32);
static inline s32 NextRandomByte(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return *(const u8 *)&gRandomTable[gRandomIndex];
}
void ScreenPhase_DampenMotion(struct ScreenMotionPhase *screen)
{
    screen->elapsed = 0;
    screen->duration = 30;
    screen->velocity = (screen->velocity >> 1) + (screen->velocity >> 2);
    screen->delay = 10;
    if (NextRandomByte() > 0xC4)
        Sound_PlaySong(0x587);
    else
        Sound_PlaySong(0x13E);
}
