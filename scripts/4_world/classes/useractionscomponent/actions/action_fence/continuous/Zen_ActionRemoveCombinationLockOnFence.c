class Zen_ActionRemoveCombinationLockOnFence : ActionContinuousBase
{
	void Zen_ActionRemoveCombinationLockOnFence()
	{
		m_CallbackClass = Zen_ActionOpenComboLockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#STR_ZenRemoveLock";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}

	override bool HasProneException()
	{
		return true;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	// Check both client & server-side action requirements
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return false;
		}
		#endif

		// If there is no object or we're holding something, stop here
		if (!target.GetObject() || item || !GetZenComboLocksConfig().ClientSyncConfig)
			return false;

		// If interact anywhere is enabled, stop here as that action is used instead
		if (!GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
			return false;

		// Check if player is looking at a fence containing a combo lock
		CombinationLock lock = CombinationLock.Cast(ZenComboLocksHelper.GetCombinationLock(target.GetObject()));

		// If we haven't found a gate with a combo lock or gate is open, stop here.
		if (!lock || ZenComboLocksHelper.IsOpen(lock.GetHierarchyParent()))
			return false;

		// If client has not received lock perms, don't display action yet
		if (!lock.HasReceivedClientsidePerms() || !GetZenComboLocksConfig().ClientSyncConfig.OwnerCanRemoveLockWithoutCode)
			return false;

		// If we're not managing the lock, stop here
		if (GetGame().IsClient() && !lock.IsManagingLockClient())
			return false;

		// Set construction action data for the player (referenced in other actions related to combo lock)
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);

		// Client-side check: If player is an admin, stop here (as Instant action is used instead)
		if (player.IsAdminZCBL())
			return false;

		// Check action condition client (player has client permission & is managing the lock)
		return lock.IsPermittedToOpen(player) && !lock.IsTakeable();
	}

	// Called on server when each dial action finishes (0.5 secs apart)
	override void OnFinishProgressServer(ActionData action_data)
	{
		// Check that player identity exists
		if (!action_data.m_Player || !action_data.m_Player.GetIdentity() || !action_data.m_Player.GetIdentity())
			return;

		// Get combi lock from action data
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock lock = construction_action_data.GetCombinationLock();

		// If lock doesn't exist, stop here
		if (!lock)
			return;

		// Check permission
		if (lock.IsLockedOnGate() && lock.IsLocked() && lock.IsPermittedToOpen(action_data.m_Player))
		{
			lock.IncreaseSimulatedDialChanges();

			if (lock.GetSimulatedDialChangeCount() >= (lock.GetLockDigits() * GetZenComboLocksConfig().ServerConfig.DigitMultiplier))
			{
				// Open lock
				EntityAI target_entity = EntityAI.Cast(action_data.m_Target.GetObject());
				lock.UnlockServerZen(action_data.m_Player, target_entity);
				ZenComboLocksLogger.Log("Player " + action_data.m_Player.GetIdentity().GetPlainId() + " unlocked lock @ " + action_data.m_Player.GetPosition());
			}
		}
	}

	// Called when action starts (resets simulated lock dial count)
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		if (!action_data.m_Player || !action_data.m_Player.GetIdentity())
			return;

		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock lock = construction_action_data.GetCombinationLock();

		// Reset simulated dial changes
		if (lock)
			lock.ResetSimulatedDialChanges();
	}

	// Do action (client-side only)
	override void OnEndClient(ActionData action_data)
	{
		super.OnEndClient(action_data);

		// Get combo lock
		CombinationLock combination_lock = ZenComboLocksHelper.GetCombinationLock(action_data.m_Target.GetObject());

		// If combo lock exists, handle management
		if (combination_lock)
		{
			// Set managing lock to false after door has been closed
			combination_lock.SetManagingLockClient(false);
		}
	}
}