# Sprite unit cleanup handoff

Tags: #assets #animation #matching #process

The weekly run ended during the revisit of Sprite, 082142DC–08215698 (37
single-function files). This unit remains unchanged and unconsolidated. There is
no candidate C draft or claimed match from this revisit. The observations below
are the saved design work; the next worker should validate the proposed types
against the stock compiler before merging the files.

## Shared layouts

`DisplaySequenceSprite` in `include/display_animation.h` is the existing sprite
object. Its bytes at offsets 8 and 9 are called `palette` and `paletteVariant`,
but the piece renderer at 08214600 and list renderers read them as signed X/Y
scales. The layout observed across the unit is flags at 0, linked at 4, visible
at 5, rotation at 6, priority at 7, scale X/Y at 8/9, list index at A, resource
pointer at C, eight bytes of animation state at 10, attributes at 18,
`ContactPosition` at 1C, and previous/next links at 24/28. Audit external field
uses before renaming or changing signedness in the shared header.

The effect list helpers at 082143CC and 082143F8 can use the complete
`SpriteDrawable` from `include/sprite_oam.h`: linked at 4, list at 11, and
previous/next at 20/24. The animation list helpers at 08214424 and 08214450 can
use the now-expanded `SceneInputWidget`: linked in `unknown4[0]`, list at 1C,
and previous/next at 58/5C. Those complete types were recovered and validated
in the earlier Sprite OAM and Anim cleanup commits.

The resource views need a single shared definition. The renderers observe a
palette key at 6, width/height at 8/9, signed offsets at A/B, palette pointer
at C, and frame records through the pointer at 14. `AnimData_Load` at 082151E4
also fills fields at 0–4 and a data pointer at 10. `Sprite_CopyRenderState` at
0821452C copies a 28-byte resource block, so preserve the trailing word rather
than adopting the shorter private view. The existing display resource wrappers
must be audited for whether they represent this exact object or a larger owner.

## Prototype and code-generation checks still needed

`Sprite_Init` (082144A4) and `Sprite_InitUnlinked` (082144E4) currently declare
and call `Sprite_ApplyAnimation` with one argument. The definition at 08214588
reads a second argument, the resource pointer. Both initializers already receive
a currently unused second parameter. Passing that parameter explicitly is the
likely intended API, but this has not been compiled or proven byte-identical.
Check both initializers first before undertaking the full merge.

`include/engine.h` has generic `void *` sprite APIs and some differing signed
parameter types. Including `display_animation.h` pulls those declarations in;
reconcile them explicitly instead of leaving conflicting prototypes in the unit.
A shared sprite data header separate from the broad engine API may help, but
its consumers must all be checked.

`Sprite_CopyRenderState` copies the animation state as two words and copies the
position words in a particular order. Try shared struct assignments and inspect
the exact result before retaining any minimal word-access adaptation. The four
list renderers repeat projection helpers; consolidate identical helpers only
after their signedness and vector alignment agree. `ContactPosition` is explicitly
four-byte aligned, while some old local vectors contain only three halfwords.

## Adjacent completed work

Anim, 08219574–08219A94, is committed as `src/animation_widget.c`: all 1312 bytes
match, and all 341 external direct/transitive widget-header consumers pass their
per-unit checks. Its 160 bytes of Thumb/ARM call veneers are existing protected
assembly already cataloged in `symbols/progress_asm.csv`; they remain in ROM
order through `INCLUDE_ASM`. No new handwritten instructions were introduced.

The scoped queue ended before Nav mesh, 0821F15C. Seventeen of its eighteen
units were consolidated; Sprite is the remaining cleanup holdout. The coordinator
owns the combined ROM build and integration. No new tooling work was started
after the stop instruction.
