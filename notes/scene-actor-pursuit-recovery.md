# Pursuit after contact release

Tags: #scene-actors #matching 0807DB34

The guarded route recovery now uses the same arrival routine as Mummy_ResumePlayerPursuit.
It releases an owned contact, installs the delayed response, then uses either the
cell-center route or the destination supplied by the pursuit group. The stock check
improved from 36.0% to 73.0%. The inlined route, failure and turn paths still differ.
