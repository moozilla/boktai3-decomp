#include "global.h"
#include "ram.h"
extern u8 gUnk_0203B400[];
extern u8 *gUnk_02000580;
u32 Callback_InvokeActor(u8 *, void *);
void Actor_StepFacingAngle(u8 *);
void Actor_SetFacingAnimationEntry(u8 *, u32, u32, u32, u32, u32);
void MotionPool_InitializeParams(void *);
void MotionPool_SetLifetimeAndVelocity(void *, u32, u32, u32, u32);
void MotionPool_SetTracking(void *, u32, u32, u32);
void MotionPool_SetContact(void *, u32, u32, u32, u32, u32);
void MotionPool_SetBounds(void *, u32, u32, u32);
void MotionPool_SetGraphics(void *, u32, u32, u32);
void MotionPool_Spawn(void *, void *);
struct VD218 {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
void Actor_OnTrackingAction(u8 *p, u32 tick)
{
    u8 buf[0x44];
    struct VD218 v;
    void *fn;
    if (p[0x12d])
        p[0x12d] = 0;
    fn = *(void **)(p + 0x124);
    if (fn && Callback_InvokeActor(p, fn))
        return;
    fn = *(void **)(p + 0x120);
    if (fn && Callback_InvokeActor(p, fn))
        return;
    Actor_StepFacingAngle(p);
    Actor_SetFacingAnimationEntry(p, 1, 0, 1, 0, 0);
    if ((tick & 3) == 0) {
        gRandomIndex = (gRandomIndex + 1) & 0x3ff;
        {
            s32 random = *(u8 *)((gRandomIndex << 1) + (u32)gUnk_0203B400);
            if (random <= 63) {
                MotionPool_InitializeParams(buf);
                MotionPool_SetLifetimeAndVelocity(buf, 0, 0xf0, p[9] + 0x80, 12);
                MotionPool_SetTracking(buf, 0xb4, *(u32 *)(gUnk_02000580 + 0x24), 1);
                MotionPool_SetContact(buf, 0, 0x200004, 0x64, 0x32, 0x28);
                MotionPool_SetBounds(buf, 0x10, 0x10, 0x10);
                MotionPool_SetGraphics(buf, 0x61f9, 0x29, 0x29f);
                v = *(struct VD218 *)(p + 0xc);
                v.y += 0x80;
                MotionPool_Spawn(buf, &v);
            }
        }
    }
}
