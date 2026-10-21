#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct S7Trans {
    u8 pad0[0x90];
    u32 frame;
    s16 hp;
    u8 pad96[0xC];
    u8 progressFlag;
    u8 padA3;
    u32 progress;
    u8 padA8[7];
    u8 finished, entered, initialize;
    u8 padB2[0x12];
    u32 timer;
    u8 padC8[0x38];
    void (*callback)(void);
    u8 pad104[0x190];
    u32 val294;
    u8 pad298[0x104];
    u32 state;
    u8 pad3A0[0x210];
    s32 counter;
};
void SceneMover_InterpolateScreenX(void);
void SceneActors_UpdatePaletteDispersal(void);
void sub_081EEE30(void);
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);

static inline u8 ConsumeEntry(struct S7Trans *p)
{
    u8 *q = &p->entered;
    if (*q) {
        *q = 0;
        *(q - 1) = 0;
        return TRUE;
    }
    return FALSE;
}
static inline void SetCallback(void (*v)(void), void (**a)(void))
{
    *a = v;
}
#define SETCB(p, fn) SETCBV(p, fn, 1)
#define SETCBV(p, fn, v)                                                                           \
    do {                                                                                           \
        SetCallback(fn, &(p)->callback);                                                           \
        (p)->frame = 0;                                                                            \
        (p)->initialize = (v);                                                                     \
        (p)->progressFlag = 0;                                                                     \
        (p)->progress = 0;                                                                         \
    } while (0)

void SingletonSlot7_UpdateEndingTransition(struct S7Trans *p)
{
    register struct S7Trans *q asm("r5") = p;
    u32 f = ConsumeEntry(p);
    if (f == 1) {
        SetCallback(SceneMover_InterpolateScreenX, &p->callback);
        asm("" ::: "r1");
        p->frame = 0;
        p->initialize = 1;
        p->progressFlag = 0;
        p->progress = 0;
        q->val294 = 100;
        q->state = 0;
        q->counter = 0;
        asm("" ::"r"(f));
    }
    switch (q->state) {
        case 0:
            if (p->finished) {
                SETCB(p, sub_081EEE30);
                {
                    u32 *sp = &q->state;
                    *sp = *sp + 1;
                }
            }
            break;
        case 1:
            if (p->hp <= 0) {
                SETCB(p, SceneActors_UpdatePaletteDispersal);
                {
                    u32 *sp = &q->state;
                    *sp = *sp + 1;
                }
            }
            if (p->timer > 600) {
                q->val294 = 0xa0;
                SETCB(p, SceneMover_InterpolateScreenX);
                q->state = 3;
            }
        case 2:
            if (p->finished) {
                ScreenTransition_SendCommand(1, 3, 0x1f, 0x1f, 0x1f, 0xFFFF, 0);
                q->state = 4;
            }
            break;
        case 3:
            if (p->finished) {
                SETCB(p, sub_081EEE30);
            }
            break;
        case 4: {
            s32 *cp = &q->counter;
            if (*cp > 8) {
                SETCB(p, sub_081EEE30);
            }
            *cp = *cp + 1;
        } break;
    }
    p->timer++;
    asm("" ::"r"(q));
}
