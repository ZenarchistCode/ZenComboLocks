modded class ActionCombinationLockUnlock
{
	override void OnActionInfoUpdate(PlayerBase player, ActionTarget target, ItemBase item)
	{
		super.OnActionInfoUpdate(player, target, item);

		if (!ZenComboLocks_IsEnabled() || !target)
		{
			return;
		}

		CombinationLock targetLock = CombinationLock.Cast(target.GetObject());
		if (targetLock && targetLock.IsLockAttached() && !ZenComboLocksHelper.IsOpenForLock(targetLock))
		{
			m_Text = "#open";
			return;
		}

		m_Text = "#detach_combination_lock";
	}
}
