#ifdef BuildingFort_Mod_1A
modded class BF_DoorBarricade
{
	// Return whether or not this fence can be opened
	override bool CanOpenFence()
	{
		if (super.CanOpenFence())
			return true;

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return true;
		}
		#endif

		if (HasHinges() && !IsOpened() && IsLocked())
		{
			if (GetGame().IsClient())
			{
				return GetCombinationLock() && GetCombinationLock().IsPermittedToOpen();
			}
		}

		return false;
	}

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
		// Lock combination lock if it is not locked onto the door (ie. door owner/guest has unlocked lock, but not taken it off)
		if (GetCombinationLock() && !GetCombinationLock().IsLocked())
		{
			GetCombinationLock().LockServer(this, true);
		}
		#endif
	}

	// Set actions
	override void SetActions()
	{
		super.SetActions();

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