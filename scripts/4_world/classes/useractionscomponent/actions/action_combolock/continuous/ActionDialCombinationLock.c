// Simple vanilla override - no need for custom action
modded class ActionDialCombinationLockCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(GetZenComboLocksConfig().ClientSyncConfig.DialTime);
	}
}

modded class ActionDialCombinationLock : ActionContinuousBase
{
	// CLIENT-SIDE: Display code digit if enabled in config
	override void Do(ActionData action_data, int state)
	{
		super.Do(action_data, state);

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

// For BBP compatibility:
modded class ActionDialCombinationLockOnTarget
{
	// CLIENT-SIDE: Display code digit if enabled in config
	override void Do(ActionData action_data, int state)
	{
		super.Do(action_data, state);

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

// For BBP compatibility:
modded class ActionCloseFence
{
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);

		if (action_data.m_Target.GetObject() && action_data.m_Target.GetObject().IsKindOf("BBP_WALL_BASE"))
		{
			CombinationLock lock = ZenComboLocksHelper.GetCombinationLock(action_data.m_Target.GetObject());
			if (lock)
			{
				if (!lock.IsLocked())
				{
					lock.LockServer(EntityAI.Cast(action_data.m_Target.GetObject()), true);
				}
			}
		}
	};
};