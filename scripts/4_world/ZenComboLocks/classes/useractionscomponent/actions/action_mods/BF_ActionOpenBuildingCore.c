#ifdef BuildingFortifications
modded class ActionOpenBuildingCore
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		#ifdef ZenModPack
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return super.ActionCondition(player, target, item);
		}
		#endif

		// Let my custom Zen_ActionOpenComboLockInstantFence takeover.
		return false;
	}
}
#endif