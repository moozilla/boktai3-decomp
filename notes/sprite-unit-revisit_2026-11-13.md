# Sprite unit cleanup

Tags: #assets #animation #matching #process

Sprite, 082142DC–08215698, now lives in `src/sprite.c`: 37 former one-function
files, all 5052 bytes matching stock agbcc. This resolves the cleanup holdout
recorded at the first weekly stop; work resumed on the subsequent continue
instruction.

## Shared layouts and APIs

`DisplaySequenceSprite` moved from `include/display_animation.h` into
`include/sprite.h`, with the same layout and typed previous/next links at 24/28.
Legacy public field spellings remain compatible with external users: `palette`
and `paletteVariant` hold signed X/Y scales in the piece renderer, `attribute`
holds rotation, `unknown4` stores linked/visible, and `unknownA[0]` selects the
list. The renderer casts the scale bytes to their signed interpretation.

The effect list helpers reuse `SpriteDrawable` from `include/sprite_oam.h`.
The animation list helpers reuse `SceneInputWidget` from
`include/scene_input_widget.h`. Private padded list views were removed.

`SpriteRenderResource` unifies the resource views into a 28-byte record with
palette key, bounds, palette/tile/frame pointers, and the trailing word copied
by `Sprite_CopyRenderState`. Piece records, frame records, and the eight-byte
animation state also have shared definitions. The public sprite keeps the
animation fields flat for existing sequence consumers; the copy routine uses a
shared animation-record view beginning at `frame`. Both this aggregate copy and
the `ContactPosition` assignment match the original word-copy instructions.

`Sprite_Init` (082144A4) and `Sprite_InitUnlinked` (082144E4) now explicitly pass
their resource argument to `Sprite_ApplyAnimation` (08214588). Both initializers
matched individually before merging. The data header deliberately has no API
prototypes: external callers still use several incompatible legacy pointer
views and return declarations. Introducing a typed `Sprite_LinkToList` prototype
there broke two consumers; removing that declaration restored compatibility.
The unit itself uses consistent typed prototypes. A repository-wide engine API
cleanup remains for integration rather than changing other workers' callers.

## Structure and names

The four sprite list renderers now use `for` loops and `continue` in place of
skip labels. All four still match. Identical projection helpers are shared. The existing one-iteration list-head
wrappers in two renderers remain: flattening them, replacing them with plain
blocks or a list helper, and moving the load into the for initializer each
changed 47 instructions solely through register selection. Moving the resource
variable to the outer scope did not resolve that allocation difference.
The DMA paths reuse `VideoDmaRegisters` and named hardware constants.
Three old compiler-override comments were removed from the list-head setters;
the stock compiler matches those setters.

Eight formerly unnamed functions now have descriptive CSV names, covering list
head setters, resource replacement, frame-piece rendering, camera-relative list
rendering, tile-slot allocation, and queued tile upload. Existing source aliases
preserve linker references until the coordinator promotes names with all callers.

## Validation and integration

The complete unit matches and has no counted concessions. All 2173 external direct or transitive consumers of the shared sprite header
match. Early consumers were checked again after the header
prototypes and private type names were finalized.

All eighteen units in the narrowed queue are now consolidated. The queue ends
before Nav mesh, 0821F15C; later units belong to another worker. The coordinator
owns the combined ROM build. Anim, 08219574–08219A94, retains only the existing
protected Thumb/ARM veneers cataloged in `symbols/progress_asm.csv`, with all
1312 bytes and 341 external widget-header consumers matching.
