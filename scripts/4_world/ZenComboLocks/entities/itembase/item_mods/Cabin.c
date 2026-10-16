#ifdef Cabin_Mod_RaGed
modded class Prefab_Cabin
{
	override void CloseCabinFence()
	{
		super.CloseCabinFence();

		if (ZenComboLocks_IsEnabled() && g_Game && g_Game.IsServer())
		{
			ZenComboLocksHelper.RelockIfNeeded(GetCombinationLock());
		}
	}
}
#endif
