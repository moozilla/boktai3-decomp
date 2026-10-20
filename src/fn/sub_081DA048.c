#include "global.h"

struct TCollider {
    u8 pad0[0xA];
    u16 flags;
};
struct TConfig {
    u16 flags;
    u8 pad2[0x48 - 2];
    s32 type;
};
struct TEntry {
    u8 pad0[0x18];
    struct TConfig config;
};

void Projectile_SetUpdateCallback(void *, void *);
void sub_081D9904(void);
void sub_081D934C(void);
void sub_081D7C68(void *);

void sub_081DA048(struct TCollider *self, struct TCollider *other, struct TEntry *entry)
{
    struct TConfig *config = &entry->config;
    struct TCollider *o;
    struct TEntry *e;

    if (config->type == 3) {
        (*(&entry))->config.flags |= 0x80;
        Projectile_SetUpdateCallback(config, sub_081D9904);
    } else if (config->type != 8) {
        if (config->type == 9) {
            Projectile_SetUpdateCallback(config, sub_081D934C);
        } else {
            e = entry;
            if ((self->flags & 1) || (other->flags & 1)) {
                o = other;
                if (!(o->flags & 0x20)) {
                    e->config.flags |= 0x80;
                    sub_081D7C68(e);
                }
            }
        }
    }
}
