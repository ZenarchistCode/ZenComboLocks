class Zen_ActionManageCombinationLockOnFence : ActionInteractBase
{
	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}

	// Returns action text (this only works because this is a local action - GetGame().GetPlayer() is client-only!
	override string GetText()
	{
		PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
		if (pb)
		{
			// Get combo lock
			ConstructionActionData construction_action_data = pb.GetConstructionActionData();
			CombinationLock combination_lock = construction_action_data.GetCombinationLock();

			// If combo lock exists, handle management
			if (combination_lock)
			{
				// Set action text accordingly
				if (combination_lock.IsManagingLockClient())
				{
					m_Text = "#STR_ZenStopManageLock";
				}
				else
				{
					m_Text = "#STR_ZenStartManageLock";
				}
			}
		}

		return m_Text;
	}

	// Action is to be performed instantly with no delay
	override bool IsInstant()
	{
		return true;
	}

	// This action doesn't need to be synchronized to the server
	override bool IsLocal()
	{
		return true;
	}

	// Check client-side conditions
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		// If interact anywhere is disabled, do not show manage lock action on fence
		if (!GetZenComboLocksConfig().ClientSyncConfig || !GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
			return false;

		// If there is no target object or we're an admin, stop here
		if (!target.GetObject() || (player.IsAdminZCBL() && !GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen))
			return false;

		// BaseBuildingPlus is difficult to get my lock management to work with, so I just don't bother for now
		if (target.GetObject().IsKindOf("BBP_WALL_BASE"))
			return false;

		// Check if player is looking at fence
		CombinationLock lock = ZenComboLocksHelper.GetCombinationLock(target.GetObject());

		// If we haven't found a combo lock, stop here
		if (!lock || !lock.IsLocked())
			return false;

		// If client has not received lock perms, don't display action
		if (!lock.HasReceivedClientsidePerms())
			return false;

		// If we've found a combination lock, check that we have permission to use "manage" lock action
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);
		return !ZenComboLocksHelper.IsOpen(target.GetObject()) && lock.IsPermittedToOpen(player) && !lock.IsTakeable();
	}

	// Do action (client-side only)
	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);

		// Get combo lock
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock combination_lock = construction_action_data.GetCombinationLock();

		// If combo lock exists, handle management
		if (combination_lock)
		{
			// Set managing lock to opposite of whatever it currently is
			combination_lock.SetManagingLockClient(!combination_lock.IsManagingLockClient());
		}
	}
}