# AGENTS.md — m2dev server source

Read the workspace rules in `H:\m2dev-client\AGENTS.md` and the canonical
workflow in `H:\m2dev-client\m2dev-docs\docs\specifications\development-workflow.md`.

## Scope and authority

- This repository owns C++ `game`, `db`, `qc`, quest VM and shared libraries.
- It is authoritative for server state transitions, packet headers/structs,
  GD/DG contracts and C++ quest API registration.
- Runtime CONFIG, SQL, Lua quests and locale belong to `m2dev-server`.
- Never use `Desktop\server`, `OLD`, vendor code or backup copies as parity sources.

## Working rules

- Start with `git status --short`; preserve all unrelated dirty changes.
- For a packet change inspect classic client and UE5 producers/consumers before
  editing. Confirm field widths, packing, phase, direction and total length.
- Validate client input before mutating state. Treat auth, item, trade, guild,
  shop and GM command changes as security-sensitive.
- For persistence trace the complete `game -> GD -> db/cache -> MySQL -> DG`
  path and update runtime SQL/migrations when required.
- For quest API changes update registration, Lua use sites and runtime quests.

## Validation

```powershell
cmake -S . -B build
cmake --build build --config Debug --target game db qc
```

There is no first-party automated suite: do not describe a build as a test.
Build every changed target and run the relevant scenario in `m2dev-server`.
Packet changes require binary fixtures and code-map validation.
Use a separate, atomic commit for this repository and link the shared WORKLOG ID.
