#ifdef Cabin_Mod_RaGed
modded class ActionCloseCabinFence : ActionInteractBase
{
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