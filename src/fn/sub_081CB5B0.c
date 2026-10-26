// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"

void Actor_PlayAnimationEntry(void *, u32, u32, u32, u32);
void Palette_InterpolateBanks(void *dest, void *a, void *b, u32 amount, u32 count);
u32 PlayerParticleCloud_Create(u32 a, u32 b);
s32 PlayerParticleCloud_RequestStop(void);
void Sound_PlaySong(u32 song);
void sub_081D3620(void);
void Actor_OnHiddenIdleUpdate(void);

void sub_081CB5B0(u32 a, u8 *b, s32 c)
{
    if (b[9]) {
        u32 zero = 0;
        b[9] = zero;
        b[7] = zero;
        CpuSet(gPaletteBankData + 0x680, b + 0x1CC, 0x04000008);
        Actor_PlayAnimationEntry(b, 0, 2, 1, 4);
        b[0x18] = zero;
        *(u32 *)(b + 0x10) = zero;
    }
    switch (b[0x18]) {
        case 0: {
            u32 t = *(u32 *)(b + 0x10);
            if (t <= 0x80) {
                void *bank1 = gPaletteBankData + 0x680;
                void *bank2 = gPaletteBankData + 0x2E80;
                void *dest = b + 0x1CC;
                Palette_InterpolateBanks(dest, bank1, bank2, t, 7);
            } else {
                *(u32 *)(b + 0x10) = 0;
                b[0x18]++;
                PlayerParticleCloud_Create((u32)(b + 0x68), 1);
                sub_081D3620();
                Sound_PlaySong(0x5B5);
            }
            break;
        }
        case 1: {
            u32 zero;
            {
                u32 m = 0x100;
                u16 old = *(u16 *)(b + 0xE);
                zero = 0;
                asm("" : "+r"(zero));
                m |= old;
                *(u16 *)(b + 0xE) = m;
            }
            if (c > 0x12C) {
                CpuSet(gPaletteBankData + 0x680, b + 0x1CC, 0x04000008);
                PlayerParticleCloud_RequestStop();
                b[7] = 1;
                {
                    void (*callback)(void) = Actor_OnHiddenIdleUpdate;
                    b[6] = 1;
                    *(void (**)(void))(b + 0x2CC) = callback;
                }
                *(u32 *)(b + 0x10) = 0;
                b[9] = 1;
                b[0x18] = zero;
            }
            break;
        }
    }
}
