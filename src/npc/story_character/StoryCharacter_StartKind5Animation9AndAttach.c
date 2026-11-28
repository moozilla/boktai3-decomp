#include "npc/manager/scene_actor_lifecycle.h"
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
#include "global.h"
#include "npc/story_character/state_actor.h"

void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, u32, u32, u32, u32);
s32 sub_081C8FB0(struct StateActorTargetContext *, struct SceneActor *, s32);

void StoryCharacter_StartKind5Animation9AndAttach(struct StateActorTargetContext *ctx,
                                                  struct SceneActor *p, u32 a2)
{
    u32 r;
    u32 mask;

    if (p->animationEntered != 0) {
        p->animationEntered = 0;
        p->finished = 0;
        switch (p->variant) {
            case 4:
                r = 0;
                break;
            case 5:
                r = 9;
                break;
            case 6:
                r = 0;
                break;
            case 7:
                r = 0;
                break;
            case 8:
                r = 0;
                break;
            case 9:
                r = 0;
                break;
            case 10:
                r = 0;
                break;
            case 11:
                r = 0;
                break;
            case 12:
                r = 0;
                break;
            case 13:
                r = 0;
                break;
            case 14:
                r = 0;
                break;
            case 15:
                r = 0;
                break;
            case 16:
                r = 0;
                break;
            case 17:
                r = 0;
                break;
            case 18:
                r = 0;
                break;
            case 19:
                r = 0;
                break;
            case 20:
                r = 0;
                break;
            case 21:
                r = 0;
                break;
            case 22:
                r = 0;
                break;
            case 23:
                r = 0;
                break;
            case 24:
                r = 0;
                break;
            case 25:
                r = 0;
                break;
            case 26:
                r = 0;
                break;
            case 27:
                r = 0;
                break;
            case 28:
                r = 0;
                break;
            case 29:
                r = 0;
                break;
            default:
                r = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, r, 1, 1, 4);
    }

    if (sub_081C8FB0(ctx, p, 0))
        SceneActor_SetFlags(p, 0x2800);
    SceneActor_SetFlags(p, 0x1000);

    mask = a2 & 0x3f;
    if (mask == 0)
        p->attachment = ActorAttachment_UpdateFromDescriptor(p->attachment, &p->body, 2, 0, mask,
                                                             mask, 0xfa, 0xfa);

    if (DisplayAnimation_TestFinished(&p->drawable.common))
        p->finished = 1;
}
