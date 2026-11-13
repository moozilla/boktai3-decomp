# Spread-shot cleanup draft

Tags: #matching #scene #particles #structs #compiler

Status: inspection only at the worker stop request. No source or symbol changes were made for 08122DF0–08125B64. The existing matching implementations remain intact. These are provisional layout notes for resuming phase 4, not a checked C implementation.

## Object boundaries

- 08122DF0 is a trailing empty scheduled-effect shutdown hook.
- 08122DF4–08123194 is the 32-slot sprite-notice pool. Each slot is 0x48 bytes; the task has sequence and graphics pointers at 0x18/0x1C, slots at 0x20, and occupied mask at 0x920. Its implementation can reuse `SceneParticleEffect`, `DisplaySequenceState`, and `Task`.
- 08123194–08125B64 contains the terrain probe and two-slot spread-shot object, interleaved with notice interpolation and empty callbacks. Keep a contiguous translation unit rather than moving the interpolation helper out across the probe. The existing probe comment points to `notes/cloud-08122DF4.md` for GCSE/PRE-sensitive details; read that before changing its expressions.

## Proposed shared layouts

The spread-shot constructor and step agree on the following layout. This draft has not been compiled or checked:

```c
struct SpreadShot {
    struct SceneActorBody body;           /* 0x00 */
    struct ContactSwarmSprite sprite;     /* 0x48 */
    struct ContactPosition position;      /* 0x74 */
    struct ContactPosition velocity;      /* 0x7C */
    struct ContactCollider collider;      /* 0x84 */
    u16 hits;                             /* 0xD8 */
    u8 kind, reset;
    u16 cell;                             /* 0xDC */
    s8 index;
    u8 state;
    u16 timer, unknownE2;
};
struct SpreadShotPool {
    struct Task task;
    struct SpreadShot entries[2];         /* 0x18 */
    u32 allocated;                        /* 0x1E0 */
};
```

The probe's `vx`/`vz` accesses are `velocity.x`/`velocity.z`. The step's `spritePosition` at 0x0C is the body's position, not the display sprite's position. The constructor's collider prefix agrees with the shared `ContactCollider` layout. The pool update's word index 0x78 is the allocated mask at 0x1E0.

A sprite-notice slot begins with a 0x28-byte `SceneParticleEffect`, then 0x10-byte `DisplaySequenceState`. The bytes from 0x38 through 0x41 remain unclassified. State is at 0x42, signed slot index at 0x43, and entered/restore at 0x44; the full stride is 0x48. Preserve the notice constructor's three-halfword position copy until its alignment behavior has been checked.

## Next checks

Start by unifying the notice pool and testing its complete range. Then recover the spread-shot layout above and check the large terrain probe separately before merging. Its existing comment identifies expression hash size and pseudo numbering as sensitive to restructuring. Do not import the status-indicator unit at 08126650 or any later unit: those were reassigned to the cloud worker. The moving-particle range 08125B64–08126140 was not started by this worker.
