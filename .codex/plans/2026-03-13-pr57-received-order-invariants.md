## 1) Goal and context

- Ticket/branch: PR #57 on `feature/full-mvp-development`
- Problem statement: New review threads report purchase-order integrity gaps after receipt posting. A received order can be reopened through `editOrder`, and receipt-posted line items can still be added/edited/deleted, which can desynchronize inventory from purchase history.
- Intended outcome: Enforce server-side invariants so receipt-posted purchase orders cannot be reopened or have line items mutated, add regression coverage, amend the fixes into the relevant historical commits, and force-push the updated branch.

## 2) Scope

- In scope:
  - `_server/src/BusinessLogic/PurchaseModule.*`
  - `_server/tests/unit/ErpModulesTest.cpp`
  - `_server/tests/component/HttpContractTest.cpp`
  - `.codex/plans/2026-03-13-pr57-received-order-invariants.md`
- Out of scope:
  - broader purchase UX redesign
  - docs/status updates beyond what is required for this focused review-fix
  - unrelated PR comments already addressed in earlier rewrites

## 3) Current behavior (baseline)

- `receiveOrder` already normalizes `Received` checks and posts stock transactionally.
- `editOrder` still accepts request status changes even when the stored order is already received.
- `addOrderRecord`, `editOrderRecord`, and `deleteOrderRecord` do not guard against parent orders that are already received.
- Relevant files/functions:
  - `_server/src/BusinessLogic/PurchaseModule.cpp`
  - `_server/tests/unit/ErpModulesTest.cpp`
  - `_server/tests/component/HttpContractTest.cpp`

## 4) Target behavior (DoD)

- [ ] Build succeeds for touched targets
- [ ] Relevant tests pass
- [ ] No regression in purchase receipt flow
- [ ] Compatibility expectation documented: no API shape change; stricter server-side validation only

## 5) Files to touch

| File/Dir | Why |
| --- | --- |
| `_server/src/BusinessLogic/PurchaseModule.cpp` | enforce received-order invariants |
| `_server/tests/unit/ErpModulesTest.cpp` | add focused unit regressions |
| `_server/tests/component/HttpContractTest.cpp` | add HTTP contract regressions |
| `.codex/plans/2026-03-13-pr57-received-order-invariants.md` | required ExecPlan |

## 6) Interfaces and compatibility

- External interfaces impacted: existing purchase endpoints now reject more invalid state transitions
- Backward compatibility: compatible at the API-contract level; behavior becomes stricter for invalid edits/mutations after receipt
- Migration notes: none

## 7) Approach (implementation steps)

- [ ] Add helper logic in `PurchaseModule` to load/normalize parent order state and reject forbidden post-receipt mutations
- [ ] Cover the new invariants in unit and contract tests
- [ ] Run focused validation, then amend the relevant `mvp` and `test` commits via `rebase -i`
- [ ] Force-push the rewritten branch

Notes:

- Prefer the smallest safe diff in `PurchaseModule`.
- Keep the fix server-side so all API consumers get the same protection.

## 8) Risk review

| Risk | Impact | Mitigation/test |
| --- | --- | --- |
| Rejecting a previously accepted edit path | existing invalid workflows now fail | unit + contract regressions for received-order edits and line mutations |
| Missing one mutation path | inventory/order history can still diverge | cover add/edit/delete line-item operations explicitly |
| Rebase misplaces fixes | history no longer matches change intent | map code changes to `mvp` and tests to `test`, then rerun focused validation before push |

## 9) Verification plan

- `cmake --build build --target ErpModulesTest HttpContractTest --parallel`
- `ctest --test-dir build -R "Unit_ErpModulesTest|Component_HttpContractTest" --output-on-failure --verbose`
- `git diff --check`

## 10) Progress log

- [22:19] Reviewed new PR #57 threads and mapped them to purchase-order invariants in `PurchaseModule`.
- [23:21] Implemented server-side guards for received-order edits, deletes, and line-item mutations; added direct `Received` transition rejection for order add/edit.
- [23:24] Focused validation passed: build of `ErpModulesTest` and `HttpContractTest`, plus targeted `ctest` run for both.

## 11) Final review

- What changed vs plan: Kept the fix entirely on the server plus server test layers; no purchase UI changes were needed to satisfy the correctness issues in PR #57.
- What was verified: `cmake --build build --target ErpModulesTest HttpContractTest --parallel`, `ctest --test-dir build -R "Unit_ErpModulesTest|Component_HttpContractTest" --output-on-failure --verbose`, and `git diff --check`.
- Remaining risks/follow-ups: The desktop purchase form still exposes status editing affordances, so invalid post-receipt edits now fail server-side rather than being prevented client-side. Correctness is protected; UX tightening can be done separately if needed.
