#include "global.h"
#include "render.h"
#include "ram.h"
struct Weather {
    u8 pad[24];
    u32 timer;
    u8 entered, state;
    u16 countdown;
    u8 enabled, flags;
    u16 light, phase;
    u8 variant, pad27;
    u16 duration, randomDuration;
    u8 pad2c[36];
    int bank;
};
extern u8 *gGameState;
extern u32 gEngineFlags;
extern u8 gUnk_03005430[];
extern u16 gUnk_0203B400[];
extern u32 gRandomIndex;
void Palette_SetBrightnessMode(int);
int Weather_TestFlags(int, int);
void Weather_SetRandomDuration(void *);
void Weather_EnterPhase(void *, int);
int sub_080177AC(void *, int, int, int);
int Mod(int, int);
void Sound_PlaySong(int);
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
static inline int randomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)((u8 *)gUnk_0203B400 + offset);
}
static inline void flags(void)
{
    int mask = 512;
    gDispCntShadow = mask | gDispCntShadow;
    {
        int bit = 4096;
        *(u32 *)(gGameState + 0x868) |= bit;
    }
}
static inline int light(struct Weather *p)
{
    int v = *(s16 *)(gGameState + 0x876);
    v += p->light;
    return v;
}
static inline int interrupted(void)
{
    int mask = 1024;
    return (gEngineFlags | gScenePauseFlags) & mask;
}
int Weather_ActiveStorm(struct Weather *p)
{
    if (p->entered) {
        p->entered = 0;
        if (!gStormTimer) {
            gStormTimer = p->duration;
            if (p->randomDuration)
                gStormTimer += Mod(randomValue(), p->randomDuration);
        }
        p->light = 0;
        flags();
        Palette_SetBrightnessMode(2);
    }
    if (!Weather_TestFlags(p->flags, 8)) {
        if (!p->countdown) {
            if (!(gEngineFlags & 1024) && gWeatherCollisionTimer > 0) {
                gGrayscaleControl = 5;
                Sound_PlaySong(103);
            }
            Weather_SetRandomDuration(p);
        } else {
            if (gUnk_03005430[14] > 2) {
                if (p->countdown == 5)
                    ScreenTransition_SendCommand(0, 2, 31, 31, 31, 8191, 0);
                else if (p->countdown == 3)
                    gWeatherCollisionTimer = 6;
            }
            p->countdown--;
        }
    }
    if (!Weather_TestFlags(p->flags, 1) && p->timer > 29) {
        p->light = light(p);
        if (p->light > 59 || interrupted()) {
            gStormTimer = 0;
            Weather_EnterPhase(p, 4);
            return 1;
        }
    }
    if (!Weather_TestFlags(p->flags, 2)) {
        if (gStormTimer <= 0) {
            Weather_EnterPhase(p, 4);
            gStormTimer = 0;
            return 1;
        } else
            gStormTimer--;
    }
    sub_080177AC(p, (randomValue() & 3) + 4, 1, 0);
    p->bank = 1 - p->bank;
    p->timer++;
    return 0;
}
