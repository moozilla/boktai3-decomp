#include "global.h"
#include "ram.h"
void *ScaledShadowManager_Create(int, int);
void ScaledShadow_Update(void *);
void ScaledShadow_UpdateNoOp(void *);
void AnimData_Load(void *, int);
void Sprite_Init(void *, void *, int);
void ScaledShadowManager_AddShadow(void *, void *);
int ScaledShadow_Initialize(u8 *p, void *source, int scale, int min, int max, int extra,
                            int maxHeight, int minHeight, int shift, int nearShift, int mode)
{
    void *m = gScaledShadowManager;
    u8 *resource, *sprite;
    int zero;
    if (!m) {
        m = ScaledShadowManager_Create(0, 0);
        if (!m)
            return -1;
    }
    zero = 0;
    p[0] = zero;
    *(void **)(p + 8) = source;
    p[1] = zero;
    *(u16 *)(p + 2) = zero;
    *(u16 *)(p + 4) = zero;
    p[0x54] = scale;
    p[0x55] = min;
    p[0x56] = max;
    p[0x57] = extra;
    *(u16 *)(p + 0x58) = maxHeight;
    *(u16 *)(p + 0x5a) = minHeight;
    *(u16 *)(p + 0x5c) = shift;
    *(u16 *)(p + 0x5e) = nearShift;
    if (!mode)
        *(void (**)(void *))(p + 0x60) = ScaledShadow_Update;
    else
        *(void (**)(void *))(p + 0x60) = ScaledShadow_UpdateNoOp;
    resource = p + 0x38;
    AnimData_Load(resource, 0x1C1B);
    sprite = p + 12;
    Sprite_Init(sprite, resource, 514);
    *(u16 *)(sprite + 16) = 5;
    p[0x13] = 2;
    ScaledShadowManager_AddShadow(m, p);
    return 0;
}
