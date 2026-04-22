// Disable vanilla action, I use my custom action instead to avoid needing to override vanilla code
modded class ActionNextCombinationLockDialOnTarget
{
	override bool Can(PlayerBase player, ActionTarget target, ItemBase item, int condition_mask)
	{
		#ifdef ZenModPack
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return super.Can(player, target, item, condition_mask);
		}
		#endif

		return false;
	}	
}