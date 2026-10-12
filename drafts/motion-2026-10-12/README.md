# Motion family checkpoints, October 12

Nonmatching authored source; zero exact credit. See docs/MOTION_0818.md.

- typed_vector.c: DB10, target 320 bytes, emitted 324, 32 differing bytes.
- sibling_vector.c: F5B68, target 308 bytes, emitted 304, 59 differing bytes.

Both improve the calculation prefix through typed signed/unsigned vector
fields. Final status writes remain unresolved. attempts.json retains failed
hypotheses and recipe/hash metadata, with no assembly or ROM bytes. Absolute
source paths identify original local provenance, not required checkout paths.
The sibling_signed_flags padding change was erroneous: callback224 was already
at +224. Discard that variant; the selected sibling keeps the correct layout.

Recheck with tools/candidate.py and the target sizes above. Main source must
remain exact; do not merge this drafts directory into main.
