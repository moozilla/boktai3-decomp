#include "global.h"
#include "render.h"
struct Group {
    u8 active, count, type, priority;
    u8 particles[448];
};
struct Manager {
    u8 pad[24];
    u32 timer;
    void *small, *large;
    struct Group groups[6];
};
void ParticleGroup_UpdateFrozen(struct Manager *, struct Group *);
void sub_0801404C(struct Manager *, struct Group *);
void ParticleGroup_Update(struct Manager *, struct Group *);
int ParticleManager_Update(struct Manager *m)
{
    struct Group *g;
    int i;
    if (gEnginePauseFlags) {
        g = m->groups;
        for (i = 5; i >= 0; g++, i--) {
            if (g->active)
                ParticleGroup_UpdateFrozen(m, g);
        }
    } else {
        g = m->groups;
        for (i = 5; i >= 0; g++, i--) {
            if (g->active) {
                if (g->type <= 8)
                    sub_0801404C(m, g);
                else
                    ParticleGroup_Update(m, g);
            }
        }
    }
    m->timer++;
    return 0;
}
