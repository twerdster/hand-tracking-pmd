# Updated browser example

Added 17 November 2026 as a new, project-specific interaction. It is separate from the recovered software and is not the student's implementation.

## What runs

Move a generated hand shape, change depth and cutoff, inspect a connected region and export its pixel centroid.

The original algorithm uses seed selection and 3D distance constraints; this separate demo uses a simpler 4-connected depth threshold to make one segmentation step visible. The 160 × 120 generated frame contains one hand shape over a flat background. No tracking accuracy, PMD compatibility or real-camera performance is claimed.

## Run

Open https://twerdster.github.io/hand-tracking-pmd/ or serve the repository's `docs/` directory with any static HTTP server. For example, from the repository root:

```sh
python3 -m http.server 8000 --directory docs
```

Open `http://localhost:8000`. The demo uses standard Canvas 2D and local browser controls. No additional demo packages, SDKs or credentials are required. Downloads are generated JSON files. No camera permission is requested.

## Implementation

`docs/index.html` contains the controls and factual scope labels; `docs/demo.js` implements the interaction; `docs/demo.css` contains the responsive layout. All generated images/data are illustrative. It does not port the seed-based tracker, estimate joints or establish PMD-camera accuracy.

## Recorded checks

[VALIDATION.json](VALIDATION.json) records 10 passed browser assertions on the new interaction, JSON export where present, local-only requests, error-free JavaScript and a 390-pixel responsive layout. These are interaction assertions on generated/local examples, not accuracy or compatibility benchmarks. Original native application/hardware coverage is unchanged; see [historical recovery notes](../docs/RECOVERY.md).

## Attribution and terms

Original project: Sa'ar Zehavi. Supervision: Aaron Wetzler. The new browser example was prepared with Codex assistance in November 2026; Claude reviewed the README wording and factual scope. The new files have a [scoped MIT license](../docs/DEMO-LICENSE.txt); it does not relicense historical code, images, slides, reports or third-party dependencies.
