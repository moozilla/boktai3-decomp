#include "global.h"
#include "render.h"

u32 SceneEffect_LoadRingAnimation(void);
void Anim_RestartSequence(void *, u32, u32, u32, u32);
void Callback_InvokeActor(void *, void *);
s32 Collider_Refresh(void *);
void Collider_SetPosition(void *, void *, u32);

static inline s32 sh(s32 v)
{
    s32 r;
    if (v >= 0)
        r = v >> 5;
    else
        r = -((-v) >> 5);
    return r;
}

void SceneEffectRing_Update(u8 *param)
{
    u8 *p = param;
    s16 buf[3];
    s16 *out = buf;
    s16 *pos = (s16 *)(p + 0x40);
    s32 x = pos[0], z = pos[2], a, c, y;
    u8 *mode;
    void *cb;
    u8 *refpos;
    u8 *col;
    s16 *px, *pz;
    u32 zero;
    out[0] = sh(3 * (x - z));
    c = sh(3 * (x + z));
    y = sh(3 * pos[1]);
    a = c - y;
    zero = 0;
    c = c + y;
    out[0] = out[0] - gCameraOrigin[0] + 120;
    out[1] = a - gCameraOrigin[1] + 80;
    c = c - gCameraOrigin[2];
    out[2] = c;
    if ((u16)(buf[0] + 0x20) > 0x130) {
        *(u32 *)p = zero;
        cb = *(void **)(p + 4);
        if (cb != 0) {
            Callback_InvokeActor(p, cb);
            *(u32 *)(p + 4) = zero;
        }
        return;
    }
    mode = p + 0xb4;
    switch (*mode) {
        default:
            refpos = p + 0x40;
            break;
        case 0:
            if (*(p + 0xb5) != 0) {
                Anim_RestartSequence(p + 0x50, SceneEffect_LoadRingAnimation(), 1, 0, zero);
                (*mode)++;
            }
        case 1:
            px = (s16 *)(p + 0x40);
            px[0] += 0x50;
            pz = (s16 *)(p + 0x44);
            pz[0] -= 0x50;
            refpos = (u8 *)px;
            break;
        case 0x10:
            if (*(p + 0xb5) != 0) {
                Anim_RestartSequence(p + 0x50, SceneEffect_LoadRingAnimation(), 1, 0, zero);
                (*mode)++;
            }
        case 0x11:
            px = (s16 *)(p + 0x40);
            px[0] -= 0x50;
            pz = (s16 *)(p + 0x44);
            pz[0] += 0x50;
            refpos = (u8 *)px;
            break;
    }
    col = p + 0x60;
    Collider_Refresh(col);
    Collider_SetPosition(col, refpos, 0);
}
