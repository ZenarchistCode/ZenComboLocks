class Zen_ActionOpenComboLockInstantFence : ActionInteractBase
{
	void Zen_ActionOpenComboLockInstantFence()
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

		// Check if player is looking at fence and interact anywhere is enabled
		if (!GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
			return false;

		// Get combo lock from fence target
		CombinationLock lock = ZenComboLocksHelper.GetCombinationLock(target.GetObject());

		// If we haven't found a combo lock, stop here.
		if (!lock)
			return false;

		// If client has not received lock perms, don't display action
		if (!lock.HasReceivedClientsidePerms())
			return false;

		// If fence doesn't exist or gate is opened, stop here.
		if (ZenComboLocksHelper.IsOpen(target.GetObject()))
			return false;

		// Set construction action data for the player (referenced in other actions related to combo lock)
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);

		// If we've made it this far, check if client has permission to open
		return lock.IsLocked() && (GetZenComboLocksConfig().ClientSyncConfig.InstantOpen && lock.IsPermittedToOpen(player) && !lock.IsManagingLockClient()) || player.IsAdminZCBL();
	}

	// Do action
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		// If player identity doesn't exist, stop here
		if (!action_data.m_Player.GetIdentity())
			return;

		// If server has enabled instant open, allow instant open if player is permitted.
		if (GetZenComboLocksConfig().ClientSyncConfig.InstantOpen || (GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen && action_data.m_Player.IsAdminZCBL()))
		{
			// If player doesn't exist, stop here to avoid crashes
			if (!action_data.m_Player || !action_data.m_Player.GetIdentity() || !action_data.m_Player.GetIdentity())
				return;

			// Get player action construction data & combo lock object
			ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
			CombinationLock lock = construction_action_data.GetCombinationLock();

			// If combo lock exists...
			if (lock)
			{
				// If combo lock is locked onto the gate, check if player has permission
				if (lock.IsLockedOnGate() && lock.IsPermittedToOpen(action_data.m_Player))
				{
					// Open fence
					ZenComboLocksHelper.Open(lock.GetHierarchyParent());

					// Unlock lock if enabled in config
					if (GetZenComboLocksConfig().ServerConfig.UnlockOnOpen)
						lock.UnlockServerZen(action_data.m_Player, lock.GetHierarchyParent());

					ZenComboLocksLogger.Log("Player " + action_data.m_Player.GetIdentity().GetPlainId() + " opened lock @ " + action_data.m_Player.GetPosition());
				}
			}
		}
	}
}