#include "global.h"
#include "scene_actor_registry.h"
#include "scene_actor_animation.h"

void sub_0808EACC(struct SceneActorRegistryActor *);

#define SceneActorAnimation_RefreshProfileEntry sub_0808E9A0

void sub_0808E9A0(struct SceneActorRegistryActor *actor)
{
    u32 triggered;

    if (actor->field2B2) {
        actor->field2B2 = 0;
        triggered = 1;
    } else {
        triggered = 0;
    }
    asm("" : "+r"(triggered));
    if (triggered) {
        struct SceneActorProfileEntry *entry;
        u32 selection;
        u32 flags;

        entry = (void *)SceneActorProfiles_GetEntryAddress(actor->profileIndex,
                                                             actor->livingCountByCategory, 15);
        selection = entry->value;
        flags = 0;
        asm("" : "+r"(flags));

        if (!actor->drawableKind) {
            struct SceneActorAnimationSequenceDrawable *drawable;

            actor->unknown2B5[2] = selection;
            drawable = actor->drawable;
            Anim_SetSequence(drawable->sequence, (void *)drawable->resource, selection, 1, flags);
            actor->field2B3 = 0;
        } else {
            struct SceneActorAnimationTile *tile =
                &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
            u32 newSelection = selection + 1;
            u8 *saved = &actor->unknown2B5[2];
            u8 savedValue;
            u8 *restart;

            asm("" : "+r"(newSelection));
            savedValue = *saved;
            restart = &actor->field2B3;

            if (savedValue == newSelection && !*restart && !(tile->flags & 4))
                goto skip;
            *saved = newSelection;
            {
                typedef void (*SetKeyframesFn)(void *, void *, u16, u32, u32);
                u32 mode = (flags & 4) ? 6 : 4;
                ((SetKeyframesFn)Anim_SetKeyframes)(
                    &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                    actor->drawable, newSelection, 2, mode);
            }
            tile->flags &= ~4;
            *restart = 0;
        }
    skip:;
    }
    if (actor->field2B3) {
        void *control = (void *)sub_0808EACC;
        u32 resetValue = 0x24;

        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->unknown2B5[0] = resetValue;
        actor->control = control;
        actor->field2C8 = 0;
        {
            u32 *flagsPtr = &actor->flags114;
            u32 mask = ~1;
            u32 value;
            value = *flagsPtr;
            value &= mask;
            *flagsPtr = value;
        }
        actor->field104 = 0;
    }
}
