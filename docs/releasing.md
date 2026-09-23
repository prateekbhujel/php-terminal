# Release process

This is a maintainer checklist for 1.x releases. The tag-triggered release
workflow is the source of truth for publication.

## Prepare the release commit

1. Choose the version according to semantic versioning.
2. Change `PHP_TERMINAL_VERSION` in `php_terminal.h`.
3. Rename the top `## Unreleased` changelog section to
   `## X.Y.Z - YYYY-MM-DD`.
4. Update the README "Current release" value and source-build checkout tag.
5. Update compatibility/install notes only if the supported matrix changed.
6. Run `composer validate --strict` and the relevant local tests.
7. Merge the release preparation only after required CI is green.

The release workflow extracts notes from the first `##` section in
`CHANGELOG.md`, so do not leave a new `## Unreleased` section above the dated
release section before tagging. Add a fresh Unreleased section with the next
post-release change.

## Tag

Create `vX.Y.Z` from the exact release commit and push the tag.

The workflow verifies that:

- the tag equals `v` plus `PHP_TERMINAL_VERSION`
- the changelog contains a dated section for that version
- the full CI workflow passes

Do not move an existing release tag after publication.

## Publication

After checks pass, the workflow:

- downloads the Windows artifacts produced by CI
- creates or updates the matching GitHub release
- uploads the Windows archives
- publishes release notes from the dated changelog section

Verify the release page contains the expected Windows x64 TS/NTS artifacts and
that the release is public.

PIE/Packagist metadata comes from `composer.json`; verify the new tag is visible
there after GitHub publication.

## After release

Start the next `## Unreleased` section when the next user-visible change lands.
Keep patch releases backward compatible with the documented 1.x contract. If a
change needs an incompatible public API, plan a major release instead.
