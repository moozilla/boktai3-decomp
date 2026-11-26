# Scene actor placement cleanup

Tags: #matching #types #phase3 #scene

Placement diagnostic lines fell from 94 to 84 (10.6%). Charged constant uses
fell from 94 to 81 (13.8%) with zero errors. Every touched translation unit
matches under the stock compiler. Exact code coverage remains 100%, a change
of zero percentage points.

- Actor status reads the local participant from the canonical link-session
  type. It replaces the old integer declaration and enables the shared
  defeat-record operation in 08075588. All 23 header dependents match.
- Actor height now uses the existing terrain query (08074A98), and the
  route position (0807E7DC) uses ContactPosition and named chase route fields.
  The animation update (08074EB0) and resource release (0807E908) use
  declared sprite and actor types.
- Route advance and actor reset (08074698, 080772C0) reuse the existing
  state and control transitions. The two shake amplitudes (080776E4)
  share one operation that updates the drawable position on both axes.
- The placement visibility tasks (08136C80, 08136DFC) use the established
  sequence and tile visibility operations. The two November 22 staged word
  conversions restore explicit locals where direct expressions differ.

The duplicate slope branches at 08076910 retain the ROM's branch layout.
Consolidating them changes comparison and branch sharing. The old pin marker
was removed after confirming that the shared height helper uses ordinary C.
The direct RNG forms in 080776E4, 08077C48 and 0807E6FC change the sample
address or register computations. The exact pointer handling remains.
