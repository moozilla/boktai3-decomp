#include "global.h"
struct Manager {
    u8 pad[24];
    u8 *resource;
};
extern struct Manager *gShadowManager;
void *ShadowTask_Create(int, int);
void Shadow_UpdateTerrain(void *);
void ActorShadow_UpdateNone(void *);
void ParticleSprite_Initialize(void *, void *, int);
void Effect_SetOrigin(void *, int, int);
void ShadowList_Insert(void *, void *);
int Shadow_Initialize(u8 *p, void *source, int mode)
{
    u8 *sprite;
    int zero;
    if (!gShadowManager && !ShadowTask_Create(0, 0))
        return -1;
    zero = 0;
    p[0] = zero;
    *(void **)(p + 8) = source;
    p[1] = zero;
    *(u16 *)(p + 2) = zero;
    *(u16 *)(p + 4) = zero;
    if (!mode)
        *(void (**)(void *))(p + 0x34) = Shadow_UpdateTerrain;
    else
        *(void (**)(void *))(p + 0x34) = ActorShadow_UpdateNone;
    sprite = p + 12;
    ParticleSprite_Initialize(sprite, gShadowManager->resource, 512);
    Effect_SetOrigin(sprite, -8, -8);
    {
        u8 *resource = gShadowManager->resource;
        int width = p[0x16] >> 3, height = p[0x17] >> 3;
        *(u16 *)(p + 0x14) = width * height * 34 + *(u16 *)(resource + 4);
    }
    p[0x1c] = 252;
    p[0x1b] = 2;
    ShadowList_Insert(gShadowManager, p);
    return 0;
}
