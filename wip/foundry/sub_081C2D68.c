#include "global.h"

#define SparkleEmitter_InitFromScript sub_081C2D68

s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void SceneMessages_RegisterReceiver(void *, int, int);
s32 Script_GetKeywordValue(u32, s32);
void Sound_PauseChannel12(s32);
void Sound_PlaySong(u32);
void Sprite_Init(void *, void *, u32);
void SparkleEmitter_UpdateEmitting(void *);
void SparkleEmitter_UpdateDraining(void *);

s32 SparkleEmitter_InitFromScript(void *p0, void *script)
{
    u8 *p = (u8 *)p0;
    u8 *anim = p + 0x718;
    u8 *particle;
    s32 i;

    *(void **)(p + 0x76C) = script;
    AnimData_Load(anim, 0xCC88);
    AnimData_SelectPaletteBank(anim, 0x255);

    particle = p + 0x18;
    for (i = 0; i <= 0x1F; i++) {
        u32 *flags = (u32 *)particle;
        Sprite_Init(particle, p + 0x718, 0);
        *flags |= 1;
        *flags |= (i & 1) ? 0x200 : 0x400;
        particle += 0x38;
    }

    SceneMessages_RegisterReceiver(p + 0x734, *(void **)(p + 0x76C), 10);
    {
        void (*fn)(void *) = SparkleEmitter_UpdateEmitting;
        *(void (**)(void *))(p + 0x77C) = fn;
    }
    *(u32 *)(p + 0x770) = 0;
    *(s32 *)(p + 0x778) = 0;

    {
        u32 t = (u32)Script_GetKeywordValue(0x64, 5);
        *(u16 *)(p + 0x774) = ((t << 29) + 0xA0000000) >> 24;
    }
    *(s16 *)(p + 0x776) = Script_GetKeywordValue(0x70, 0x40);
    *(s32 *)(p + 0x768) = Script_GetKeywordValue(0x63, 8);

    if (Script_GetKeywordValue(0x69, 1) != 0) {
        s32 v776 = *(s16 *)(p + 0x776);
        s32 v774 = *(s16 *)(p + 0x774);
        s32 v768 = *(s32 *)(p + 0x768);
        *(s16 *)(p + 0x776) = v776;
        *(s16 *)(p + 0x774) = v774;
        *(s32 *)(p + 0x768) = v768;
        if ((u32)v768 <= 14)
            AnimData_SelectPaletteBank(p + 0x718, (u32)v768 + 0x255);
        Sound_PlaySong(0x83);
        {
            void (*fn)(void *) = SparkleEmitter_UpdateEmitting;
            *(void (**)(void *))(p + 0x77C) = fn;
        }
        *(u32 *)(p + 0x770) = 0;
    } else {
        Sound_PauseChannel12(8);
        {
            void (*fn)(void *) = SparkleEmitter_UpdateDraining;
            *(void (**)(void *))(p + 0x77C) = fn;
        }
        *(u32 *)(p + 0x770) = 0;
    }
    return 0;
}
