class Zen_ActionOpenComboLockInstant : ActionInteractBase
{
	void Zen_ActionOpenComboLockInstant()
	{
		m_Text = "#open";
	}

	// Create action item & target conditions
	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}

	// Action is an instant action
	override bool IsInstant()
	{
		return true;
	}

	// Check the various conditions required to allow this action
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		// If there is no target, stop here
		if (!target.GetObject() || !GetZenComboLocksConfig().ClientSyncConfig)
			return false;

		// Check if player is looking directly at combo lock
		CombinationLock lock = CombinationLock.Cast(target.GetObject());

		// If we haven't found a combo lock, stop here.
		if (!lock || !lock.IsLockedOnGate())
			return false;

		// Ensure the door is not already open
		if (ZenComboLocksHelper.IsOpen(lock.GetHierarchyParent()))
			return false;

		// Set construction action data for the player (referenced in other actions related to combo lock)
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);

		// Check if player is admin - if so then permit instant open
		if (GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen && player.IsAdminZCBL())
			return true;

		// If we've made it this far, check if player has permission to open
		return GetZenComboLocksConfig().ClientSyncConfig.InstantOpen && lock.IsPermittedToOpen(player) && !lock.IsManagingLockClient();
	}

	// Do action
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		// If player identity doesn't exist, stop here
		if (!action_data.m_Player.GetIdentity())
			return;

		// If this is server has enabled instant open, allow instant open if player is permitted.
		if (GetZenComboLocksConfig().ClientSyncConfig.InstantOpen || (GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen && action_data.m_Player.IsAdminZCBL()))
		{
			// Get player action construction data & combo lock object
			ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
			CombinationLock lock = construction_action_data.GetCombinationLock();

			// If combo lock exists...
			if (lock)
			{
				// If combo lock is locked onto the gate, check if player has permission
				if (lock.IsLockedOnGate() && lock.IsPermittedToOpen(action_data.m_Player))
				{
					// Open door
					EntityAI lockParent = lock.GetHierarchyParent();
					ZenComboLocksHelper.Open(lockParent);

					// Unlock lock if enabled in config
					if (GetZenComboLocksConfig().ServerConfig.UnlockOnOpen)
						lock.UnlockServerZen(action_data.m_Player, lockParent);

					ZenComboLocksLogger.Log("Player " + action_data.m_Player.GetIdentity().GetPlainId() + " opened lock @ " + action_data.m_Player.GetPosition());
				}
			}
		}
	}
}