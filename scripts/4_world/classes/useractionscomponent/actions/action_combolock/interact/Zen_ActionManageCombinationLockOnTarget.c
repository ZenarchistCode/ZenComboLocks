class Zen_ActionManageCombinationLockOnTarget : ActionInteractBase
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
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return false;
		}
		#endif

		// If there is no target object or we're an admin, stop here
		if (!target.GetObject() || !GetZenComboLocksConfig().ClientSyncConfig || (player.IsAdminZCBL() && !GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen))
			return false;

		// Check if player is looking directly at combo lock
		CombinationLock lock = CombinationLock.Cast(target.GetObject());

		// If we haven't found a combo lock, stop here
		if (!lock || !lock.IsLockedOnGate())
			return false;

		// If client has not received lock perms, don't display action
		if (!lock.HasReceivedClientsidePerms())
			return false;

		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);
		return !ZenComboLocksHelper.IsOpen(lock.GetHierarchyParent()) && lock.IsPermittedToOpen(player) && !lock.IsTakeable();
	}

	// Do action
	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);

		// Get combo lock
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock lock = construction_action_data.GetCombinationLock();

		// If combo lock exists, handle management
		if (lock)
		{
			// Set managing lock to opposite of whatever it currently is
			lock.SetManagingLockClient(!lock.IsManagingLockClient());
		}
	}
}