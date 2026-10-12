#include "global.h"
struct Object {
  u8 pad0[9];
  u8 angle;
  u8 pada[0x42];
  int health;
  u32 flags;
  u8 pad54[4];
  int damage;
  u32 source, attackFlags;
  u8 pad64[2];
  u16 reaction;
  u8 pad68[0x118];
  u8 facing, facing8;
  u8 pad182[0xc6];
  int maxHealth;
};
struct Collider {
  u8 pad0[0x34];
  u32 source, flags;
  u8 pad3c[2];
  u16 damage;
  u8 pad40[4];
  u16 reaction;
};
void sub_0802C96C(struct Object *p, struct Collider *self,
                  struct Collider *other) {
  if (!p->damage) {
    p->damage = other->damage;
    p->source = self->source;
    p->attackFlags = self->flags;
    {
      u32 value = self->reaction;
      p->reaction = value;
    }
  }
}
