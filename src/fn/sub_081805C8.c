#define SpriteComponent_Init sub_081805C8
#define SpriteComponent_InitState sub_08180490
#define SpriteComponent_Update sub_08180570
#define SpriteComponent_RemoveSprite ArmorIndicator_DestroySprite
#include "global.h"

struct SpriteComponent {
    void (*update)(struct SpriteComponent *);
    void (*destroy)(struct SpriteComponent *);
};
s32 sub_08180490(struct SpriteComponent *);
void sub_08180570(struct SpriteComponent *);
void ArmorIndicator_DestroySprite(struct SpriteComponent *);

s32 sub_081805C8(struct SpriteComponent *component)
{
    s32 result;
    void (*update)(struct SpriteComponent *);
    void (*destroy)(struct SpriteComponent *);
    component->update = NULL;
    component->destroy = NULL;
    result = sub_08180490(component);
    update = sub_08180570;
    destroy = ArmorIndicator_DestroySprite;
    component->update = update;
    component->destroy = destroy;
    return result;
}
