# Video development

The [19 generated keyframes](../visuals/keyframes/README.md) now cover all ten
scenes. Open the [ordered gallery](../visuals/keyframes/index.html) to review them.
They were made entirely with the image-generation tool, including corrections.
The gallery includes transition notes for the planned Manim reconstruction.

The sequence below connects the visual examples to the measured results.
No Manim animation has been rendered.

## Audience and structure

Assume high-school algebra, averages and basic geometry. Introduce each new
term through an example before relying on it. The viewer does not need prior
knowledge of matrices, numerical solvers, evolutionary search or graph storage.

The order is deliberate:

1. Ask how to solve a grid whose numbers affect their neighbors.
2. Show four simultaneous equations and a checkable answer.
3. Use an incorrect guess to explain residuals and correction directions.
4. Turn simple arithmetic into a search over six-instruction programs.
5. Show how reusing computation makes more experiments affordable.
6. Read an actual formula produced by the search.
7. Explain the uniform error through a shift the neighbors cannot detect.
8. Test the formula against changed systems and strong controls.
9. Retain successes and failures as connected evidence records.
10. Return to the opening question with a precise account of the result.

This follows broad principles in
[Grant Sanderson's explanation advice](https://www.3blue1brown.com/about/).
The scene order is original. There is no affiliation with 3Blue1Brown.

## Manim target

Use [ManimCommunity/manim](https://github.com/ManimCommunity/manim).
The latest stable release checked on 21 September 2026 was
[v0.21.0](https://github.com/ManimCommunity/manim/releases/tag/v0.21.0).
The main branch at that check was `485c226168e9c189512b22468de89b18dbc1780e`.
Start the eventual renderer from the stable release and record its environment.

| Scenes | Intended Manim objects and transitions |
|---|---|
| 1 and 2 | Points, lines, numeric labels and a moving camera; preserve the same four-point layout |
| 3 | Equation terms built from `MathTex`; move matching symbols into the compact notation |
| 4 | Six grouped instruction slots, highlighted connections and a mutation of one field |
| 5 | Merge duplicate active-program cards into one evaluation, then return the result to both |
| 6 | `TransformMatchingTex` from the active expression into the three-term explanation |
| 7 | Shared vertical motion, fixed difference segments, then a two-component error diagram |
| 8 | Fixed result rows revealed after each definition; read values from retained JSON |
| 9 | Directed graph of candidate, condition and observation nodes; merge with stable positions |
| 10 | Return to the original four-point picture and the now-familiar correction colors |

Relevant references: [matching equation transforms](https://docs.manim.community/en/stable/reference/manim.animation.transform_matching_parts.TransformMatchingTex.html),
[graph objects](https://docs.manim.community/en/stable/reference/manim.mobject.graph.Graph.html),
and [moving-camera scenes](https://docs.manim.community/en/stable/reference/manim.scene.moving_camera_scene.MovingCameraScene.html).

## Visual continuity

Use a dark background, ivory labels, coral residuals, teal local corrections,
gold previous steps and lavender uniform corrections. Retain object positions
and colors as the explanation develops. Reserve green for a passed check.
Introduce one important transformation at a time and leave pauses for prediction.

## Before animation

Verify that a viewer can explain the uniform-shift example before adding
detail. Set animation timing from the narration, then construct the
mathematical motion in Manim. Measured plots and counts should come from the saved data, with image
generation used for art direction.
