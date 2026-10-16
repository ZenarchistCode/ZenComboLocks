# ZenComboLocks — DayZ 1.30 rebuild

This is a 1.30-native replacement for the supplied 1.29 script project.

## Behaviour

- Vanilla 1.30 owns PIN entry, inside/outside lock faces, dial animation, PIN text, and code validation.
- Attaching a lock assigns its owner and clears the previous guest list, matching the old mod.
- Successfully entering the PIN while the associated door is closed:
  - remembers that player;
  - opens the associated gate or door;
  - keeps the lock attached; and
  - immediately returns the lock to its locked state.
- A remembered owner or guest can subsequently use **Open** while aiming at the lock or, when `InteractAnywhere` is enabled, the associated gate/door.
- Native 1.30 digital locks use the same remembered-player system:
  - the player who sets the PIN becomes the owner;
  - each player who subsequently enters the correct PIN is remembered;
  - closing the gate/door automatically re-locks an unlocked digital lock; and
  - remembered players can open the locked gate/door without reopening the keypad.
- Resetting or detaching a digital lock clears its remembered-player list. Bunker puzzle locks are excluded.
- Successfully entering the PIN while the door is already open performs the normal DayZ 1.30 detach operation. This preserves code-based lock removal without restoring the old Manage Lock mode.
- `OwnerCanRemoveLockWithoutCode` gives only the recorded owner (and permitted admins) the separate three-second removal action.
- Bolt-cutter raids work on the lock itself or its associated gate/door and use the configured time and tool damage.

## Retained systems

- Existing CF owner/guest persistence format (`storageVersion = 1`).
- Existing saved player permissions on vanilla and legacy Zen lock entities.
- Server logging.
- Admin opening/removal.
- Instant or simulated-dial-time remembered opening.
- Interact-anywhere opening, closing, removal, and raiding.
- Conditional Building Fortifications and RaG Cabin support.
- `ZenModPack` runtime gating.
- Native 1.30 rebuildable `BuildingBase` doors with per-door lock-slot resolution.

## Removed or replaced

- Custom 5–9-digit behaviour.
- The custom digit HUD.
- Custom replacements for vanilla dial/next-dial actions.
- The copied 1.29 `ActionDismantlePart` implementation and `DismantleWithLock` setting.
- The old Manage Lock mode.
- The old `UnlockOnOpen` state workaround.

The removed systems are redundant or unsafe against the 1.30 two-sided lock and construction architecture.

## Legacy Zen lock migration

`Zen_CombinationLock4` through `Zen_CombinationLock9` remain as hidden persistence aliases so existing objects do not become missing classes. They all migrate to native four-dial behaviour and cannot be newly spawned through normal loot/config scope.

For an old 5–9-digit lock, the migrated PIN is the final four digits of its saved PIN. For example, `123456` becomes `3456`.

## Config migration

The active settings are:

```json
{
    "ServerConfig": {
        "PrintLogs": true,
        "DigitMultiplier": 1
    },
    "ClientSyncConfig": {
        "AllowAdminOpen": true,
        "InteractAnywhere": true,
        "InstantOpen": false,
        "DialTime": 0.5,
        "OwnerCanRemoveLockWithoutCode": false,
        "RaidConfig": {
            "Enabled": true,
            "LockConfig": []
        }
    }
}
```

Old `DisplayDigits`, `DismantleWithLock`, and `UnlockOnOpen` keys are no longer used.

## Recommended test pass

1. Attach both vanilla three- and four-dial locks.
2. Enter a correct PIN from each side and confirm the closed door opens without detaching the lock.
3. Close the door and confirm the remembered Open action works while aiming away from the lock selection.
4. Re-enter the PIN while the door is open and confirm the lock detaches.
5. Restart the server and confirm owner/guest access persists.
6. Raid both lock types directly and through the gate/door with `Zen_BoltCutter`.
7. Repeat against any enabled BBP, Building Fortifications, and RaG Cabin versions.
8. Set a digital-lock PIN, close/re-lock its gate or door, and confirm the owner can use **Open** without the keypad.
9. Have a second player enter the correct digital PIN once, re-lock the door, and confirm their remembered access survives a restart.
10. Reset or detach the digital lock and confirm its old remembered players lose access.

The supplied archive contains the rebuilt `config.cpp` and script tree. Keep the existing `data`, sound, model, and stringtable assets used by the original full PBO source.
