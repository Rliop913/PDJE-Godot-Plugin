# Known Gaps

- The harness documents current wrapper boundaries but does not replace
  upstream `Project-DJ-Engine/AGENT_DOCS/`.
- `WRAPPING_STRATEGY.md` describes current wrapper code conventions. It is not
  an API stability guarantee and not an upstream engine design document.
- Build validation is intentionally procedural here. Documentation-only changes
  should be verified statically unless the user asks for a build.
- Godot scene behavior is only mapped at a high level. Add scene-specific notes
  if repeatable editor or runtime workflows become part of normal maintenance.
- Input/judge availability is documented from current CMake conditions; revisit
  this page if platform support changes.
- Generated local data directories are listed as non-editable, but exact cleanup
  policy is not defined here.
