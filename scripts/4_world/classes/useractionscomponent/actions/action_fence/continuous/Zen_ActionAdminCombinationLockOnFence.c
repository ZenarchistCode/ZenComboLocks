class Zen_ActionAdminCombinationLockOnFenceCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(3);
	}
}

class Zen_ActionAdminCombinationLockOnFence : ActionContinuousBase
{
	void Zen_ActionAdminCombinationLockOnFence()
	{
		m_CallbackClass = Zen_ActionAdminCombinationLockOnFenceCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool HasProgress()
	{
		return true;
	}

	override string GetText()
	{
		return "#STR_ZenAdminManageLock";
	}

	// Check client-side conditions
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return false;
		}
		#endif

		// If there is no target object, stop here
		if (!target.GetObject() || !GetZenComboLocksConfig().ClientSyncConfig)
			return false;

		// If player does not have admin authority, stop here
		if (!player.IsAdminZCBL() || !GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen)
			return false;

		// Check if player is looking at fence
		CombinationLock lock = ZenComboLocksHelper.GetCombinationLock(target.GetObject());

		// If we haven't found a combo lock, stop here
		if (!lock || !lock.IsLockedOnGate())
			return false;

		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);
		return true;
	}

	// Do action
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		// If player does not have admin authority, stop here
		if (!action_data.m_Player || !action_data.m_Player.IsAdminZCBL() || !GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen)
			return;

		// Get player action construction data & combo lock object
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock lock = construction_action_data.GetCombinationLock();

		// If combo lock exists...
		if (lock)
		{
			// Unlock and drop it
			lock.UnlockServer(action_data.m_Player, lock.GetHierarchyParent());
			lock.GetInventory().DropEntity(InventoryMode.SERVER, action_data.m_Player, lock);
			ZenComboLocksLogger.Log("Admin " + action_data.m_Player.GetIdentity().GetPlainId() + " removed lock @ " + action_data.m_Player.GetPosition());
		}
	}
}