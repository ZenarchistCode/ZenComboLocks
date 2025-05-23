// Simple vanilla override - no need for custom action
modded class ActionDialCombinationLockCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			super.CreateActionComponent();
			return;
		}
		#endif

		m_ActionData.m_ActionComponent = new CAContinuousRepeat(GetZenComboLocksConfig().ClientSyncConfig.DialTime);
	}
}

modded class ActionDialCombinationLock : ActionContinuousBase
{
	// CLIENT-SIDE: Display code digit if enabled in config
	override void Do(ActionData action_data, int state)
	{
		super.Do(action_data, state);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
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