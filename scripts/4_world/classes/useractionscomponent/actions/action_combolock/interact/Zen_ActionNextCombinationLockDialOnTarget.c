class Zen_ActionNextCombinationLockDialOnTarget : ActionInteractBase
{
	void Zen_ActionNextCombinationLockDialOnTarget()
	{
		m_Text = "#next_combination_lock_dial";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}

	override bool IsInstant()
	{
		return true;
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

		// Check if player is looking directly at combo lock
		CombinationLock lock = CombinationLock.Cast(target.GetObject());

		// If lock doesn't exist or is unlocked, stop here
		if (!lock || !lock.IsLockedOnGate())
			return false;

		// If client has not received lock perms, don't display action
		if (!lock.HasReceivedClientsidePerms())
			return false;

		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);

		// If player owns this lock is a guest, hide the dial action unless they've selected Manage Lock
		if (lock.IsPermittedToOpen(player))
			return GetGame().IsDedicatedServer() || (lock.IsManagingLockClient() && !GetZenComboLocksConfig().ClientSyncConfig.OwnerCanRemoveLockWithoutCode);

		// Player is not an owner of this combo lock - let them dial the lock as normal
		return true;
	}

	// From vanilla action
	override void Start(ActionData action_data)
	{
		super.Start(action_data);

		//set next dial
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock combination_lock = construction_action_data.GetCombinationLock();
		if (combination_lock)
			combination_lock.SetNextDial();
	}
}