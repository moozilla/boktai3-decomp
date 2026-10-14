#include "global.h"
struct Actor {
    u8 pad0[0x3b9];
    u8 command;
    u8 pad3ba[2];
    int argument;
    u8 pad3c0[0x61c];
    void *handle;
};
struct Actor *sub_0802140C(int);
int Script_GetKeywordValue(int, int);
void sub_08022AF4(void *);
void BackgroundFlash_BeginAlternating(void *);
void BackgroundFlash_BeginAppearance(void *);
extern u32 gUnk_0300523C;
void sub_08013B74(void *);
void Collider_Remove(void *);
void Anim_Remove(void *);
void SceneActors_Destroy(void *);
int sub_08028F48(u8 *p)
{
    sub_08013B74(p + 0x2a0);
    Collider_Remove(p + 0x1cc);
    Anim_Remove(p + 0x240);
    SceneActors_Destroy(p);
    return 0;
}
