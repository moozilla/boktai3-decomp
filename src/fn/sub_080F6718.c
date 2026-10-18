#define Sword_DestroyTask sub_080F6718
#include "global.h"
struct SwordSprite {
    u8 unknown0[0x2C];
};
struct SwordComponent {
    u8 sprite[0x48], collider[0x54], secondaryCollider[0xF8];
};
struct SwordTask {
    u8 unknown0[0xA8];
    struct SwordSprite sprites[4];
    u8 unknown158[0x364];
    struct SwordComponent components[4];
    u8 unknownB0C[0x5C], sprite[0x48], animation[0x6C], collider[0x54], secondaryCollider[0x54];
    u8 unknownCC4[0x68], componentCount, unknownD2D[0x5B], status[0x1C];
};
void Sprite_Remove(void *);
void Collider_Remove(void *);
void sub_08013B74(void *);
void sub_0815F738(void *);
s32 sub_080F6718(struct SwordTask *p)
{
    struct SwordSprite *sprite;
    s32 i;
    Sprite_Remove(p->sprite);
    sub_08013B74(p->animation);
    sub_0815F738(p->status);
    Collider_Remove(p->collider);
    Collider_Remove(p->secondaryCollider);
    sprite = p->sprites;
    i = 3;
    do {
        Sprite_Remove(sprite);
        sprite++;
        i--;
    } while (i >= 0);
    for (i = 0; i < p->componentCount; i++) {
        struct SwordComponent *component = &p->components[i];
        Sprite_Remove(component);
        Collider_Remove(component->collider);
        Collider_Remove(component->secondaryCollider);
    }
    return 0;
}
