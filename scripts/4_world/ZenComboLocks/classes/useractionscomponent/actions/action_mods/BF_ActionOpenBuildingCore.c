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

		Object targetObject = target.GetObject();
		if (!targetObject)
		{
			return false;
		}

		BuildingFortficationsCore fence = BuildingFortficationsCore.Cast(targetObject);
		if (!fence)
		{
			return super.ActionCondition(player, target, item);
		}

		CombinationLock combinationLock = ZenComboLocksHelper.GetCombinationLock(targetObject);

		// Only suppress BF's native open action when a Zen/vanilla combo lock is actually locked onto the BF door.
		// In that case, the Zen combo-lock open action should handle permission checks/opening.
		if (combinationLock && combinationLock.IsLockedOnGate())
		{
			return false;
		}

		// No locked combo lock = let BuildingFortifications handle normal unlocked door opening.
		return super.ActionCondition(player, target, item);
	}
}
#endif