# Keyframes for the explainer

Nineteen separate frames form a ten-scene visual explanation.
Open [the gallery](index.html) to review the sequence. Each image opens at full size.

All images and image corrections were made with the built-in image-generation
tool. No Manim, SVG, canvas, or programmatic image drawing was used. The gallery
only displays the original PNG files. [Prompts and file hashes](prompts.json)
record the selected outputs and correction instructions.

The selected frames are 1672 × 941 pixels, approximately 16:9. They are visual
targets for the next Manim Community pass, not rendered Manim scenes. Use the
composition and palette as references; construct equations, connections and
geometric constraints explicitly. Generated text spacing, subtle shading and
small placement differences are not an exact font or coordinate specification.

## Sequence and motion

| Frame | Narrative beat | Manim construction and transition |
|---|---|---|
| [01a](01a.png) | A coupled puzzle | Four circles, four edges, values and residual labels. Keep node identities clockwise. |
| [01b](01b.png) | One move affects neighbors | Change x1 from 0 to −0.5; residuals become −0.85, −0.3, 2.3, −0.3. Pull back to the 8 × 8 grid. |
| [02a](02a.png) | Build one rule | Move back to the four nodes. Reveal the tether and two neighbor differences before assembling the equation. |
| [02b](02b.png) | Four rules agree | Transform matching equation terms; reveal the four rules and solution values. Residual labels reach zero. |
| [03a](03a.png) | Define residual | Return to the zero guess, collect the four discrepancies into a column vector, then show r = b − Ax. |
| [03b](03b.png) | Define local correction | Build the coefficient matrix from the equations. Highlight its diagonal; transform one residual into a scaled correction. |
| [04a](04a.png) | A program language | Reveal inputs and six ordered slots. This is an illustrative program. Slot references are instruction numbers, not constants. Show only the dependencies used by each instruction as it executes; unused slot 5 can fade. |
| [04b](04b.png) | Evolution and fresh exploration | Move cards through propose, test, score and select. Return mutations to proposals; introduce random proposals separately. |
| [05a](05a.png) | Reuse one evaluation | Merge identical active computations into one evaluator; return the same symbolic score q to both proposals. Reveal recorded CPU figures separately. |
| [06a](06a.png) | Read the discovered formula | Build half the previous step, local correction, then the mean correction. The four residual values below are a teaching example. |
| [07a](07a.png) | A uniform shift cancels | Show the left state first and pause before revealing the shifted state and cancellation. Edge labels use source minus destination along clockwise arrows. |
| [07b](07b.png) | The tether detects the offset | Keep neighbor differences unchanged. Animate tether changes and reveal the distinction between answer error and residual. Tether lengths in this generated frame are schematic. |
| [07c](07c.png) | Decompose mixed error | Use exact vectors [1,3,2,3,1] = [−1,1,0,1,−1] + [2,2,2,2,2]. Build plots with one common scale and aligned baselines. These are illustrative values. |
| [08a](08a.png) | Freeze, then test | Freeze six selected cards before revealing changed systems. Make varying shifts explicit as different tether strengths. Keep the reference answer out of the solver inputs. |
| [08b](08b.png) | Reveal all six outcomes | Reveal rows in script order from the retained report. These are repeated comparisons on the same 48 systems. |
| [08c](08c.png) | Retain strong controls | Explain PCG before revealing its abbreviation. Use neutral type for 12/48 and green only for fully passed rows in the final render. |
| [09a](09a.png) | Preserve the evidence | Reveal candidate, conditions and separate outcomes. Merge local collections; demonstrate duplicate delivery and snapshot restoration. The graph is schematic, not a literal export. |
| [10a](10a.png) | Return to the original puzzle | Restore the four-point layout and familiar term colors. Component arrows are schematic; the formula supplies their relationship. |
| [10b](10b.png) | Ask a more specific question | Follow the loop once, then hold on the repository name. Release availability must be checked before public publication. |

## Visual system

- Background: #0B1019; text: #F3F0E7; structure: #455367.
- Residual: #F47C72; local correction: #5FD1C3.
- Previous step: #F4C66A; uniform correction: #B8A0E8.
- Passed checks: #9FCC8C.
- Use `MathTex` for equations and `Text` for prose; group related objects.
- Use circles, dots, lines, arrows, rectangles, braces and simple axes.
- Keep persistent objects in place while changing only the idea under discussion.

## Review

The images were inspected during generation. Image-tool revisions corrected
the opening move, program-flow arrows, a false attribution on example residuals,
illustrative score tokens, the mixed-error example, and incorrect residual labels
on the solved closing diagram. The displayed result counts and main equations
match the retained results and mathematical examples. No performance curve was invented.

Some generated details still need normalization in Manim: exact common plot
scales, graph-arrow routing, consistent color semantics, tether geometry, and
text margins. Match the visual design while keeping the mathematical objects
correct; do not trace raster defects into the final animation.
