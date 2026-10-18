#define ScreenPhase_DampenMotion sub_081F022C
#define gRandomCursor gUnk_03005308
#define gRandomValues gUnk_0203B400
#include "global.h"
struct ScreenMotionPhase { u8 unknown0[0x1AE]; u16 duration; u8 unknown1B0[0xD0]; s32 velocity; u8 unknown284[0x33C]; u16 elapsed; u16 delay; };
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
void Sound_PlaySong(u32);
static inline s32 NextRandomByte(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    return *(const u8 *)&gUnk_0203B400[gUnk_03005308];
}
void sub_081F022C(struct ScreenMotionPhase *screen)
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
