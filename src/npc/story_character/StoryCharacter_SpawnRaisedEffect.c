#include "npc/manager/scene_actor_lifecycle.h"
void SunOrb_GetOrCreateManager(void);
u32 SunOrb_CreatePlayerTargeted(void *);
void Sound_PlaySong(u32);
void StoryCharacter_SpawnRaisedEffect(u32 a, struct SceneActor *p)
{
    u32 *effects = (u32 *)p->unknown20;
    if (p->animationEntered != 0) {
        struct ContactPosition *target;
        p->animationEntered = 0;
        p->finished = 0;
        SunOrb_GetOrCreateManager();
        target = &p->target;
        *target = p->body.position;
        p->target.y += 0x100;
        effects[1] = SunOrb_CreatePlayerTargeted(target);
        Sound_PlaySong(0x571);
    }
    p->finished = 1;
}
