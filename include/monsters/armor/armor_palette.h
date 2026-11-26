#ifndef GUARD_ARMOR_PALETTE_H
#define GUARD_ARMOR_PALETTE_H
#include "monsters/armor/scene_combat_actor.h"

static inline void ArmorPalette_BeginEffect(struct ArmorPaletteController *controller,
                                            void (*update)(struct ArmorPaletteController *),
                                            void (*finish)(struct ArmorPaletteController *))
{
    controller->update = update;
    controller->finish = finish;
    controller->timer = 0;
}

#endif
