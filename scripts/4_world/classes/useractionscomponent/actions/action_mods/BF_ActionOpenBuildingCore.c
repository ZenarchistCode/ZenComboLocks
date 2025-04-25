#ifdef BuildingFort_Mod_1A
modded class ActionOpenBuildingCore
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (super.ActionCondition(player, target, item))
		{
			BF_DoorBarricade door = BF_DoorBarricade.Cast(target.GetObject());
			if (door)
			{
				if (door.GetCombinationLock() && door.GetCombinationLock().IsLockedOnGate() && GetZenComboLocksConfig().ClientSyncConfig)
				{
					return GetZenComboLocksConfig().ClientSyncConfig.InstantOpen;
				}
			}

			return true;
		}

		return false;
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		BuildingFortficationsCore bfc = BuildingFortficationsCore.Cast(action_data.m_Target.GetObject());
		if (bfc)
		{
			CombinationLock lock = ZenComboLocksHelper.GetCombinationLock(bfc);
			if (lock && lock.IsLockedOnGate() && action_data.m_Player.GetIdentity() && lock.IsPermittedToOpen(action_data.m_Player))
			{
				bfc.OpenFence();
				ZenComboLocksLogger.Log("Player " + action_data.m_Player.GetIdentity().GetPlainId() + " opened lock @ " + action_data.m_Player.GetPosition());
			}
		}
	}
};
#endif