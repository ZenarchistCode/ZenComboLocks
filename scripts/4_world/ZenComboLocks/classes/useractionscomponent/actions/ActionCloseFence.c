modded class ActionCloseFence
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!ZenComboLocks_IsEnabled() || !GetZenComboLocksConfig().ClientSyncConfig || !GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
		{
			return super.ActionCondition(player, target, item);
		}

		if (target && target.GetObject())
		{
			Fence targetFence = Fence.Cast(target.GetObject());
			if (targetFence && targetFence.CanCloseFence())
			{
				return true;
			}
		}

		return super.ActionCondition(player, target, item);
	}

	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);

		if (ZenComboLocks_IsEnabled())
		{
			ZenComboLocksHelper.RelockActionTarget(action_data.m_Target);
			ZenComboLocksHelper.RelockDigitalActionTarget(action_data.m_Target);
		}
	}
}

modded class ActionCloseDoors
{
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);

		if (ZenComboLocks_IsEnabled())
		{
			ZenComboLocksHelper.RelockActionTarget(action_data.m_Target);
			ZenComboLocksHelper.RelockDigitalActionTarget(action_data.m_Target);
		}
	}
}
