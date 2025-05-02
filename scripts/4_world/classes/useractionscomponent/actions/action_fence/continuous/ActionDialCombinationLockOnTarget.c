modded class ActionDialCombinationLockOnTarget
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
			return super.ActionCondition(player, target, item);
		#endif

		return false;
	}

	// CLIENT-SIDE: Display code digit if enabled in config
	override void Do(ActionData action_data, int state)
	{
		super.Do(action_data, state);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
			return;
		#endif

		// If digit display is disabled or we're calling Do() on server, stop here
		if (GetGame().IsDedicatedServer() || GetZenComboLocksConfig().ClientSyncConfig.DisplayDigits == 0)
			return;

		// Get combo lock
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock lock = construction_action_data.GetCombinationLock();

		// If lock exists, display current digit
		if (lock)
			lock.DisplayDigitText();
	}
}