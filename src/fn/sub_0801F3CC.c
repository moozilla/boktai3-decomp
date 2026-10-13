// MATCHING_CONCESSION: register-pinning
#include "global.h"
void Sound_PlaySong(int);
void SceneObject_UpdateLowering(void);
void SceneObject_BeginLowering(u8 *s)
{
    Sound_PlaySong(0x152);
    s[0x39] = 0;
    s[0x3a] = 0;
    {
        u16 *flags = (u16 *)(*(u8 **)(s + 0x2c) + 6);
        int old = *flags;
        {
            int result = 2;
            result |= old;
            *flags = result;
        }
    }
    asm("" ::: "memory");
    *(u16 *)(*(u8 **)(s + 0x2c) + 6) |= 3;
    s[0x37] = 3;
    (*(u8 **)(s + 0x2c))[4] = ((*(u8 **)(s + 0x2c))[4] & 0xf0) | (s[0x3b] + 1);
    *(u16 *)(*(u8 **)(s + 0x2c)) &= ~1;
    *(void **)(s + 0x30) = SceneObject_UpdateLowering;
}
