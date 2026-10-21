#include "global.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: initializes a half's sprite, origin, collision, target registration and shadow.
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void ActorStatus_InitializeCollider(void *, s32, s32, s32);
void ActorStatus_InitializeBodyCollider(void *);
void ActorStatus_BeginTimedState(void);
void TargetRegistry_RegisterSceneActor(void *, void *, void (*)(void));
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void ScaledShadow_SetGraphic(void *, u32, s32);
void ScaledShadow_Enable(void *);
#define FIELD(t, p, off) (*(t *)((u8 *)(p) + (off)))
void PairedSceneActor_InitializeHalf(u8 *half, void *origin, s32 mode)
{
    void *data;
    void *shadow;
    register u32 zero asm("r4");
    u32 byteZero;
    u32 scale;
    u32 one;
    FIELD(s32, half, 0x15C) = mode;
    data = half + 0x2C;
    AnimData_Load(data, 0x89CA);
    Sprite_Init(half, data, 0);
    zero = 0;
    FIELD(u16, half, 0x10) = zero;
    half[7] = 2;
    if (mode == 1)
        FIELD(u32, half, 0) |= 4;
    FIELD(u32, half, 0) |= 1;
    {
        void *offset = half + 0x9C;
        u32 value = zero;
        register void *source asm("r0") = &value;
        asm("" : : "r"(source));
        CpuSet(source, offset, 0x05000002);
    }
    FIELD(void *, half, 0x90) = origin;
    if (mode == 0)
        FIELD(u16, half, 0xAC) = 64;
    else
        FIELD(u16, half, 0xAC) = zero;
    {
        u16 *size = (u16 *)(half + 0xAE);
        byteZero = 0;
        zero = 0;
        scale = 1024;
        *size = scale;
    }
    FIELD(u16, half, 0xB0) = 3;
    {
        u16 *active = (u16 *)(half + 0x172);
        one = 1;
        *active = one;
    }
    ActorStatus_InitializeCollider(half, 30, 128, 60);
    ActorStatus_InitializeBodyCollider(half);
    FIELD(u16, half, 0x188) = zero;
    half[0x16C] = byteZero;
    half[0x16D] = byteZero;
    {
        u8 *track = half + 0x58;
        FIELD(u32, track, 0) = zero;
        half[0x81] = byteZero;
        half[0x80] = byteZero;
        FIELD(u32, track, 0x34) = zero;
    }
    TargetRegistry_RegisterSceneActor(half + 0x20C, half + 0x108, ActorStatus_BeginTimedState);
    shadow = half + 0x1A0;
    ScaledShadow_Initialize(shadow, half + 0x1C, 128, 128, 128, 128, scale, one, 6, zero, zero);
    ScaledShadow_SetGraphic(shadow, 0x1C1B, 7);
    ScaledShadow_Enable(shadow);
    FIELD(u16, half, 0x180) = zero;
    FIELD(u32, half, 0x17C) = zero;
    FIELD(u16, half, 0x182) = zero;
    half[0x186] = byteZero;
    FIELD(u16, half, 0x184) = zero;
    half[0x187] = byteZero;
    FIELD(u32, half, 0x19C) = zero;
    FIELD(u16, half, 0xB2) = one;
    {
        u32 offset = 0x176;
        u16 *duration = (u16 *)(half + offset);
        *duration = offset - 0x4A;
    }
    FIELD(u16, half, 0x174) = zero;
}
