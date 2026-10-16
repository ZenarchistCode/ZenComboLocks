modded class Fence
{
	override void CloseFence()
	{
		super.CloseFence();

		if (!ZenComboLocks_IsEnabled() || !g_Game || !g_Game.IsServer())
		{
			return;
		}

		ZenComboLocksHelper.RelockIfNeeded(GetCombinationLock());
		ZenComboLocksHelper.RelockDigitalIfNeeded(GetCodeLock());
	}
}
