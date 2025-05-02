// Modified vanilla combination dial action to work with fence
class Zen_ActionDialCombinationLockOnFence : Zen_ActionDialCombinationLockOnTarget
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return false;
		}
		#endif

		// If there is no object, stop here
		if (!target.GetObject() || !GetZenComboLocksConfig().ClientSyncConfig)
			return false;

		// Check if interact anywhere is enabled or the owner can use the RemoveLockWithoutCode action
		if (!GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
			return false;

		// Check if lock exists and is not already unlocked
		CombinationLock lock = ZenComboLocksHelper.GetCombinationLock(target.GetObject());
		if (!lock || (lock && !lock.IsLocked()))
			return false;

		// If client has not received lock perms, don't display action
		if (!lock.HasReceivedClientsidePerms())
			return false;

		// Set player construction data for other functions & actions to reference
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);

		// Don't display combo dial if client has open perms and is not managing the lock
		return !lock.IsPermittedToOpen(player) || GetGame().IsDedicatedServer() || (lock.IsManagingLockClient() && !GetZenComboLocksConfig().ClientSyncConfig.OwnerCanRemoveLockWithoutCode);
	}

	// CLIENT-SIDE: Display code digit if enabled in config
	override void Do(ActionData action_data, int state)
	{
		super.Do(action_data, state);

		// If digit display is disabled or we're calling Do() on server, stop here
		if (!GetGame().IsClient() || GetZenComboLocksConfig().ClientSyncConfig.DisplayDigits == 0)
			return;

		// Get combo lock
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock lock = construction_action_data.GetCombinationLock();

		// If lock exists, display current digit
		if (lock)
			lock.DisplayDigitText();
	}

	// It's necessary to override this for Fence action as vanilla action is designed to use combo lock as UnlockServer parent and won't work with fence
	override void OnFinishProgressServer(ActionData action_data)
	{
		//set dialed number
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock lock = construction_action_data.GetCombinationLock();

		// If lock exists, dial next number
		if (lock)
		{
			lock.DialNextNumber();

			// If lock dial matches combination lock, unlock it
			if (!lock.IsLockedOnGate())
			{
				EntityAI target_entity = EntityAI.Cast(action_data.m_Target.GetObject());
				lock.UnlockServerZen(action_data.m_Player, target_entity);
				ZenComboLocksLogger.Log("Player " + action_data.m_Player.GetIdentity().GetPlainId() + " entered pin @ " + action_data.m_Player.GetPosition());
			}
		}
	}
}