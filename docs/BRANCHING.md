# Branching strategy

Repository: https://github.com/katlehochristophersenqhe-maker/CS4433-LMS

This project currently uses `main` as its integration branch; there is no
`develop` branch. Feature and hotfix branches are merged into `main` by pull
request.

## Branches

| Branch | Purpose |
|--------|---------|
| `main` | Integration branch and source for releases; merge changes via PR. |
| `feature/<name>` | One feature or fix (e.g. `feature/unit-tests`). |
| `hotfix/<name>` | Urgent fix; merge into `main` via PR. |

## Workflow

1. Create a feature branch from the latest `main`:
   ```bash
   git switch main
   git pull --ff-only origin main
   git switch -c feature/my-change
   ```
2. Commit with a clear message explaining what changed and why.
3. Push the branch and open a pull request into `main`.
4. Review the changes and CI results, then merge the pull request.
5. When a release is ready, tag the merged commit on `main` using that
   release's version:
   ```bash
   git checkout main
   git pull --ff-only origin main
   git tag -a <version> -m "Release <version>"
   git push origin <version>
   ```
   Create and push a release tag only when that version has actually been
   prepared for release.

## Published tags

| Tag | Commit | Status |
|-----|--------|--------|
| `v1.0` | `39e592c27ea6cbc388cf7690ae3ddee04d53c95b` | Published |
| `v1.1` | `81afa92567b43348c17821092a37c328f0df8c2c` | Published; includes CR1 |

For an urgent fix, create `hotfix/<name>` from `main` and follow the same pull
request workflow.

## Commit message style

- `Add Catch2 late-penalty unit tests`
- `Fix empty submission validation`
- `Docs: installation guide for school technician`

Avoid messages like "update" or "fix" with no detail.
