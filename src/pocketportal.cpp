/* Credits: Micrah/Milestorme Module creator */
/* Tofu Worldview changes: per-player cooldown, separate cooldown and despawn settings, a new use replaces the player's previous teleporter, and every use is handled by the script so the item's own spell never casts. */

#include "Config.h"
#include "Creature.h"
#include "DataMap.h"
#include "GameTime.h"
#include "Item.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "StringFormat.h"
#include "TemporarySummon.h"
#include "Unit.h"

#include <algorithm>

namespace
{
    constexpr uint32 NPC_POCKET_PORTAL = 900000;
    constexpr char const* POCKET_PORTAL_DATA_KEY = "TofuPocketPortal";

    // Per-player state, stored on the Player object itself, so each player has their own cooldown and it is only touched from that player's map thread.
    struct PocketPortalData : public DataMap::Base
    {
        bool hasUsed = false;
        Milliseconds lastUse = 0ms;
        ObjectGuid npcGuid;
    };
}

class pocket_portal : public ItemScript
{
public:
    pocket_portal() : ItemScript("pocket_portal") { }

    // Every path returns true: true tells the core the use was handled here, so it does not cast the item's own on-use spell.
    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/) override
    {
        if (!sConfigMgr->GetOption<bool>("PocketPortal.Enable", true))
        {
            player->SendSystemMessage("The Pocket Portal is disabled on this server.");
            return true;
        }

        if (player->duel || player->GetMap()->IsBattleArena() || player->InBattleground() || player->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH) || player->isDead() || player->IsInCombat() || player->IsInFlight() || player->HasStealthAura() || player->HasInvisibilityAura())
        {
            player->SendSystemMessage("You can't use the Pocket Portal right now.");
            return true;
        }

        PocketPortalData* data = player->CustomData.GetDefault<PocketPortalData>(POCKET_PORTAL_DATA_KEY);

        Milliseconds const now = GameTime::GetGameTimeMS();
        Milliseconds const cooldown = Seconds(std::max<int32>(0, sConfigMgr->GetOption<int32>("PocketPortal.Cooldown", 3)));

        if (data->hasUsed && now < data->lastUse + cooldown)
        {
            Milliseconds const remaining = data->lastUse + cooldown - now;
            uint32 const secondsLeft = uint32((remaining.count() + 999) / 1000);
            player->SendSystemMessage(Acore::StringFormat("Pocket Portal is on cooldown. Try again in {} seconds.", secondsLeft));
            return true;
        }

        // A new use replaces this player's previous teleporter, if it is still on the same map.
        if (!data->npcGuid.IsEmpty())
        {
            if (Creature* previous = ObjectAccessor::GetCreature(*player, data->npcGuid))
                previous->DespawnOrUnsummon();

            data->npcGuid.Clear();
        }

        Milliseconds const duration = Seconds(std::max<int32>(1, sConfigMgr->GetOption<int32>("Portal.NpcDuration", 15)));

        TempSummon* npc = player->SummonCreature(NPC_POCKET_PORTAL, player->GetPositionX() + 1.5f, player->GetPositionY() + 1.5f, player->GetPositionZ() + 0.15f, player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, uint32(duration.count()));

        if (!npc)
        {
            player->SendSystemMessage("The Pocket Portal fizzles. Try again somewhere else.");
            return true;
        }

        npc->SetFacingToObject(player);

        data->npcGuid = npc->GetGUID();
        data->lastUse = now;
        data->hasUsed = true;
        return true;
    }
};

void Addmod_pocket_portalScripts()
{
    new pocket_portal();
}
