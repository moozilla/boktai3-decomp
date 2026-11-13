# Crow unit cleanup

Tags: Crow, 0809ED00, 0809F510, 0809FC74, 080A0364, 080A0E00, 080A1440, phase4

Consolidated 40 files from 0809ED00 through 080A2048 into `src/crow.c`.
The final formatted unit matches all 13128 bytes.

The shared header describes the complete 0x43C allocation: animation phase,
steering duration and speed, orbit range, home cell and height, a saved motion
subject, three vectors, and the initial position. Existing actor, sprite,
sequence, collider, and terrain types replace numeric offsets and per-function
padded views. The reset-view prefix is repeated because its canonical type
continues past this species allocation. No shared engine header changed.

Five routines receive consistent Crow names, including attack contact, defeat,
periodic/sequence animation, and height return. Existing meaningful names remain.
Private availability and ordinary control operations use shared actor helpers;
unused inherited helpers and declarations were removed.

Deciding evidence: the sprite Y field is interpreted as unsigned for animation
height adjustments, as permitted by the shared sprite definition. A signed read
adds eight bytes. The wander flag predicate needs the explicit if/return form:
a boolean expression changes branch layout and literal placement. Typed terrain
cell lookup, saved movement, and the full context all preserve exact output.
The tiny pre/post-update wrappers now explicitly pass their incoming actor,
matching the original calls without relying on incompatible declarations.

Validation: all 40 routines checked, fixes rechecked individually, and the final
merged/formatted unit MATCH. No new concessions and no ROM build.
