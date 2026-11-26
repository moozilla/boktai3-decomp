#define SpriteComponent_RemoveSprite ArmorIndicator_DestroySprite
#include "global.h"

struct SpriteComponent {
    void (*update)(struct SpriteComponent *);
    void (*destroy)(struct SpriteComponent *);
};
s32 SceneSpriteCollider_Init(struct SpriteComponent *);
void SpriteComponent_Update(struct SpriteComponent *);
void ArmorIndicator_DestroySprite(struct SpriteComponent *);

s32 SpriteComponent_Init(struct SpriteComponent *component)
{
    s32 result;
    void (*update)(struct SpriteComponent *);
    void (*destroy)(struct SpriteComponent *);
    component->update = NULL;
    component->destroy = NULL;
    result = SceneSpriteCollider_Init(component);
    update = SpriteComponent_Update;
    destroy = ArmorIndicator_DestroySprite;
    component->update = update;
    component->destroy = destroy;
    return result;
}
