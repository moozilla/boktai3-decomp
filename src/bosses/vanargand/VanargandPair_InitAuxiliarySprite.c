#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"

s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);

void VanargandPair_InitAuxiliarySprite(struct VanargandPairAttack *boss,
                                       struct VanargandPartSprite *auxiliary)
{
    struct SpriteRenderResource *graphics = &auxiliary->graphics;
    struct ContactPosition *position;
    u32 width, height;

    AnimData_Load(graphics, 0x89CC);
    graphics->paletteKey = 0x6F;
    graphics->palette = boss->palettes;
    Sprite_Init(auxiliary, graphics, 0);
    auxiliary->sprite.frame = 5;
    auxiliary->sprite.priority = 3;
    position = &auxiliary->sprite.position;
    width = 0x33B;
    height = 0x100;
    position->x = width;
    position->y = height;
    position->z = width;
}
