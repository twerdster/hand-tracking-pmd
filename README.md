# Hand tracking with PMD depth images

<p align="center">
  <img src="assets/project-preview.jpg" alt="Four depth-image panels of a hand: a tracked point marked on the hand, a close-range depth view, a red hand contour with interior points, and a binary hand segmentation" width="255">
</p>

*Original project image, unchanged.*

This MATLAB/C++ MEX project estimated hand positions from close-range PMD CamBoard nano depth frames, using seed selection and region segmentation.

Student: Sa'ar Zehavi. Supervision: Aaron Wetzler.

![Platform: MATLAB + PMD CamBoard nano](https://img.shields.io/badge/platform-MATLAB%20%2B%20PMD%20CamBoard%20nano-lightgrey)
![Code: MATLAB + C++ MEX](https://img.shields.io/badge/code-MATLAB%20%2B%20C%2B%2B%20MEX-blue)
![Source: original files recovered](https://img.shields.io/badge/source-original%20files%20recovered-blue)

## Original

Nine source files survive unchanged, along with the June 2013 report and an unfinished slide deck. The MATLAB/PMD dependencies and depth recordings are missing, so no build or tracking run of the original code is recorded. The [recovery notes](docs/RECOVERY.md) record historical requirements and attribution; see also [PROVENANCE.md](PROVENANCE.md).

## Updated

[**Run the browser demo**](https://twerdster.github.io/hand-tracking-pmd/)

A separate browser demo, written in November 2026, generates a hand-shaped depth frame. Drag the hand, change its depth and the segmentation cutoff, inspect the resulting connected region and export its pixel centroid. It uses a simple depth threshold with connected components; it does not port the seed-based 3D tracker, estimate joints or indicate PMD camera accuracy. The recovered original files are unchanged.

### Run locally

```sh
python3 -m http.server 8000 --directory docs
```

Then open http://localhost:8000. No build step or external service is required.

### Checks and licensing

Browser-check results, implementation notes and limits are in [updated/README.md](updated/README.md). These checks apply only to the new demo; they do not validate the historical code or any camera. The [MIT license](docs/DEMO-LICENSE.txt) covers only the new demo; original materials keep their existing attribution and terms.
