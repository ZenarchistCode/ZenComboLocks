#ifdef Cabin_Mod_RaGed
modded class Prefab_Cabin
{
	override void CloseCabinFence()
	{
		super.CloseCabinFence();

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
		#endif

		#ifdef SERVER
		if (GetCombinationLock() && !GetCombinationLock().IsLocked())
		{
			GetCombinationLock().LockServer(this, true);
		}
		#endif
	}

	override void SetActions()
	{
		super.SetActions();

		// Remove combo lock actions
		RemoveAction(ActionDialCombinationLockOnCabin);
		RemoveAction(ActionNextCombinationLockDialOnCabin);

		// Add my combo lock actions
		AddAction(Zen_ActionNextCombinationLockDialOnFence);
		AddAction(Zen_ActionRemoveComboLock);
		AddAction(Zen_ActionOpenComboLockFence);
		AddAction(Zen_ActionOpenComboLockInstantFence);
		AddAction(Zen_ActionDialCombinationLockOnFence);
		AddAction(Zen_ActionManageCombinationLockOnFence);
		AddAction(Zen_ActionRemoveCombinationLockOnFence);
		AddAction(Zen_ActionAdminCombinationLockOnFence);
	}
}
#endif