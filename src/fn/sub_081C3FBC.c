#include "global.h"

#define RegistryF3E4_CreateHidden sub_081C3FBC

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

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void CpuSet(const void *src, void *dest, u32 control);

void RegistryF3E4_OnTaskUpdate(void *);
void RegistryF3E4_OnTaskDestroy(void *);
void RegistryF3E4_SetHidden(struct F3E4Actor *);

static inline void fill32(void *dst, u32 *tmp, u32 control)
{
    *tmp = 0;
    CpuSet(tmp, dst, control);
}

struct F3E4Actor *RegistryF3E4_CreateHidden(s32 field3B, s32 mode)
{
    struct F3E4Actor *p = Task_Create(0xA, 0xFC);
    u32 zero;
    if (p)
        Task_SetCallbacks(p, RegistryF3E4_OnTaskUpdate, RegistryF3E4_OnTaskDestroy);
    p->state.mode = mode;
    AnimData_Load(p->animData, 0x3641);
    Sprite_Init(p->sprite, p->animData, 0x12);
    p->field3B = field3B;
    fill32(&p->x, &zero, 0x05000002);
    p->state.field4 = 0;
    p->state.field6 = 0;
    {
        struct StateInner *in = &p->state.inner;
        in->field0 = 0;
        in->field29 = 0;
        in->field28 = 0;
        in->field34 = 0;
    }
    RegistryF3E4_SetHidden(p);
    return p;
}
