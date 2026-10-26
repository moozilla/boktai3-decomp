#define SceneProcSprites_GetOrCreate BallisticSpriteEffect_EnsureManager
#include "global.h"
#include "ram.h"
struct SpriteVector {
    u32 x, y, z, w;
};
struct SceneProcSpriteSlot {
    void (*callback)(void);
    u8 pad04[0x20 - 4];
    u32 spriteFlags;
    u8 pad24[0x9c - 0x24];
    struct SpriteVector position;
    s32 vx, vy, vz, vw;
    struct SpriteVector target;
    s32 targetX, targetY;
    u8 flags, used;
    u8 padD6[4];
    u8 kind;
    u8 padDB[5];
};
struct SceneProcSprites {
    u8 pad[0x18];
    struct SceneProcSpriteSlot entries[4];
};
u8 *BallisticSpriteEffect_EnsureManager(void);
void Memory_Clear(void *, u32);
void BallisticSpriteEffect_OnAnimationComplete(void);
void BallisticSpriteEffect_UpdateLaunch(void);
void SceneProcSprites_SubmitBounds(void *, void (*)(void), u32, u32);
s32 SceneProcSprites_CreateEntry(struct SpriteVector *position, s32 a, s32 b, u32 kind, u16 *params)
{
    struct SceneProcSprites *p;
    u8 *existing = gSceneProcSprites;
    s32 i;
    if (existing)
        p = (void *)existing;
    else {
        p = (void *)BallisticSpriteEffect_EnsureManager();
        if (!p)
            return -1;
    }
    for (i = 0; i < 4; i++) {
        if (!p->entries[i].used) {
            p->entries[i].spriteFlags |= 1;
            p->entries[i].position = *position;
            p->entries[i].target = *position;
            p->entries[i].used = 1;
            Memory_Clear(&p->entries[i].vx, 16);
            p->entries[i].vx = b + 0x2000;
            p->entries[i].vy = -0x1000;
            p->entries[i].vz = 0;
            p->entries[i].targetX = a;
            p->entries[i].targetY = p->entries[i].vx;
            p->entries[i].kind = kind;
            SceneProcSprites_SubmitBounds(&p->entries[i], BallisticSpriteEffect_OnAnimationComplete,
                                          params[3], kind + 1);
            ((struct SceneProcSprites *)((u8 *)p + i * 0xe0))->entries[0].callback = BallisticSpriteEffect_UpdateLaunch;
            p->entries[i].flags = 1;
            return i;
        }
    }
    return -1;
}
