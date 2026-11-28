#include "npc/story_character/scene_character.h"
#include "scene/collision/contact_position.h"

s32 Mod(s32, s32);
struct ContactPosition *SunOrb_GetPosition(s32);
void sub_081FED40(s32);
void Sound_PlaySong(u32);
void StoryCharacter_SpawnRadialEffect(struct SceneNpc *);

void SceneCharacter_UpdateParticleDissolve(void *context, struct SceneNpc *actor, s32 frame)
{
    struct SceneCharacterEffects *effects = &actor->effects;
    s32 i;
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->done = 0;
        *(struct ContactPosition *)effects->unknown8 = *SunOrb_GetPosition((s16)effects->orbId);
        sub_081FED40((s16)effects->orbId);
        Sound_PlaySong(0x5ca);
        actor->animationPhase = 0;
        actor->stateFrame = 0;
    }
    if (frame <= 30) {
        if (frame <= 19 || Mod(frame, 2) == 0) {
            StoryCharacter_SpawnRadialEffect(actor);
            if (frame <= 9)
                for (i = 0; i < 2; i++)
                    StoryCharacter_SpawnRadialEffect(actor);
        }
    }
    if (frame > 30)
        actor->done = 1;
}
