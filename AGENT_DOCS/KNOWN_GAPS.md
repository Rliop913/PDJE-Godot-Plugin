# Known Gaps

- The harness documents current wrapper boundaries but does not replace
  upstream `Project-DJ-Engine/AGENT_DOCS/`.
- `WRAPPING_STRATEGY.md` describes current wrapper code conventions. It is not
  an API stability guarantee and not an upstream engine design document.
- Build validation is intentionally procedural here. Documentation-only changes
  should be verified statically unless the user asks for a build.
- Interactive scene behavior remains manual. Util GDExtension registration and
  API behavior have repeatable headless coverage under `tests/headless/`.
- Input/judge availability is documented from current CMake conditions; revisit
  this page if platform support changes.
- Generated local data directories are listed as non-editable, but exact cleanup
  policy is not defined here.
