class Zen_ActionAdminCombinationLockOnTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(3);
	}
}

class Zen_ActionAdminCombinationLockOnTarget : ActionContinuousBase
{
	void Zen_ActionAdminCombinationLockOnTarget()
	{
		m_CallbackClass = Zen_ActionAdminCombinationLockOnTargetCB;
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
		if (!target.GetObject())
			return false;

		// If we're client-side and player does not have admin authority, stop here
		if (GetGame().IsClient() && !player.IsAdminZCBL())
			return false;

		// Check if player is looking directly at combo lock
		CombinationLock lock = CombinationLock.Cast(target.GetObject());

		// If we haven't found a combo lock, stop here
		if (!lock || !lock.IsLockedOnGate())
			return false;

		// If we're server-side and player does not have admin authority, stop here
		if (GetGame().IsDedicatedServer())
		{
			if (!GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen)
				return false;

			if (player && !player.IsAdminZCBL())
				return false;
		}

		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);
		return true;
	}

	// Do action
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		if (!GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen)
			return;

		if (!action_data.m_Player || !action_data.m_Player.GetIdentity() || !action_data.m_Player.IsAdminZCBL())
			return;

		// Get player action construction data & combo lock object
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock combination_lock = construction_action_data.GetCombinationLock();

		// If combo lock exists...
		if (combination_lock)
		{
			// Unlock and drop it
			combination_lock.UnlockServer(action_data.m_Player, combination_lock.GetHierarchyParent());
			combination_lock.GetInventory().DropEntity(InventoryMode.SERVER, action_data.m_Player, combination_lock);
			ZenComboLocksLogger.Log("Admin " + action_data.m_Player.GetIdentity().GetPlainId() + " unlocked lock @ " + action_data.m_Player.GetPosition());
		}
	}
}