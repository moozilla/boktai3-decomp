# Volatile typedef detection in the concession scanner

Tags: #tooling #matching #progress

`tools/concessions.py` now recognizes scalar locals declared through the project's
volatile integer and Boolean typedefs (`vu8` through `vu64`, `vs8` through `vs64`,
and `vbool8` through `vbool32`). The previous scanner recognized explicit
`volatile` declarations and some pointer casts, but missed `vu32 zero;` in
`PlayerClone_UpdateBehavior` during the cleanup run.

Detection uses the existing top-level C item parser to restrict these new matches
to function definitions and function-like macros. Top-level register layouts,
globals, typedef declarations, and pointers to volatile hardware stay excluded.
Comments and string contents do not create matches. Line numbers, library
exemptions, explicit concession allowances, and kind filtering are preserved.
A redundant explicit `volatile` qualifier does not count the same declaration
twice.

The landing guard now detects a change from `u32 zero` to `vu32 zero` as an added
concession. Progress scoring recognizes both direct declarations and calls to
inline helpers containing them. No game source changed, no concessions were
introduced, and no checked-in progress reports were regenerated.

Validation: 20 concession tests, 10 landing tests, and 22 progress tests pass.
The declaration tests exercise all eleven typedef names, initialization, arrays,
multiple declarators, const qualifiers, duplicate volatile qualifiers, macros,
hardware pointers, top-level layouts, comments, strings, allowances, and library
exemptions.

A read-only audit of this worker's source snapshot found fourteen newly detected
lines: thirteen in ten source functions, and one in the shared `ZeroFill8` helper
in `include/global.h`. The source functions include the Player clone behavior
update and 08093810, 08248778, 08163C84, 080697C8, 08176F70, 081F2208, 0807D2E0,
080747B8, and 080FE3B0. The EEPROM function at 08248778 contains four declarations.
These are existing source sites, not newly added volatile objects. Callers of
`ZeroFill8` can consequently receive a concession score when the coordinator
regenerates progress; the existing allowance mechanism remains available for
cases justified by original behavior.

The narrowed cleanup queue remains complete: all eighteen units before Nav mesh
at 0821F15C were consolidated. The scanner fix is a separate tooling commit after
that queue, as requested by the worker brief.
