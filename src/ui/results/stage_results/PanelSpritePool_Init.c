#include "global.h"
#include "ui/results/actor_marker_pool.h"

s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ScriptProc_Start(u16, u16);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void Memory_Clear(void *, u32);

s32 PanelSpritePool_Init(struct MarkerPool *pool)
{
    s32 i;
    for (i = 0; i < 8; i++) {
        if (!AnimData_Load(pool->slots[i].resource, 0x409C))
            return -1;
        Sprite_Init(&pool->slots[i].sprite, pool->slots[i].resource, 0);
        pool->spriteSheet = ScriptProc_Start(0x922E, 0x1C22);
        Anim_SetSequence(&pool->slots[i].sequence, pool->spriteSheet, 0, 0, 0);
        Memory_Clear(&pool->slots[i].sprite.position, 8);
        pool->slots[i].active = 0;
        pool->slots[i].sprite.flags |= 0x211;
        pool->slots[i].sprite.priority = 3;
    }
    gPanelSpritePool = (u8 *)pool;
    return 0;
}
