// Disable vanilla action, I use my custom action instead to avoid needing to override vanilla code
modded class ActionNextCombinationLockDialOnTarget
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return super.ActionCondition(player, target, item);
		}
		#endif

		return false;
	}
}