# SceneBody owner validation

The shared `SceneActorBody` record is now used by the engine operations that
initialize it, attach a drawable, and install its movement probe. The API is
in `include/scene_body_api.h`. The canonical record remains in
`include/scene_actor_components.h`; existing used members retain their names,
offsets, and types. The record remains 0x48 bytes, with the actor's next field
at +0x98 when the body begins at +0x50.

## Fields exposed by the engine owners

Offsets in this table are relative to the body, not the containing actor.

| Offset | Storage | Member | Evidence |
| --- | --- | --- | --- |
| 0x00 | word | `id` | First initialization argument is stored here. |
| 0x04 | word | `flags` | Initialization clears it; existing generic body views call this flags. |
| 0x08 | byte | `mode` | Fifth initialization argument. |
| 0x09 | byte | `direction` | Fourth initialization argument. |
| 0x0C | 8 bytes | `position` | An aligned two-word copy from the supplied position. |
| 0x14 | 8 bytes | `motion` | Three halfwords clear; the fourth is set to 0x10. |
| 0x1C | pointer | `movementProbe` | Stored and passed to `CellObject_Init`. |
| 0x20, 0x22 | halfwords | `probeExtentX`, `probeExtentZ` | Third and fourth movement-probe arguments. |
| 0x24 | word | `field24` | Cleared by initialization; no new semantic name inferred. |
| 0x28 | pointer | `currentRoute` | Existing canonical member; initialization clears it. |
| 0x2C | pointer | `sprite` | Initialization clears it; `SetSprite` replaces it. |
| 0x3C | pointer | `owner` | Final initialization argument, stored after registry insertion. |

The fourth halfword of `motion` is active data in this view. Its inherited
`ContactPosition.pad` spelling is retained; this change does not rename that
member across unrelated position consumers or claim that the value is unused.

## Interface contracts

All three functions return the word value 1. `SceneBody_Init` accepts a word
ID, two byte arguments, and a position source. `SceneBody_SetMovementProbe`
accepts two halfword extents. The shared declarations preserve these widths.
The position is a `const ContactPosition *`: initialization copies it, and
the cell initializer reads its X/Z coordinates without modifying it.

The probe's pointer role is established by its use as the destination object
of `CellObject_Init` (0821BC68). The sprite and owner operations in these three
leaves only store a word. Their `void *` types additionally use caller evidence:
the accepted monster initializers pass `actor->drawable` or null to
`SceneBody_SetSprite`, and pass the actor address as the final
`SceneBody_Init` argument. These leaves do not establish a more specific
sprite class or ownership policy, so the API keeps both pointers opaque.

The old declarations represented some of these addresses as `u32`. Replacing
those raw words with the stated pointer contracts introduces no instruction
changes in the three owners or the five adopted pursuit-body callers.

## Verification

The first typed reconstruction of each leaf matched immediately with the
unmodified stock compiler. No search, register constraints, inline assembly,
extra operations, or compiler options were needed.

| Function | Address | Exact bytes | Stock check |
| --- | --- | ---: | --- |
| `SceneBody_Init` | 08225AB4 | 60 | `build/check/run-k8d_bjsf` |
| `SceneBody_SetMovementProbe` | 08225AF0 | 40 | `build/check/run-oybqi82x` |
| `SceneBody_SetSprite` | 08225B28 | 8 | `build/check/run-rg_3q1xc` |

The five callers using the shared pursuit-body constructor also remained
exact after adopting the typed API: Skeleton (4684 bytes), Mummy (4188),
Spider (4184), Ghoul (4092), and Axe (4296). The remaining initializer
prototypes are migrated separately by the integrator, followed by the
combined ROM gate.
