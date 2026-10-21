#include "global.h"
// MATCHING_CONCESSION: register-pinning

extern const u8 gUnk_0824E218[];

void SceneActorReaction_PlayAnimationBUntilComplete(void);
void SceneActorBehavior_CallActionScript(u8 *);
void Vorn_SelectRecoveryAction(void *, const u8 *, u32);

static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

void Vorn_UpdateScriptedAction(u8 *obj)
{
    if (TakeFlag(obj + 0x2B1)) {
        void (*fn)(void) = SceneActorReaction_PlayAnimationBUntilComplete;
        u8 code = 10;
        u32 zero;
        u32 *f;
        u16 *h;
        s32 mask;
        s32 off;
        {
            u8 *taken = obj + 0x2B2;
            zero = 0;
            *taken = 1;
        }
        do {
            u32 field = 0x2B0;
            do {
                ((u8 *)obj)[field] = zero;
            } while (0);
            field += 5;
            ((u8 *)obj)[off = field] = code;
            {
                register void **q asm("r0") = (void **)(obj + 0x21C);
                asm("" : "+r"(q));
                *q = (void *)fn;
            }
            field += 0x13;
            {
                register u16 *hh asm("r0") = (u16 *)(obj + field);
                asm("" : "+r"(hh));
                *hh = zero;
            }
        } while (0);
        f = (u32 *)(obj + 0x114);
        mask = -2;
        *f &= mask;
        *((u32 *)(obj + 0x104)) = zero;
        {
            u32 byte = obj[0xE9];
            obj[0x59] = byte;
        }
        SceneActorBehavior_CallActionScript(obj);
    }
    if (obj[0x2B0]) {
        u16 *h = (u16 *)(obj + 0x414);
        {
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = (u16 *)(obj + 0x2D4);
            flag |= *flags;
            *flags = flag;
        }
        {
            s32 off = 0x416;
            u16 value = *((u16 *)(((u8 *)obj) + off));
            off += 2;
            *((u16 *)(((u8 *)obj) - (-off))) = value;
        }
        Vorn_SelectRecoveryAction(obj, gUnk_0824E218, 0x27);
    } else {
        ++(*((u32 *)(obj + 0x2AC)));
    }
}
