modded class CombinationLock
{
	protected ref ComboLockData m_ZenComboLockData;
	protected bool m_ZenRequestedPermissions;
	protected bool m_ZenReceivedPermissions;
	protected bool m_ZenHasClientPermission;
	protected bool m_ZenIsClientOwner;
	protected bool m_ZenForceVanillaUnlock;
	protected int m_ZenClientSyncID;
	protected int m_ZenServerSyncID;
	protected int m_ZenOpenProgress;

	void CombinationLock()
	{
		m_ZenRequestedPermissions = false;
		m_ZenReceivedPermissions = false;
		m_ZenHasClientPermission = false;
		m_ZenIsClientOwner = false;
		m_ZenForceVanillaUnlock = false;
		m_ZenClientSyncID = -1;
		m_ZenServerSyncID = 0;
		m_ZenOpenProgress = 0;
	}

	void ~CombinationLock()
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ZenRequestPermissions);
		}
	}

	override void InitItemVariables()
	{
		super.InitItemVariables();
		RegisterNetSyncVariableInt("m_ZenServerSyncID");
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (!ZenComboLocks_IsEnabled())
		{
			return;
		}

		if (m_ZenClientSyncID != m_ZenServerSyncID)
		{
			m_ZenClientSyncID = m_ZenServerSyncID;
			m_ZenRequestedPermissions = false;
			m_ZenReceivedPermissions = false;
			ZenRequestPermissions();
		}
	}

	void ZenRequestPermissions()
	{
		if (!ZenComboLocks_IsEnabled() || !g_Game || !g_Game.IsClient())
		{
			return;
		}

		PlayerBase localPlayer = PlayerBase.Cast(g_Game.GetPlayer());
		if (!localPlayer || localPlayer.GetSimulationTimeStamp() < 300)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ZenRequestPermissions);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ZenRequestPermissions, 300, false);
			return;
		}

		if (m_ZenReceivedPermissions || m_ZenRequestedPermissions)
		{
			return;
		}

		m_ZenRequestedPermissions = true;
		Param1<bool> requestPayload = new Param1<bool>(true);
		RPCSingleParam(ZENCOMBOLOCKRPCs.REQUEST_COMBO_DATA, requestPayload, true, NULL);
	}

	bool ZenHasReceivedClientPermissions()
	{
		if (!g_Game || !g_Game.IsClient())
		{
			return true;
		}

		if (m_ZenReceivedPermissions)
		{
			return true;
		}

		ZenRequestPermissions();
		return false;
	}

	bool ZenCanOpen(PlayerBase player)
	{
		if (g_Game && g_Game.IsServer())
		{
			return ZenHasServerPermission(player);
		}

		return m_ZenHasClientPermission;
	}

	// Public 1.29 permission API retained for dependent mods.
	ComboLockData GetComboLockData()
	{
		ZenEnsurePermissionData();
		return m_ZenComboLockData;
	}

	bool IsPermittedToOpen(PlayerBase player = NULL)
	{
		if (!player || (g_Game && g_Game.IsClient() && !g_Game.IsServer()))
		{
			return m_ZenHasClientPermission;
		}

		return ZenHasServerPermission(player);
	}

	bool SetOwnerPermission(string playerID)
	{
		ZenSetOwnerPermission(playerID);
		return true;
	}

	bool AddGuestPermission(string playerID)
	{
		return ZenAddGuestPermission(playerID);
	}

	void ResetPlayerPermissions()
	{
		ZenEnsurePermissionData();
		m_ZenComboLockData.m_PlayerOwner = "";
		m_ZenComboLockData.m_PermittedPlayers.Clear();
		ZenTriggerPermissionSync();
	}

	void TriggerZenSync()
	{
		ZenTriggerPermissionSync();
	}

	bool ZenCanRemoveWithoutCode(PlayerBase player)
	{
		if (!player || !GetZenComboLocksConfig().ClientSyncConfig)
		{
			return false;
		}

		if (GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen && player.IsZenAdmin())
		{
			return true;
		}

		if (!GetZenComboLocksConfig().ClientSyncConfig.OwnerCanRemoveLockWithoutCode)
		{
			return false;
		}

		if (g_Game && g_Game.IsServer())
		{
			return ZenIsServerOwner(player);
		}

		return m_ZenIsClientOwner;
	}

	protected bool ZenHasServerPermission(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
		{
			return false;
		}

		PlayerIdentity permissionIdentity = player.GetIdentity();
		if (GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen && GetZenCoreConfig().IsAdmin(permissionIdentity))
		{
			return true;
		}

		ZenEnsurePermissionData();
		string permissionID = permissionIdentity.GetPlainId();
		if (m_ZenComboLockData.m_PlayerOwner == permissionID)
		{
			return true;
		}

		foreach (string permittedPlayerID : m_ZenComboLockData.m_PermittedPlayers)
		{
			if (permittedPlayerID == permissionID)
			{
				return true;
			}
		}

		return false;
	}

	protected bool ZenIsServerOwner(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
		{
			return false;
		}

		ZenEnsurePermissionData();
		return m_ZenComboLockData.m_PlayerOwner == player.GetIdentity().GetPlainId();
	}

	protected void ZenEnsurePermissionData()
	{
		if (!m_ZenComboLockData)
		{
			m_ZenComboLockData = new ComboLockData();
		}
	}

	protected void ZenSetOwnerPermission(string playerID)
	{
		ZenEnsurePermissionData();
		m_ZenComboLockData.m_PlayerOwner = playerID;
		ZenComboLocksLogger.Log("Assigned lock owner " + playerID + " to " + GetType() + " at " + GetPosition());
		ZenTriggerPermissionSync();
	}

	protected bool ZenAddGuestPermission(string playerID)
	{
		ZenEnsurePermissionData();
		if (m_ZenComboLockData.m_PlayerOwner == playerID)
		{
			return false;
		}

		foreach (string existingGuestID : m_ZenComboLockData.m_PermittedPlayers)
		{
			if (existingGuestID == playerID)
			{
				return false;
			}
		}

		m_ZenComboLockData.m_PermittedPlayers.Insert(playerID);
		ZenComboLocksLogger.Log("Remembered lock guest " + playerID + " for " + GetType() + " at " + GetPosition());
		ZenTriggerPermissionSync();
		return true;
	}

	protected void ZenGrantPermissionFromCorrectCode(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
		{
			return;
		}

		ZenEnsurePermissionData();
		string successfulPlayerID = player.GetIdentity().GetPlainId();
		if (m_ZenComboLockData.m_PlayerOwner == "")
		{
			ZenSetOwnerPermission(successfulPlayerID);
			return;
		}

		ZenAddGuestPermission(successfulPlayerID);
	}

	protected void ZenTriggerPermissionSync()
	{
		if (g_Game && g_Game.IsServer())
		{
			m_ZenServerSyncID++;
			SetSynchDirty();
		}
	}

	void ZenResetOpenProgress()
	{
		m_ZenOpenProgress = 0;
	}

	bool ZenAdvanceOpenProgress()
	{
		m_ZenOpenProgress++;

		int requiredProgress = GetLockDigits() * GetZenComboLocksConfig().ServerConfig.DigitMultiplier;
		if (requiredProgress < 1)
		{
			requiredProgress = 1;
		}

		return m_ZenOpenProgress >= requiredProgress;
	}

	bool ZenOpenForRememberedPlayer(ActionTarget actionTarget)
	{
		bool previousLockedState = m_IsLocked;
		m_IsLocked = false;

		bool openedParent = ZenComboLocksHelper.OpenForActionTarget(this, actionTarget);
		m_IsLocked = previousLockedState;

		return openedParent;
	}

	void ZenForceUnlockAndDrop(EntityAI player, EntityAI parent, bool isInsideLock = false)
	{
		m_ZenForceVanillaUnlock = true;
		UnlockOnServer(player, parent, isInsideLock);
		m_ZenForceVanillaUnlock = false;
	}

	override void UnlockOnServer(EntityAI player, EntityAI parent, bool isInsideLock = false)
	{
		if (!ZenComboLocks_IsEnabled() || m_ZenForceVanillaUnlock)
		{
			super.UnlockOnServer(player, parent, isInsideLock);
			return;
		}

		PlayerBase successfulPlayer = PlayerBase.Cast(player);
		if (IsLocked() || !successfulPlayer || !successfulPlayer.GetIdentity() || !parent)
		{
			super.UnlockOnServer(player, parent, isInsideLock);
			return;
		}

		ZenGrantPermissionFromCorrectCode(successfulPlayer);

		// Entering the code while the associated door is already open intentionally
		// performs the native 1.30 detach operation.
		if (ZenComboLocksHelper.IsOpenForLock(this))
		{
			super.UnlockOnServer(player, parent, isInsideLock);
			ZenComboLocksLogger.Log("Player " + successfulPlayer.GetIdentity().GetPlainId() + " detached " + GetType() + " with its PIN at " + GetPosition());
			return;
		}

		if (!ZenComboLocksHelper.OpenForLock(this))
		{
			super.UnlockOnServer(player, parent, isInsideLock);
			return;
		}

		// Keep the lock attached and immediately restore the locked visual/state.
		LockServer(parent, true);
		ZenComboLocksLogger.Log("Player " + successfulPlayer.GetIdentity().GetPlainId() + " entered the PIN and opened " + GetType() + " at " + GetPosition());
	}

	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);

		if (!ZenComboLocks_IsEnabled() || !g_Game || !g_Game.IsServer())
		{
			return;
		}

		PlayerBase attachingPlayer = PlayerBase.Cast(old_owner);
		if (!attachingPlayer || !attachingPlayer.GetIdentity() || !new_owner || !new_owner.CanUseConstruction())
		{
			return;
		}

		ZenEnsurePermissionData();
		m_ZenComboLockData.m_PermittedPlayers.Clear();
		ZenSetOwnerPermission(attachingPlayer.GetIdentity().GetPlainId());
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (!ZenComboLocks_IsEnabled())
		{
			return;
		}

		if (rpc_type == ZENCOMBOLOCKRPCs.RECEIVE_COMBO_DATA)
		{
			Param2<bool, bool> clientPayload = new Param2<bool, bool>(false, false);
			if (ctx.Read(clientPayload))
			{
				m_ZenHasClientPermission = clientPayload.param1;
				m_ZenIsClientOwner = clientPayload.param2;
				m_ZenReceivedPermissions = true;
				m_ZenRequestedPermissions = false;
			}

			return;
		}

		if (rpc_type != ZENCOMBOLOCKRPCs.REQUEST_COMBO_DATA || !sender)
		{
			return;
		}

		Param1<bool> serverRequest = new Param1<bool>(false);
		if (!ctx.Read(serverRequest) || !serverRequest.param1)
		{
			return;
		}

		PlayerBase requestingPlayer = PlayerBase.Cast(sender.GetPlayer());
		if (!requestingPlayer || !requestingPlayer.GetIdentity())
		{
			return;
		}

		bool playerHasPermission = ZenHasServerPermission(requestingPlayer);
		bool playerIsOwner = ZenIsServerOwner(requestingPlayer);
		Param2<bool, bool> serverPayload = new Param2<bool, bool>(playerHasPermission, playerIsOwner);
		RPCSingleParam(ZENCOMBOLOCKRPCs.RECEIVE_COMBO_DATA, serverPayload, true, requestingPlayer.GetIdentity());
	}

	override void CF_OnStoreSave(CF_ModStorageMap storage)
	{
		super.CF_OnStoreSave(storage);

		auto saveContext = storage["ZenComboLocks"];
		if (!saveContext)
		{
			return;
		}

		ZenEnsurePermissionData();
		int savedPlayerCount = m_ZenComboLockData.m_PermittedPlayers.Count();
		saveContext.Write(savedPlayerCount);

		for (int savedPlayerIndex = 0; savedPlayerIndex < savedPlayerCount; savedPlayerIndex++)
		{
			saveContext.Write(m_ZenComboLockData.m_PermittedPlayers.Get(savedPlayerIndex));
		}

		saveContext.Write(m_ZenComboLockData.m_PlayerOwner);
	}

	override bool CF_OnStoreLoad(CF_ModStorageMap storage)
	{
		if (!super.CF_OnStoreLoad(storage))
		{
			return false;
		}

		auto loadContext = storage["ZenComboLocks"];
		if (!loadContext || loadContext.GetVersion() < 1)
		{
			return true;
		}

		m_ZenComboLockData = new ComboLockData();
		int loadedPlayerCount;
		if (!loadContext.Read(loadedPlayerCount))
		{
			return false;
		}

		for (int loadedPlayerIndex = 0; loadedPlayerIndex < loadedPlayerCount; loadedPlayerIndex++)
		{
			string loadedPlayerID;
			if (!loadContext.Read(loadedPlayerID))
			{
				return false;
			}

			m_ZenComboLockData.m_PermittedPlayers.Insert(loadedPlayerID);
		}

		string loadedOwnerID;
		if (!loadContext.Read(loadedOwnerID))
		{
			return false;
		}

		m_ZenComboLockData.m_PlayerOwner = loadedOwnerID;
		return true;
	}
}
