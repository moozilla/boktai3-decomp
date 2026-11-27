#include "global.h"
struct SpriteComponent {
    void (*update)(struct SpriteComponent *);
    void (*destroy)(struct SpriteComponent *);
};
s32 Callback_InvokeActor(void *, void *);
u32 SpriteComponent_Destroy(struct SpriteComponent *component)
{
    u32 result;
    void (*destroy)(struct SpriteComponent *);
    component->update = NULL;
    destroy = component->destroy;
    result = (u32)component;
    if (destroy) {
        result = Callback_InvokeActor(component, destroy);
        component->destroy = NULL;
    }
    return result;
}
