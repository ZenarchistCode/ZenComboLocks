class Zen_ActionOpenComboLockFence : ActionContinuousBase
{
	void Zen_ActionOpenComboLockFence()
	{
		m_CallbackClass = Zen_ActionOpenComboLockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#open";
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
		// If there is no object, stop here
		if (!target.GetObject() || !GetZenComboLocksConfig().ClientSyncConfig)
			return false;

		// If interact anywhere is enabled, stop here as that action is used instead
		if (GetZenComboLocksConfig().ClientSyncConfig.InstantOpen || !GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
			return false;

		// Check if player is looking at a fence containing a combo lock
		CombinationLock lock = CombinationLock.Cast(ZenComboLocksHelper.GetCombinationLock(target.GetObject()));

		// If we haven't found a gate with a combo lock or gate is open, stop here.
		if (!lock || ZenComboLocksHelper.IsOpen(lock.GetHierarchyParent()))
			return false;

		// If client has not received lock perms, don't display action yet
		if (!lock.HasReceivedClientsidePerms())
			return false;

		// Set construction action data for the player (referenced in other actions related to combo lock)
		ConstructionActionData construction_action_data = player.GetConstructionActionData();
		construction_action_data.SetCombinationLock(lock);

		// Client-side check: If player is an admin, stop here (as Instant action is used instead)
		if (player.IsAdminZCBL())
			return false;

		// Check action condition client (player has client permission & is not managing the lock)
		return lock.IsPermittedToOpen(player) && !lock.IsManagingLockClient() && lock.IsLocked();
	}

	// Called on server when each dial action finishes (0.5 secs apart)
	override void OnFinishProgressServer(ActionData action_data)
	{
		// If instant open is allowed, this action is redundant
		if (GetZenComboLocksConfig().ClientSyncConfig.InstantOpen)
			return;

		// Check that player identity exists
		if (!action_data.m_Player || !action_data.m_Player.GetIdentity())
			return;

		// Get combi lock from action data
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		CombinationLock lock = construction_action_data.GetCombinationLock();

		// If lock doesn't exist, stop here
		if (!lock)
			return;

		// Check permission
		if (lock.IsLockedOnGate() && lock.IsPermittedToOpen(action_data.m_Player))
		{
			lock.IncreaseSimulatedDialChanges();

			if (lock.GetSimulatedDialChangeCount() >= (lock.GetLockDigits() * GetZenComboLocksConfig().ServerConfig.DigitMultiplier))
			{
				// Open fence
				ZenComboLocksHelper.Open(lock.GetHierarchyParent());

				// Unlock lock if enabled in config
				if (GetZenComboLocksConfig().ServerConfig.UnlockOnOpen)
					lock.UnlockServerZen(action_data.m_Player, lock.GetHierarchyParent());

				ZenComboLocksLogger.Log("Player " + action_data.m_Player.GetIdentity().GetPlainId() + " opened lock @ " + action_data.m_Player.GetPosition());
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
}