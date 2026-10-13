#include "global.h"
void Sound_PlaySong(int);
void SceneObject_UpdateRaising(void);
void SceneObject_BeginRaising(u8 *s)
{
    Sound_PlaySong(0x152);
    s[0x39] = 0;
    s[0x3a] = 0;
    {
        int two = 2;
        s[0x37] = two;
    }
    *(void **)(s + 0x30) = SceneObject_UpdateRaising;
}
