// MATCHING_CONCESSION: register-pinning
#include "global.h"
void Sound_PlaySong(int);
void Sound_StopSong(int);
void SceneObject_UpdateLowered(void);
void sub_08068124(void *, int, void *, int, int, int);
void SceneObject_UpdateLowering(u8 *s)
{
    struct {
        s16 x, y, z;
    } v;
    int clear;
    int one;
    {
        int next = *(u16 *)(s + 0x80) - 15;
        clear = 0;
        asm("" : "+r"(clear) : "r"(next));
        *(u16 *)(s + 0x80) = next;
    }
    if (*(s16 *)(s + 0x80) < 0) {
        *(u16 *)(s + 0x80) = clear;
        Sound_StopSong(0x152);
        Sound_PlaySong(0x153);
        *(u16 *)(s + 0x82) = 30;
        {
            one = 1;
            s[0x37] = one;
        }
        {
            int pose = s[0x84];
            s[0x39] = pose;
        }
        s[0x3a] = 0;
        *(void **)(s + 0x30) = SceneObject_UpdateLowered;
        {
            struct {
                s16 x, y, z;
            } zero = {0, 0, 0};
            v.z = zero.z;
            *(u32 *)&v = clear;
        }
        one &= s[0x38];
        if (one) {
            *(u32 *)&v = 128;
            sub_08068124(s + 0x70, 0, &v, 5, 2, clear);
        } else {
            v.z = 128;
            sub_08068124(s + 0x70, 0, &v, 5, 2, one);
        }
    }
    {
        u16 *offset = (u16 *)(s + 0x80);
        register int base asm("r1");
        base = *(u16 *)(s + 0x7a);
        base += *offset;
        *(u16 *)(s + 0x72) = base;
    }
}
