class Zen_ActionRemoveComboLock : ActionContinuousBase
{
	void Zen_ActionRemoveComboLock()
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

		// If there is no object, stop here
		if (!target.GetObject() || !GetZenComboLocksConfig().ClientSyncConfig)
			return false;

		// If server has enabled instant open or player is an admin, stop here (as Instant action is used instead)
		if (GetZenComboLocksConfig().ClientSyncConfig.InstantOpen || player.IsAdminZCBL())
			return false;

		// Check if player is looking directly at combo lock
		CombinationLock lock = CombinationLock.Cast(target.GetObject());

		// If we haven't found a combo lock or it's not locked onto the gate, stop here.
		if (!lock || !lock.IsLockedOnGate())
			return false;

		// If client has not received lock perms, don't display action
		if (!lock.HasReceivedClientsidePerms() || !GetZenComboLocksConfig().ClientSyncConfig.OwnerCanRemoveLockWithoutCode)
			return false;

		// If we're not managing the lock, stop here
		if (GetGame().IsClient() && !lock.IsManagingLockClient())
			return false;

		// Set construction action data for the player (referenced in other actions related to combo lock on both client & server)
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);

		// Check action condition client (player has client permission & is not managing the lock)
		return lock.IsPermittedToOpen(player) && !ZenComboLocksHelper.IsOpen(lock.GetHierarchyParent()) && !lock.IsTakeable();
	}

	// Called when each dial action finishes (0.5 secs apart)
	override void OnFinishProgressServer(ActionData action_data)
	{
		// If instant open is allowed, this action is redundant. Stop here.
		if (GetZenComboLocksConfig().ClientSyncConfig.InstantOpen)
			return;

		// If player identity doesn't exist, something's wrong - stop.
		if (!action_data.m_Player.GetIdentity())
			return;

		// Get combo lock from action data
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock lock = construction_action_data.GetCombinationLock();

		// Check if lock exists
		if (!lock)
			return;

		// Check if lock is locked and player is permitted to open it
		if (lock.IsLockedOnGate() && lock.IsPermittedToOpen(action_data.m_Player))
		{
			lock.IncreaseSimulatedDialChanges();

			// Open fence if simulated dial count exceeds server config
			if (lock.GetSimulatedDialChangeCount() >= (lock.GetLockDigits() * GetZenComboLocksConfig().ServerConfig.DigitMultiplier))
			{
				// Open lock
				lock.UnlockServerZen(action_data.m_Player, lock.GetHierarchyParent());
				ZenComboLocksLogger.Log("Player " + action_data.m_Player.GetIdentity().GetPlainId() + " unlocked lock @ " + action_data.m_Player.GetPosition());
			}
		}
	}

	// Called when action starts (resets simulated lock dial count)
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		if (!action_data.m_Player.GetIdentity())
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
		CombinationLock combination_lock = CombinationLock.Cast(action_data.m_Target.GetObject());

		// If combo lock exists, handle management
		if (combination_lock)
		{
			// Set managing lock to false after door has been closed
			combination_lock.SetManagingLockClient(false);
		}
	}
}