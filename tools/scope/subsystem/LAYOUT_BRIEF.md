You are a worker on a GBA decompilation project (Boktai 3; agbcc = GCC 2.95 Thumb). Your job is phase-4
work on RAM: give each RAM object one name, one shared declaration in a header, and (for struct objects)
one agreed struct layout, while every unit keeps compiling to exactly the same bytes.

You are in your own git worktree ({wt}), branch {branch}. Run every command from its root. Other workers
are matching functions in other worktrees at the same time.

Read first: docs/RAM.md (the inventory, symbols/ram.csv, and `tools/ramlayout.py`), docs/LAYOUTS.md
(`tools/layoutmerge.py`: every file's partial declaration of an object merged into one offset map), and
CLAUDE.md's rules. Field-level ground truth from an outside research round:
~/git/boktai3-decomp/build/gpt_round3/kit/boktai3-round3-research-kit/layout/ (REPORT.md,
ADDRESS_TABLE.md, merged_layouts.json, reviewed_address_map.tsv); treat it as evidence to check, not
as truth.

Your assignment: {category}

How to work, one object at a time, in address order:
1. `python3 tools/ramlayout.py show ADDR` and `python3 tools/layoutmerge.py gUnk_ADDR`: every declaration,
   every file's field names, typed accesses and the ROM evidence. Decide the object's name and type from
   that evidence (names `gCamera`, `gBgRecords`, ...; provisional names are fine, say so in the note).
2. Scalars, pointers and arrays of scalars: `python3 tools/ramlayout.py unify ADDR --decl '...'
   --header HEADER --note '...'` (or --batch for several). It renames, moves the declaration and proves
   with before/after compiles that no file's code changed.
3. Struct objects: write the struct in the header from the merged layout (named fields where any file
   names them, `u8 unkXX[N]` padding elsewhere), then convert consumers to it. Each consumer must still
   check exact: `tools/check.py FILE.c` (and `--nonmatching` for a file with drafts). A shared header
   lands only after every unit that uses it still checks exact. Known case: rewriting the raw +0xB0/+0xB2
   reads in Bg_UpdateScrollShadow as fields gives 172 bytes against the ROM's 180; leave a raw access
   like that as it is and note why. Units that already compile with the struct (witnesses):
   Video_SetRenderCallbacks, Camera_UpdatePositionMode, Background_Update, Sprite_InitSystem,
   Bg_UpdateScrollShadow, Collection_LoadPageBackground.
4. **Files you must not change**: every file listed in build/draft_units.txt (units with NONMATCHING
   drafts that matching workers are editing right now, plus src/iwram_code.c). When ramlayout.py or your
   edits change one of them, put it back (`git checkout -- FILE`), then confirm it still compiles and
   matches as before: `tools/check.py FILE` and `tools/check.py FILE --nonmatching`. Their old `gUnk_`
   name keeps linking from its address. If putting a file back breaks it (a `#define NAME gUnk_X` alias
   that now clashes with the header), drop that object for now and note it.
5. Commit each object (or a small batch of scalars) as one commit. The sandbox cannot write .git, so
   write a commit request: build/commits/NNN.msg (first line `RAM: <what>`, then a short body) and
   build/commits/NNN.files (one path per line: every file you changed, symbols/ram.csv included);
   NNN counts up from 001. A runner outside the sandbox commits it within a minute; check
   `git log --oneline -3` before starting the next object, and never stage or commit yourself.
6. Log one line per object in build/LAYOUT_LOG.md: `ADDR name  unified|struct|skipped  files N  note`, and
   append every ramlayout.py command you ran, exactly, to build/layout_commands.sh (they are replayed
   if integration needs to redo a rename).

Rules: matching means byte-identical, always. No hand-written asm. Never run make, tools/build.py or a
full build; tools/check.py per unit and ramlayout.py's own compiles are the checks (a cold layoutmerge run
takes about 90 s; that is fine). Don't edit tools/. Never `rm -rf` (move to ~/.Trash). Keep going object
after object until your assignment is done or you are stopped; if an object is blocked, record why and
move on.

Your last message: objects unified, structs landed, skipped (with reasons), and what a tool change
would have made this faster.
