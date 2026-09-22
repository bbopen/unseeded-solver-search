# Opening sequence — 33 seconds

Six image-generated frames form five connected clips. Each last frame is reused verbatim as the next first frame. Narration is preserved exactly. Durations are editorial targets before a recorded voiceover, with room for visual pauses.

| Clip | Span | Length | Narration | Frames |
|---|---|---|---|---|
| 1 | 0–8 s | 8 s | Suppose I give you a grid of numbers, and a rule that each number has to satisfy. | [frame-00.png](frame-00.png) → [frame-01.png](frame-01.png) |
| 2 | 8–13 s | 5 s | You can change any of the numbers. | [frame-01.png](frame-01.png) → [frame-02.png](frame-02.png) |
| 3 | 13–20 s | 7 s | But changing one also affects the rules at its neighbors. | [frame-02.png](frame-02.png) → [frame-03.png](frame-03.png) |
| 4 | 20–28 s | 8 s | So a move that fixes one part of the grid can disturb another. | [frame-03.png](frame-03.png) → [frame-04.png](frame-04.png) |
| 5 | 28–33 s | 5 s | Your task is to get all the rules to agree. | [frame-04.png](frame-04.png) → [frame-05.png](frame-05.png) |

## Animation instructions

### Clip 1

Hold the four numbers for one second. Introduce the four discrepancies, then highlight the first rule. Hold the end frame for one second.

```text
Generate a 8-second 16:9 animation in Grant Sanderson / 3Blue1Brown style using the attached first and last frames. Precise Manim-like geometry, deliberate smooth motion and clear visual teaching. Keep camera fixed and preserve node identities, values, colors and readable typography. Hold the four numbers for one second. Introduce the four discrepancies, then highlight the first rule. Hold the end frame for one second. Narration, verbatim: "Suppose I give you a grid of numbers, and a rule that each number has to satisfy." Deliver the sentence naturally with a short visual pause. Match the supplied endpoints. Keep the same narrator and audio character across all five clips.
```

### Clip 2

Select the upper-left number and change it from 0 to −0.5. Update every discrepancy simultaneously to its correct value; keep neighbor labels muted so focus stays on the selected number.

```text
Generate a 5-second 16:9 animation in Grant Sanderson / 3Blue1Brown style using the attached first and last frames. Precise Manim-like geometry, deliberate smooth motion and clear visual teaching. Keep camera fixed and preserve node identities, values, colors and readable typography. Select the upper-left number and change it from 0 to −0.5. Update every discrepancy simultaneously to its correct value; keep neighbor labels muted so focus stays on the selected number. Narration, verbatim: "You can change any of the numbers." Deliver the sentence naturally with a short visual pause. Match the supplied endpoints. Keep the same narrator and audio character across all five clips.
```

### Clip 3

Keep values fixed. Trace the top and left connections, then highlight the two neighboring discrepancies. Hold so the viewer can follow both connections.

```text
Generate a 7-second 16:9 animation in Grant Sanderson / 3Blue1Brown style using the attached first and last frames. Precise Manim-like geometry, deliberate smooth motion and clear visual teaching. Keep camera fixed and preserve node identities, values, colors and readable typography. Keep values fixed. Trace the top and left connections, then highlight the two neighboring discrepancies. Hold so the viewer can follow both connections. Narration, verbatim: "But changing one also affects the rules at its neighbors." Deliver the sentence naturally with a short visual pause. Match the supplied endpoints. Keep the same narrator and audio character across all five clips.
```

### Clip 4

Keep current values fixed. Reveal before/now comparisons one at a time: the selected discrepancy is closer to zero, while both neighboring discrepancies are farther away. Use emphasis, not another numerical update.

```text
Generate a 8-second 16:9 animation in Grant Sanderson / 3Blue1Brown style using the attached first and last frames. Precise Manim-like geometry, deliberate smooth motion and clear visual teaching. Keep camera fixed and preserve node identities, values, colors and readable typography. Keep current values fixed. Reveal before/now comparisons one at a time: the selected discrepancy is closer to zero, while both neighboring discrepancies are farther away. Use emphasis, not another numerical update. Narration, verbatim: "So a move that fixes one part of the grid can disturb another." Deliver the sentence naturally with a short visual pause. Match the supplied endpoints. Keep the same narrator and audio character across all five clips.
```

### Clip 5

Transition from the current guess to the known teaching solution [1,2,3,2]. Bring all four discrepancies to zero together and hold for the final second. This depicts the goal, not execution of the discovered solver.

```text
Generate a 5-second 16:9 animation in Grant Sanderson / 3Blue1Brown style using the attached first and last frames. Precise Manim-like geometry, deliberate smooth motion and clear visual teaching. Keep camera fixed and preserve node identities, values, colors and readable typography. Transition from the current guess to the known teaching solution [1,2,3,2]. Bring all four discrepancies to zero together and hold for the final second. This depicts the goal, not execution of the discovered solver. Narration, verbatim: "Your task is to get all the rules to agree." Deliver the sentence naturally with a short visual pause. Match the supplied endpoints. Keep the same narrator and audio character across all five clips.
```

## Numerical continuity

The teaching equations remain those in the main script. Clockwise values begin at [0,0,0,0], change to [−0.5,0,0,0], then end at [1,2,3,2]. Their residuals are [−1.9,0.2,2.3,0.2], [−0.85,−0.3,2.3,−0.3], and [0,0,0,0]. Clips 3 and 4 only change emphasis; they do not apply another move. The final transition illustrates the goal, not a measured solver trajectory.

All six PNGs were produced with image generation. No video generation was submitted in this step. These opening assets supersede the earlier condensed opening for narration planning. The other scenes are covered by the main keyframe guide.
