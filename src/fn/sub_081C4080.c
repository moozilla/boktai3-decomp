#include "global.h"
#include "engine.h"
#include "ram.h"
struct F3E4Actor {
    u8 pad0[0x18];
    u8 animData[0x1C];
    u8 sprite[7];
    u8 field3B;
    u8 pad3C[0x14];
    s16 x, y, z;
    u8 pad56[0xA];
    u8 receiver[0x38];
    struct State {
        u16 mode;
        u8 pad2[2];
        u16 field4, field6;
        struct StateInner {
            u32 field0;
            u8 pad4[0x24];
            u8 field28, field29;
            u8 pad2A[0xA];
            u32 field34;
        } inner;
    } state;
    u32 fieldD8;
    u8 padDC[0x14];
    s32 name;
    void (*update)(void);
    u32 fieldF8;
};
void RegistryF3E4_OnTaskUpdate(void *);
void RegistryF3E4_OnTaskDestroy(void *);
void SceneMessages_RegisterReceiver(void *, s32, s32);
void RegistryF3E4_SetHidden(struct F3E4Actor *);
void RegistryF3E4_UpdateVisibleIdle(void);
static inline void fill32(void *dst, u32 *tmp, u32 control)
{
    *tmp = 0;
    CpuSet(tmp, dst, control);
}
static inline void setUpdate(void (*fn)(void), void (**slot)(void))
{
    *slot = fn;
}
/* Registry routine F3E4: creates its task (gRegistryF3E4Actor, sprite 0x3641) for
   the given name: -p x y z (default 0), -c palette bank (+0x4B), registers it
   for scene messages, and -i nonzero starts it via RegistryF3E4_SetHidden, else idles
   with update RegistryF3E4_UpdateVisibleIdle (tentative). */
struct F3E4Actor *RegistryF3E4_Create(u32 name)
{
    struct F3E4Actor **current = &gRegistryF3E4Actor;
    struct F3E4Actor *p = Task_Create(8, 0xFC);
    u32 zero;
    if (p)
        Task_SetCallbacks(p, RegistryF3E4_OnTaskUpdate, RegistryF3E4_OnTaskDestroy);
    *current = p;
    p->name = name;
    AnimData_Load(p->animData, 0x3641);
    Sprite_Init(p->sprite, p->animData, 2);
    p->field3B = 2;
    if (Script_SeekToKeyword('p')) {
        p->x = Script_GetValue();
        p->y = Script_GetValue();
        p->z = Script_GetValue();
    } else
        fill32(&p->x, &zero, 0x05000002);
    if (Script_SeekToKeyword('c'))
        AnimData_SelectPaletteBank(p->animData, Script_GetValue() + 0x4B);
    p->state.mode = 2;
    p->state.field4 = 0;
    p->state.field6 = 0;
    {
        struct StateInner *in = &p->state.inner;
        in->field0 = 0;
        in->field29 = 0;
        in->field28 = 0;
        in->field34 = 0;
    }
    SceneMessages_RegisterReceiver(p->receiver, p->name, 10);
    if (Script_SeekToKeyword('i') && Script_GetValue())
        RegistryF3E4_SetHidden(p);
    else {
        setUpdate(RegistryF3E4_UpdateVisibleIdle, &p->update);
        p->fieldF8 = 0;
        p->fieldD8 = 0;
    }
    return p;
}
