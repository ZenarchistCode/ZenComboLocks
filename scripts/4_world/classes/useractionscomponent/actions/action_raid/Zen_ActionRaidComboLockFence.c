class Zen_ActionRaidComboLockFenceCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		if (m_ActionData && m_ActionData.m_Target && m_ActionData.m_Target.GetObject())
		{
			// Get combo lock from fence
			CombinationLock lock = ZenComboLocksHelper.GetCombinationLock(m_ActionData.m_Target.GetObject());

			// If lock exists, check config
			if (lock)
			{
				ZenLockConfig config = GetZenComboLocksConfig().GetRaidConfigForType(lock.GetType());

				if (config)
				{
					m_ActionData.m_ActionComponent = new CAContinuousTime(config.TimeRequired);
				}
			}
		}
	}
};

class Zen_ActionRaidComboLockFence : ActionContinuousBase
{
	// Declare sound update variables for boltcutter seffect
	private int m_LastSoundUpdate = 0;
	private int m_NextSoundUpdate = 0;

	void Zen_ActionRaidComboLockFence()
	{
		m_CallbackClass = Zen_ActionRaidComboLockFenceCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#destroy";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.BASEBUILDING);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return false;
		}
		#endif

		// If there is no object, stop here
		if (!target.GetObject() || !GetZenComboLocksConfig().ClientSyncConfig)
			return false;

		// Check client config
		if (!GetZenComboLocksConfig().ClientSyncConfig.RaidConfig.Enabled || !GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
			return false;

		// Check if player is looking at fence with a lock on it
		CombinationLock lock = ZenComboLocksHelper.GetCombinationLock(target.GetObject());

		// If we haven't found a combo lock or it's not locked onto the gate, stop here.
		if (!lock || !lock.IsLockedOnGate())
			return false;

		// If there is no raid config for this item type, stop here and do not allow action.
		if (!GetZenComboLocksConfig().GetRaidConfigForType(lock.GetType()))
			return false;

		// Set construction action data for the player (referenced in other actions related to combo lock)
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);

		// Check action condition client (player has client permission & is not managing the lock)
		m_Text = "#destroy " + lock.GetDisplayName();
		return true;
	}

	// Called when each dial action finishes (0.5 secs apart)
	override void OnFinishProgressServer(ActionData action_data)
	{
		// If instant open is allowed, this action is redundant
		if (!GetZenComboLocksConfig().ClientSyncConfig.RaidConfig.Enabled || !action_data.m_MainItem || action_data.m_MainItem.IsRuined())
			return;

		if (!action_data.m_Player || !action_data.m_Player.GetIdentity() || !action_data.m_Player.GetIdentity())
			return;

		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock lock = construction_action_data.GetCombinationLock();

		if (lock)
		{
			// If raid config doesn't exist, stop
			ZenLockConfig raidConfig = GetZenComboLocksConfig().GetRaidConfigForType(lock.GetType());
			if (!raidConfig)
				return;

			if (lock.IsLockedOnGate())
			{
				// Destroy lock and drop it if possible
				lock.UnlockServer(action_data.m_Player, lock.GetHierarchyParent());

				if (lock.GetHierarchyParent())
				{
					lock.GetInventory().DropEntity(InventoryMode.SERVER, lock.GetHierarchyParent(), lock);
					lock.SetHealth(0);
					action_data.m_MainItem.SetHealth(action_data.m_MainItem.GetHealth() - raidConfig.DamageTool);
					ZenComboLocksLogger.Log("Player " + action_data.m_Player.GetIdentity().GetPlainId() + " destroyed lock with Bolt Cutters @ " + action_data.m_Player.GetPosition());
				}
				else
				{
					lock.DeleteSafe();
				}
			}
		}
	}

	// Update sound effect on bolt cutter
	override void Do(ActionData action_data, int state)
	{
		super.Do(action_data, state);

		if (GetGame().IsDedicatedServer())
		{
			if (action_data.m_MainItem)
			{
				if (g_Game.GetTime() - m_LastSoundUpdate >= m_NextSoundUpdate)
				{
					// Send clipping sound every 3-10 seconds while raiding lock
					m_LastSoundUpdate = g_Game.GetTime();
					m_NextSoundUpdate = Math.RandomIntInclusive(3000, 10000);

					Zen_BoltCutter boltcutter = Zen_BoltCutter.Cast(action_data.m_MainItem);
					if (boltcutter)
						boltcutter.SyncSound();
				}
			}
		}
	}

	// Called when action starts
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		m_LastSoundUpdate = 0;
		m_NextSoundUpdate = 0;
	}
}