#define SceneComposite_RemoveAnimatedSprite Vanargand_DestroyAnimatedPart
#define SceneComposite_RemoveSpriteArray    Vanargand_DestroyPartSprites
#define SpritePair_Remove                   Vanargand_RemovePartSprites
#define gSceneComposite                     gUnk_02000250
#include "global.h"
struct SceneComposite {
    u8 actor[0x18c];
    u8 animatedSprite[0x144];
    u8 sprites[0x12c];
    u8 spritePair[0x180];
    void *resource;
    u8 unknown580[0x9c4];
    void *task;
};
void SceneActors_Destroy(void *);
void Vanargand_DestroyAnimatedPart(void *);
void Vanargand_DestroyPartSprites(void *);
void Vanargand_RemovePartSprites(void *);
void PooledController_ActivateIdleSlots(void *);
void Task_Kill(void *);
extern void *gUnk_02000250;
void SceneComposite_Destroy(struct SceneComposite *p)
{
    SceneActors_Destroy(p);
    Vanargand_DestroyAnimatedPart(p->animatedSprite);
    Vanargand_DestroyPartSprites(p->sprites);
    Vanargand_RemovePartSprites(p->spritePair);
    PooledController_ActivateIdleSlots(p->resource);
    Task_Kill(p->task);
    gUnk_02000250 = 0;
}
