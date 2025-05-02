modded class Fence
{
	override void SetActions()
	{
		super.SetActions();

		// Allow use of these actions on any area of fence if enabled in server config
		AddAction(Zen_ActionNextCombinationLockDialOnFence);
		AddAction(Zen_ActionRemoveComboLock);
		AddAction(Zen_ActionOpenComboLockFence);
		AddAction(Zen_ActionOpenComboLockInstantFence);
		AddAction(Zen_ActionDialCombinationLockOnFence);
		AddAction(Zen_ActionManageCombinationLockOnFence);
		AddAction(Zen_ActionRemoveCombinationLockOnFence);
		AddAction(Zen_ActionAdminCombinationLockOnFence);
	}

	// Lock the combo lock when fence is shut 
	override void CloseFence()
	{
		super.CloseFence();

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
		#endif

		#ifdef SERVER
		if (GetCombinationLock() && GetCombinationLock().IsTakeable())
		{
			GetCombinationLock().LockServer(this, true);
		}
		#endif
	}
};