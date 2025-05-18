// Disable vanilla action, I use my custom action instead to avoid needing to override vanilla code.
// I need to override Can() instead of ActionCondition() because some basebuilding mods do not check
// super.ActionCondition, and Can() overrides that check allowing me to do what I need to do.
modded class ActionDialCombinationLockOnTarget
{
	override bool Can(PlayerBase player, ActionTarget target, ItemBase item, int condition_mask)
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