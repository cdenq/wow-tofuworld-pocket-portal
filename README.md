# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore
## Pocket Portal
- Latest build status with azerothcore: [![Build Status](https://github.com/azerothcore/mod-pocket-portal/workflows/core-build/badge.svg?branch=master&event=push)](https://github.com/azerothcore/mod-pocket-portal)

## Description

Stygiancore Pocket Portal with anti-exploit script to stop it being used in BG, Arena, Combat, and other exploitable scenarios.

Updated to create a dialog system to teleport the player to the desired location.
Upstream sells the item at the vendor Endora Moorehead in the magical goods shop in Dalaran; the Tofu fork does not.
Now with more apples.
## Tofu Worldview fork (branch `tofu`)

Changes from upstream:
- Per-player cooldown (`PocketPortal.Cooldown`, default 3 seconds), stored on each player, instead of one cooldown shared by the whole server.
- Despawn time (`Portal.NpcDuration`) is now separate from the cooldown; 30 seconds in the shipped conf (the code falls back to 15 if the key is missing). A new use despawns the player's previous teleporter.
- The script handles every use (blocked, on cooldown, disabled, or success), so the item's own on-use spell never casts.
- IDs moved into the Tofu custom range: item 900000, creature 900000, gossip menus 900000 to 900002 and 900010 to 900026.
- The item is Bind-on-Account, cannot be sold (SellPrice 0), and is not sold by any vendor; players get it from the starting kit.
- Removed the SmartAI "despawn closest creature" row (it could despawn another player's teleporter) and the dance emote that never worked; fixed the "Wilderness 3" menu label.

## Installation

```
1. Simply place the module under the `modules` directory of your AzerothCore source.
2. Re-run cmake and launch a clean build of AzerothCore.
3. Copy pocketportal.conf.dist to pocketportal.conf
4. Launch worldserver
```


## Credits

*  [Micrah/Milestorme](https://github.com/milestorme) Script/Module Creator.
*  [stygianisthebest](http://stygianthebest.github.io) sql pocket portal.
*  [rochet2](https://github.com/Rochet2) Original code.
