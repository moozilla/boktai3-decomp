#include "global.h"
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct SceneFade {
    u8 task[0x18];
    void *actor;
    struct PackedVec from, to;
    u32 effect;
    u8 unknown30[6];
    u16 wait;
    u16 volumeA, volumeB, volumeC;
};
void *RegistryF3E4_CreateHidden(s32, s32);
void SceneFade_BeginSoundFade(struct SceneFade *p);
static inline void SetVec(struct PackedVec *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
s32 sub_081C4EF0(struct SceneFade *p)
{
    struct PackedVec from, to;
    if (Script_SeekToKeyword('s')) {
        from.x = Script_GetValue();
        from.y = Script_GetValue();
        from.z = Script_GetValue();
    } else {
        SetVec(&from, 0x880, 0x200, 0x868);
    }
    if (Script_SeekToKeyword('g')) {
        to.x = Script_GetValue();
        to.y = Script_GetValue();
        to.z = Script_GetValue();
    } else {
        SetVec(&to, 0x580, 0x200, 0x780);
    }
    {
        s32 v = Script_SeekToKeyword('w');
        if (v)
            v = Script_GetValue();
        p->wait = v;
    }
    {
        s32 v = Script_SeekToKeyword('e');
        if (v)
            v = Script_GetValue();
        p->effect = v;
    }
    p->actor = RegistryF3E4_CreateHidden(3, 1);
    p->from = from;
    p->to = to;
    p->volumeA = 0x80;
    p->volumeB = 0x80;
    p->volumeC = 0x80;
    SceneFade_BeginSoundFade(p);
}
