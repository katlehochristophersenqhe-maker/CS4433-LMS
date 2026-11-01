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
5. When a release is ready, tag the merged commit on `main`:
   ```bash
   git checkout main
   git pull --ff-only origin main
   git tag -a v1.0 -m "Release v1.0"
   git push origin v1.0
   ```
   Create and push a release tag only when that version has actually been
   prepared for release.

For an urgent fix, create `hotfix/<name>` from `main` and follow the same pull
request workflow.

## Commit message style

- `Add Catch2 late-penalty unit tests`
- `Fix empty submission validation`
- `Docs: installation guide for school technician`

Avoid messages like "update" or "fix" with no detail.

## Planned tags

| Tag | Meaning |
|-----|---------|
| `v1.0` | Intended first production release; create when ready. |
| `v1.1` | Possible future maintenance release. |
