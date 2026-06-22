#ifdef BuildingFortifications
modded class ActionCloseBuildingCore
{
	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);

		#ifdef ZenModPack
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
		#endif

		if (!action_data || !action_data.m_Target || !action_data.m_Target.GetObject())
		{
			return;
		}

		CombinationLock combinationLock = ZenComboLocksHelper.GetCombinationLock(action_data.m_Target.GetObject());
		if (combinationLock)
		{
			combinationLock.SetManagingLockClient(false);
		}
	}
}
#endif