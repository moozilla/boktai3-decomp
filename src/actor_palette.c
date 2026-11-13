#include "actor_palette.h"

void ArmorPalette_CopyKindBanks(struct ArmorPaletteController *controller, void *data, u32 first, u32 extra)
{
    u16 *from;
    u16 *to;
    u8 *index;
    from = (u16 *)((u8 *)data + (first << 5));
    index = &controller->bank;
    to = controller->palettes[*index];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + ((first + 1) << 5));
    to = controller->palettes[*index + 1];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + ((first + 2) << 5));
    to = controller->palettes[*index + 2];
    Palette_CopyBank(to, from);
    from = (u16 *)((u8 *)data + (extra << 5));
    to = controller->palettes[controller->otherBank];
    Palette_CopyBank(to, from);
}

void ActorPalette_RestoreSelectedBanks(struct ArmorPaletteController *controller)
{
    Palette_CopyBank(controller->working[controller->bank], controller->palettes[controller->bank]);
    Palette_CopyBank(controller->working[controller->bank + 1], controller->palettes[controller->bank + 1]);
    Palette_CopyBank(controller->working[controller->bank + 2], controller->palettes[controller->bank + 2]);
    Palette_CopyBank(controller->working[controller->otherBank], controller->palettes[controller->otherBank]);
}

void ActorPalette_LoadScriptBanks(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *controller = &actor->paletteController;
    struct ActorPaletteSource *source = ScriptProc_Start(0x92B3, 0xCB8F);
    u16 *bank;
    u16 *destination;
    bank = source->banks[0];
    destination = controller->palettes[actor->paletteController.bank];
    Palette_CopyBank(destination, bank);
    bank = source->banks[1];
    destination = controller->palettes[actor->paletteController.bank + 1];
    Palette_CopyBank(destination, bank);
    bank = source->banks[2];
    destination = controller->palettes[actor->paletteController.bank + 2];
    Palette_CopyBank(destination, bank);
}

extern const u32 gUnk_0824E8A8[];

extern const u8 gUnk_0824E8F8[];

extern const u8 gUnk_0824E90C[];

void Armor_BindPaletteBanks(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *controller = &actor->paletteController;
    u32 index = actor->kind;
    u32 resource;
    void *source;
    u32 tag;
    u32 bank, otherBank;

    if (actor->flags & 0x200)
        index += 4;
    resource = gUnk_0824E8A8[index];
    if (resource != 0) {
        source = (u8 *)ScriptProc_Start(0x92b3, resource) + 0x14;
        tag = 0xf000 | resource;
    } else {
        source = gPaletteBankData + 0x41e0;
        tag = 0xf20f;
    }
    bank = gUnk_0824E8F8[index];
    otherBank = gUnk_0824E90C[index];
    if (actor->kind == 5) {
        controller->bank = 6;
        controller->otherBank = 0xc;
    } else {
        controller->bank = bank;
        controller->otherBank = otherBank;
    }
    ArmorPalette_CopyKindBanks(controller, source, bank, otherBank);
    actor->paletteId = tag;
    actor->palette = controller->working;
    actor->fragmentAnimations[0].paletteId = controller->bank + tag;
    actor->fragmentAnimations[0].palette = controller->working[controller->bank];
    actor->fragmentAnimations[1].paletteId = controller->bank + tag + 1;
    actor->fragmentAnimations[1].palette = controller->working[controller->bank + 1];
    actor->particleAnimation.paletteId = controller->bank + tag + 2;
    actor->particleAnimation.palette = controller->working[controller->bank + 2];
    switch (actor->kind) {
        case 3:
        case 5:
        case 7:
        case 9:
        case 0xd:
        case 0xf:
            actor->markerAnimation.paletteId = controller->otherBank + tag;
            actor->markerAnimation.palette = controller->working[controller->otherBank];
            break;
        default:
            actor->markerAnimation.paletteId = controller->bank + tag;
            actor->markerAnimation.palette = controller->working[controller->bank];
            break;
    }
}

void Armor_InitPaletteController(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *controller;
    Armor_BindPaletteBanks(actor);
    switch (actor->kind) {
        case 3:
        case 5:
        case 7:
        case 9:
        case 0xd:
        case 0xf:
            ActorPalette_LoadScriptBanks(actor);
            break;
        default:
            break;
    }
    controller = &actor->paletteController;
    ActorPalette_RestoreSelectedBanks(controller);
    controller->update = 0;
    controller->finish = 0;
}

void ActorPalette_Reset(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *palettes = &actor->paletteController;
    palettes->update = 0;
    if (palettes->finish) {
        Callback_InvokeActor(palettes, palettes->finish);
        palettes->finish = 0;
    }
    ActorPalette_RestoreSelectedBanks(palettes);
}

void ActorPalette_FadeWhiteFast(struct ArmorPaletteController *controller)
{
    if (controller->timer <= 8) {
        void *source;
        source = controller->palettes[controller->bank];
        Palette_BlendBank(controller->working[controller->bank], source, 0x7FFF, controller->timer, 3);
        source = controller->palettes[controller->bank + 1];
        Palette_BlendBank(controller->working[controller->bank + 1], source, 0x7FFF, controller->timer, 3);
        source = controller->palettes[controller->bank + 2];
        Palette_BlendBank(controller->working[controller->bank + 2], source, 0x7FFF, controller->timer, 3);
        source = controller->palettes[controller->otherBank];
        Palette_BlendBank(controller->working[controller->otherBank], source, 0x7FFF, controller->timer, 3);
    } else {
        controller->update = 0;
        if (controller->finish) {
            Callback_InvokeActor(controller, controller->finish);
            controller->finish = 0;
        }
    }
    controller->timer++;
}

void Armor_StartFastWhiteFade(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *controller = &actor->paletteController;
    void (*f)(struct ArmorPaletteController *) = ActorPalette_FadeWhiteFast;
    u32 z = 0;
    controller->update = f;
    controller->finish = z;
    actor->paletteController.timer = z;
}

void ActorPalette_FadeWhite(struct ArmorPaletteController *controller)
{
    if (controller->timer <= 16) {
        void *source;
        source = controller->palettes[controller->bank];
        Palette_BlendBank(controller->working[controller->bank], source, 0x7FFF, controller->timer, 4);
        source = controller->palettes[controller->bank + 1];
        Palette_BlendBank(controller->working[controller->bank + 1], source, 0x7FFF, controller->timer, 4);
        source = controller->palettes[controller->bank + 2];
        Palette_BlendBank(controller->working[controller->bank + 2], source, 0x7FFF, controller->timer, 4);
        source = controller->palettes[controller->otherBank];
        Palette_BlendBank(controller->working[controller->otherBank], source, 0x7FFF, controller->timer, 4);
    } else {
        controller->update = 0;
        if (controller->finish) {
            Callback_InvokeActor(controller, controller->finish);
            controller->finish = 0;
        }
    }
    controller->timer++;
}

void Armor_StartSlowWhiteFade(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *controller = &actor->paletteController;
    void (*f)(struct ArmorPaletteController *) = ActorPalette_FadeWhite;
    u32 z = 0;
    controller->update = f;
    controller->finish = z;
    actor->paletteController.timer = z;
}

void ActorPalette_BlendSavedBanks(struct ArmorPaletteController *controller)
{
    if (controller->timer <= 16) {
        void *source;
        void *saved;
        saved = controller->saved[0];
        source = controller->palettes[controller->bank];
        Palette_InterpolateBanks(controller->working[controller->bank], saved, source, controller->timer, 4);
        saved = controller->saved[1];
        source = controller->palettes[controller->bank + 1];
        Palette_InterpolateBanks(controller->working[controller->bank + 1], saved, source, controller->timer, 4);
        saved = controller->saved[2];
        source = controller->palettes[controller->bank + 2];
        Palette_InterpolateBanks(controller->working[controller->bank + 2], saved, source, controller->timer, 4);
        saved = controller->saved[3];
        source = controller->palettes[controller->otherBank];
        Palette_InterpolateBanks(controller->working[controller->otherBank], saved, source,
                                 controller->timer, 4);
    } else {
        controller->update = 0;
        if (controller->finish) {
            Callback_InvokeActor(controller, controller->finish);
            controller->finish = 0;
        }
    }
    controller->timer++;
}

void ActorPalette_BlendSavedBanksSlow(struct ArmorPaletteController *controller)
{
    if (controller->timer <= 16) {
        void *source;
        void *saved;
        saved = controller->saved[0];
        source = controller->palettes[controller->bank];
        Palette_InterpolateBanks(controller->working[controller->bank], saved, source, controller->timer, 5);
        saved = controller->saved[1];
        source = controller->palettes[controller->bank + 1];
        Palette_InterpolateBanks(controller->working[controller->bank + 1], saved, source, controller->timer, 5);
        saved = controller->saved[2];
        source = controller->palettes[controller->bank + 2];
        Palette_InterpolateBanks(controller->working[controller->bank + 2], saved, source, controller->timer, 5);
        saved = controller->saved[3];
        source = controller->palettes[controller->otherBank];
        Palette_InterpolateBanks(controller->working[controller->otherBank], saved, source,
                                 controller->timer, 5);
    } else {
        controller->update = 0;
        if (controller->finish) {
            Callback_InvokeActor(controller, controller->finish);
            controller->finish = 0;
        }
    }
    controller->timer++;
}

void Armor_StartPaletteTransition(struct SceneCombatActor *actor)
{
    struct ArmorPaletteController *callbacks = &actor->paletteController;
    void (*update)(struct ArmorPaletteController *) = ActorPalette_BlendSavedBanks;
    void (*complete)(struct ArmorPaletteController *) = ActorPalette_RestoreSelectedBanks;
        u16 *source;
    callbacks->update = update;
    callbacks->finish = complete;
    source = actor->paletteController.working[actor->paletteController.bank];
    Palette_CopyBank(actor->paletteController.saved[0], source);
    source = actor->paletteController.working[actor->paletteController.bank + 1];
    Palette_CopyBank(actor->paletteController.saved[1], source);
    source = actor->paletteController.working[actor->paletteController.bank + 2];
    Palette_CopyBank(actor->paletteController.saved[2], source);
    source = actor->paletteController.working[actor->paletteController.otherBank];
    Palette_CopyBank(actor->paletteController.saved[3], source);
    actor->paletteController.timer = 0;
}

void ActorPalette_PulseBlendSaved(struct ArmorPaletteController *controller)
{
    void *dest;
    void *source;
    void *saved;
    if (controller->timer <= 8) {
        source = controller->palettes[controller->bank];
        dest = controller->working[controller->bank];
        Palette_BlendBank(dest, source, 0x10, controller->timer, 3);
        source = controller->palettes[controller->bank + 1];
        dest = controller->working[controller->bank + 1];
        Palette_BlendBank(dest, source, 0x10, controller->timer, 3);
        source = controller->palettes[controller->bank + 2];
        dest = controller->working[controller->bank + 2];
        Palette_BlendBank(dest, source, 0x10, controller->timer, 3);
        source = controller->palettes[controller->otherBank];
        dest = controller->working[controller->otherBank];
        Palette_BlendBank(dest, source, 0x10, controller->timer, 3);
        if (controller->timer == 8) {
            u32 bankOffset;
            bankOffset = controller->bank * sizeof(controller->working[0]);
            saved = controller->saved[0];
            dest = (u8 *)controller->working[0] + bankOffset;
            Palette_CopyBank(saved, dest);
            bankOffset = controller->bank * sizeof(controller->working[0]);
            saved = controller->saved[1];
            dest = (u8 *)controller->working[1] + bankOffset;
            Palette_CopyBank(saved, dest);
            bankOffset = controller->bank * sizeof(controller->working[0]);
            saved = controller->saved[2];
            dest = (u8 *)controller->working[2] + bankOffset;
            Palette_CopyBank(saved, dest);
            bankOffset = controller->otherBank * sizeof(controller->working[0]);
            saved = controller->saved[3];
            dest = (u8 *)controller->working[0] + bankOffset;
            Palette_CopyBank(saved, dest);
        }
    } else {
        s32 blendStep = 16 - controller->timer;
        source = controller->palettes[controller->bank];
        saved = controller->saved[0];
        dest = controller->working[controller->bank];
        Palette_InterpolateBanks(dest, saved, source, blendStep, 3);
        source = controller->palettes[controller->bank + 1];
        saved = controller->saved[1];
        dest = controller->working[controller->bank + 1];
        Palette_InterpolateBanks(dest, saved, source, blendStep, 3);
        source = controller->palettes[controller->bank + 2];
        saved = controller->saved[2];
        dest = controller->working[controller->bank + 2];
        Palette_InterpolateBanks(dest, saved, source, blendStep, 3);
        source = controller->palettes[controller->otherBank];
        saved = controller->saved[3];
        dest = controller->working[controller->otherBank];
        Palette_InterpolateBanks(dest, saved, source, blendStep, 3);
    }
    controller->timer++;
    if (controller->timer > 15)
        controller->timer = 0;
}

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
