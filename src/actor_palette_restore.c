#include "actor_palette.h"

void ActorPalette_BeginTransition(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *palettes = &actor->paletteController;
    void (*update)(struct ArmorPaletteController *) = ActorPalette_PulseBlendSaved;
    void (*cleanup)(struct ArmorPaletteController *) = ActorPalette_RestoreSelectedBanks;
    palettes->update = update;
    palettes->finish = cleanup;
    actor->paletteController.timer = 0;
}

void Palette_UpdatePulseFade(struct ArmorPaletteController *controller)
{
    s32 red, green, blue;

    if (controller->timer < 32) {
        red = controller->palettes[controller->bank + 1][1];
        green = (red >> 5) & 31;
        blue = (red >> 10) & 31;
        red = red & 31;
        red = (red * (32 - controller->timer)) >> 5;
        green = (green * (32 - controller->timer)) >> 5;
        blue = (blue * (32 - controller->timer)) >> 5;
        controller->working[controller->bank + 1][1] = red | (green << 5) | (blue << 10);
        red = controller->palettes[controller->bank + 2][1];
        green = (red >> 5) & 31;
        blue = (red >> 10) & 31;
        red = red & 31;
        red = (red * (32 - controller->timer)) >> 5;
        green = (green * (32 - controller->timer)) >> 5;
        blue = (blue * (32 - controller->timer)) >> 5;
        controller->working[controller->bank + 2][1] = red | (green << 5) | (blue << 10);
    } else {
        red = controller->palettes[controller->bank + 1][1];
        green = (red >> 5) & 31;
        blue = (red >> 10) & 31;
        red = red & 31;
        red = (red * (controller->timer - 32)) >> 5;
        green = (green * (controller->timer - 32)) >> 5;
        blue = (blue * (controller->timer - 32)) >> 5;
        controller->working[controller->bank + 1][1] = red | (green << 5) | (blue << 10);
        red = controller->palettes[controller->bank + 2][1];
        green = (red >> 5) & 31;
        blue = (red >> 10) & 31;
        red = red & 31;
        red = (red * (controller->timer - 32)) >> 5;
        green = (green * (controller->timer - 32)) >> 5;
        blue = (blue * (controller->timer - 32)) >> 5;
        controller->working[controller->bank + 2][1] = red | (green << 5) | (blue << 10);
    }
    controller->timer++;
    if (controller->timer > 63)
        controller->timer = 0;
}

void Armor_StartAccentPaletteFade(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *palette = &actor->paletteController;
    switch (actor->kind) {
        case 3:
        case 5:
        case 7:
        case 9:
        case 0xd:
        case 0xf:
            if (palette->update == 0) {
                palette->update = Palette_UpdatePulseFade;
                palette->finish = 0;
                palette->timer = 0;
            }
            break;
        default:
            break;
    }
}

void Armor_CopyScenePalette(void *a)
{
    Palette_CopyBank(a, gPaletteBankData + 0x3240);
}

void Armor_UpdatePaletteRestoreDelay(struct ArmorPaletteController *controller)
{
    s32 c = (u16)controller->timer + 1;
    u32 z = 0;
    controller->timer = c;
    if ((s16)c > 7) {
        void (*q)(struct ArmorPaletteController *);
        controller->update = z;
        q = controller->finish;
        if (q) {
            Callback_InvokeActor(controller, q);
            controller->finish = z;
        }
    }
}

void Armor_StartFullPaletteRestoreDelay(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *palette = &actor->paletteController;
    void (*update)(struct ArmorPaletteController *);
    void (*completion)(struct ArmorPaletteController *);

    ActorPalette_RestoreSelectedBanks(palette);
    Armor_CopyScenePalette(palette->working[palette->bank]);
    Armor_CopyScenePalette(palette->working[palette->bank + 1]);
    Armor_CopyScenePalette(palette->working[palette->bank + 2]);
    Armor_CopyScenePalette(palette->working[palette->otherBank]);
    update = Armor_UpdatePaletteRestoreDelay;
    completion = ActorPalette_RestoreSelectedBanks;
    palette->update = update;
    palette->finish = completion;
    palette->timer = 0;
}

void Armor_StartPaletteRestoreDelay(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *controller = &actor->paletteController;
    void (*update)(struct ArmorPaletteController *);
    void (*finish)(struct ArmorPaletteController *);
    ActorPalette_RestoreSelectedBanks(controller);
    Armor_CopyScenePalette(controller->working[actor->paletteController.bank + 2]);
    update = Armor_UpdatePaletteRestoreDelay;
    finish = ActorPalette_RestoreSelectedBanks;
    controller->update = update;
    controller->finish = finish;
    actor->paletteController.timer = 0;
}

void Armor_InitPaletteBanks(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *controller = &actor->paletteController;
    u16 *source;
    void (*update)(struct ArmorPaletteController *);
    void (*finish)(struct ArmorPaletteController *);
    Armor_BindPaletteBanks(actor);
    update = ActorPalette_BlendSavedBanksSlow;
    finish = ActorPalette_RestoreSelectedBanks;
    controller->update = update;
    controller->finish = finish;
    source = controller->working[actor->paletteController.bank];
    Palette_CopyBank(actor->paletteController.saved[0], source);
    source = controller->working[actor->paletteController.bank + 1];
    Palette_CopyBank(actor->paletteController.saved[1], source);
    source = controller->working[actor->paletteController.bank + 2];
    Palette_CopyBank(actor->paletteController.saved[2], source);
    source = controller->working[actor->paletteController.otherBank];
    Palette_CopyBank(actor->paletteController.saved[3], source);
    actor->paletteController.timer = 0;
}

void ActorPalette_ReturnImmediately(struct ArmorPaletteController *controller)
{
}

void ActorPalette_DisableCallbacks(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *palettes = &actor->paletteController;
    palettes->update = ActorPalette_ReturnImmediately;
    palettes->finish = ActorPalette_ReturnImmediately;
}
