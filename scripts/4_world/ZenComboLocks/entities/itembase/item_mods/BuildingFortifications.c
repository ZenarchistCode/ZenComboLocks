#ifdef BuildingFortifications
modded class BF_DoorBarricade
{
	override void CloseFence()
	{
		super.CloseFence();

		if (ZenComboLocks_IsEnabled() && g_Game && g_Game.IsServer())
		{
			ZenComboLocksHelper.RelockIfNeeded(GetCombinationLock());
		}
	}
}

modded class BF_DoubleDoorBarricade
{
	override void CloseFence()
	{
		super.CloseFence();

		if (ZenComboLocks_IsEnabled() && g_Game && g_Game.IsServer())
		{
			ZenComboLocksHelper.RelockIfNeeded(GetCombinationLock());
		}
	}
}
#endif
