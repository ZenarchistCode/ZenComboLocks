#ifdef BuildingFort_Mod_1A
modded class ActionCloseBuildingCore : ActionInteractBase
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return super.ActionCondition(player, target, item);
		}
		#endif

		Object targetObject = target.GetObject();
		if (targetObject && targetObject.CanUseConstruction())
		{
			BuildingFortficationsCore fence = BuildingFortficationsCore.Cast(targetObject);

			if (fence && fence.CanCloseFence())
			{
				// Check if interact anywhere is enabled
				if (GetZenComboLocksConfig().ClientSyncConfig && GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
					return true;
			}
		}

		return super.ActionCondition(player, target, item);
	}

	// Do action (client-side only)
	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
		#endif

		// Get combo lock
		CombinationLock combination_lock = ZenComboLocksHelper.GetCombinationLock(action_data.m_Target.GetObject());

		// If combo lock exists, handle management
		if (combination_lock)
		{
			// Set managing lock to false after door has been closed
			combination_lock.SetManagingLockClient(false);
		}
	}
}
#endif