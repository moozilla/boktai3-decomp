#include "global.h"
#include "bosses/vanargand/vanargand_damage_part.h"

void Sprite_Remove(void *);

void Vanargand_RemovePartSprites(struct VanargandDamagePart *part)
{
    if (part != NULL) {
        if (part->auxiliarySprite.sprite.unknown4[0])
            Sprite_Remove(&part->auxiliarySprite);
        if (part->sprite.unknown4[0])
            Sprite_Remove(part);
    }
}
