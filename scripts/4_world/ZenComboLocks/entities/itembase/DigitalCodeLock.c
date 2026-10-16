modded class DigitalCodeLock
{
	protected ref ComboLockData m_ZenDigitalLockData;
	protected bool m_ZenDigitalRequestedPermissions;
	protected bool m_ZenDigitalReceivedPermissions;
	protected bool m_ZenDigitalHasClientPermission;
	protected int m_ZenDigitalClientSyncID;
	protected int m_ZenDigitalServerSyncID;
	protected int m_ZenDigitalOpenProgress;

	void DigitalCodeLock()
	{
		m_ZenDigitalRequestedPermissions = false;
		m_ZenDigitalReceivedPermissions = false;
		m_ZenDigitalHasClientPermission = false;
		m_ZenDigitalClientSyncID = -1;
		m_ZenDigitalServerSyncID = 0;
		m_ZenDigitalOpenProgress = 0;
	}

	void ~DigitalCodeLock()
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ZenRequestDigitalPermissions);
		}
	}

	override void InitItemVariables()
	{
		super.InitItemVariables();
		RegisterNetSyncVariableInt("m_ZenDigitalServerSyncID");
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (!ZenComboLocks_IsEnabled())
		{
			return;
		}

		if (m_ZenDigitalClientSyncID != m_ZenDigitalServerSyncID)
		{
			m_ZenDigitalClientSyncID = m_ZenDigitalServerSyncID;
			m_ZenDigitalRequestedPermissions = false;
			m_ZenDigitalReceivedPermissions = false;
			ZenRequestDigitalPermissions();
		}
	}

	void ZenRequestDigitalPermissions()
	{
		if (!ZenComboLocks_IsEnabled() || !g_Game || !g_Game.IsClient())
		{
			return;
		}

		PlayerBase digitalLocalPlayer = PlayerBase.Cast(g_Game.GetPlayer());
		if (!digitalLocalPlayer || digitalLocalPlayer.GetSimulationTimeStamp() < 300)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ZenRequestDigitalPermissions);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ZenRequestDigitalPermissions, 300, false);
			return;
		}

		if (m_ZenDigitalReceivedPermissions || m_ZenDigitalRequestedPermissions)
		{
			return;
		}

		m_ZenDigitalRequestedPermissions = true;
		Param1<bool> digitalRequestPayload = new Param1<bool>(true);
		RPCSingleParam(ZENCOMBOLOCKRPCs.REQUEST_COMBO_DATA, digitalRequestPayload, true, NULL);
	}

	bool ZenHasReceivedDigitalClientPermissions()
	{
		if (!g_Game || !g_Game.IsClient())
		{
			return true;
		}

		if (m_ZenDigitalReceivedPermissions)
		{
			return true;
		}

		ZenRequestDigitalPermissions();
		return false;
	}

	bool ZenCanOpenDigitalLock(PlayerBase player)
	{
		if (g_Game && g_Game.IsServer())
		{
			return ZenHasDigitalServerPermission(player);
		}

		return m_ZenDigitalHasClientPermission;
	}

	protected bool ZenHasDigitalServerPermission(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
		{
			return false;
		}

		PlayerIdentity digitalPermissionIdentity = player.GetIdentity();
		if (GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen && GetZenCoreConfig().IsAdmin(digitalPermissionIdentity))
		{
			return true;
		}

		ZenEnsureDigitalPermissionData();
		string digitalPermissionID = digitalPermissionIdentity.GetPlainId();
		if (m_ZenDigitalLockData.m_PlayerOwner == digitalPermissionID)
		{
			return true;
		}

		foreach (string digitalGuestID : m_ZenDigitalLockData.m_PermittedPlayers)
		{
			if (digitalGuestID == digitalPermissionID)
			{
				return true;
			}
		}

		return false;
	}

	protected void ZenEnsureDigitalPermissionData()
	{
		if (!m_ZenDigitalLockData)
		{
			m_ZenDigitalLockData = new ComboLockData();
		}
	}

	void ZenRememberDigitalIdentity(PlayerIdentity identity, bool isCodeSetter)
	{
		if (!identity || !CanSetCode())
		{
			return;
		}

		ZenEnsureDigitalPermissionData();
		string rememberedDigitalID = identity.GetPlainId();

		if (isCodeSetter || m_ZenDigitalLockData.m_PlayerOwner == "")
		{
			m_ZenDigitalLockData.m_PermittedPlayers.Clear();
			m_ZenDigitalLockData.m_PlayerOwner = rememberedDigitalID;
			ZenComboLocksLogger.Log("Assigned digital lock owner " + rememberedDigitalID + " to " + GetType() + " at " + GetPosition());
			ZenTriggerDigitalPermissionSync();
			return;
		}

		if (m_ZenDigitalLockData.m_PlayerOwner == rememberedDigitalID)
		{
			return;
		}

		foreach (string existingDigitalGuestID : m_ZenDigitalLockData.m_PermittedPlayers)
		{
			if (existingDigitalGuestID == rememberedDigitalID)
			{
				return;
			}
		}

		m_ZenDigitalLockData.m_PermittedPlayers.Insert(rememberedDigitalID);
		ZenComboLocksLogger.Log("Remembered digital lock guest " + rememberedDigitalID + " for " + GetType() + " at " + GetPosition());
		ZenTriggerDigitalPermissionSync();
	}

	void ZenResetDigitalPermissions()
	{
		ZenEnsureDigitalPermissionData();
		m_ZenDigitalLockData.m_PlayerOwner = "";
		m_ZenDigitalLockData.m_PermittedPlayers.Clear();
		ZenTriggerDigitalPermissionSync();
	}

	protected void ZenTriggerDigitalPermissionSync()
	{
		if (g_Game && g_Game.IsServer())
		{
			m_ZenDigitalServerSyncID++;
			SetSynchDirty();
		}
	}

	void ZenResetDigitalOpenProgress()
	{
		m_ZenDigitalOpenProgress = 0;
	}

	bool ZenAdvanceDigitalOpenProgress()
	{
		m_ZenDigitalOpenProgress++;

		int digitalPINLength = GetLockPIN().Length();
		if (digitalPINLength < GetMinLockPINLength())
		{
			digitalPINLength = GetMinLockPINLength();
		}

		int requiredDigitalProgress = digitalPINLength * GetZenComboLocksConfig().ServerConfig.DigitMultiplier;
		if (requiredDigitalProgress < 1)
		{
			requiredDigitalProgress = 1;
		}

		return m_ZenDigitalOpenProgress >= requiredDigitalProgress;
	}

	bool ZenOpenForRememberedDigitalPlayer(ActionTarget actionTarget)
	{
		if (!IsLocked())
		{
			return false;
		}

		bool openedDigitalParent = ZenComboLocksHelper.OpenForDigitalActionTarget(this, actionTarget);
		if (openedDigitalParent)
		{
			StartItemSoundServer(SoundConstants.ITEM_DIGITAL_LOCK_ACCESS);
		}

		return openedDigitalParent;
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
			Param1<bool> digitalClientPayload = new Param1<bool>(false);
			if (ctx.Read(digitalClientPayload))
			{
				m_ZenDigitalHasClientPermission = digitalClientPayload.param1;
				m_ZenDigitalReceivedPermissions = true;
				m_ZenDigitalRequestedPermissions = false;
			}

			return;
		}

		if (rpc_type != ZENCOMBOLOCKRPCs.REQUEST_COMBO_DATA || !sender)
		{
			return;
		}

		Param1<bool> digitalServerRequest = new Param1<bool>(false);
		if (!ctx.Read(digitalServerRequest) || !digitalServerRequest.param1)
		{
			return;
		}

		PlayerBase digitalRequestingPlayer = PlayerBase.Cast(sender.GetPlayer());
		if (!digitalRequestingPlayer || !digitalRequestingPlayer.GetIdentity())
		{
			return;
		}

		bool digitalPlayerHasPermission = ZenHasDigitalServerPermission(digitalRequestingPlayer);
		Param1<bool> digitalServerPayload = new Param1<bool>(digitalPlayerHasPermission);
		RPCSingleParam(ZENCOMBOLOCKRPCs.RECEIVE_COMBO_DATA, digitalServerPayload, true, digitalRequestingPlayer.GetIdentity());
	}

	override void CF_OnStoreSave(CF_ModStorageMap storage)
	{
		super.CF_OnStoreSave(storage);

		auto digitalSaveContext = storage["ZenComboLocks"];
		if (!digitalSaveContext)
		{
			return;
		}

		ZenEnsureDigitalPermissionData();
		int savedDigitalPlayerCount = m_ZenDigitalLockData.m_PermittedPlayers.Count();
		digitalSaveContext.Write(savedDigitalPlayerCount);

		for (int savedDigitalPlayerIndex = 0; savedDigitalPlayerIndex < savedDigitalPlayerCount; savedDigitalPlayerIndex++)
		{
			digitalSaveContext.Write(m_ZenDigitalLockData.m_PermittedPlayers.Get(savedDigitalPlayerIndex));
		}

		digitalSaveContext.Write(m_ZenDigitalLockData.m_PlayerOwner);
	}

	override bool CF_OnStoreLoad(CF_ModStorageMap storage)
	{
		if (!super.CF_OnStoreLoad(storage))
		{
			return false;
		}

		auto digitalLoadContext = storage["ZenComboLocks"];
		if (!digitalLoadContext || digitalLoadContext.GetVersion() < 1)
		{
			return true;
		}

		m_ZenDigitalLockData = new ComboLockData();
		int loadedDigitalPlayerCount;
		if (!digitalLoadContext.Read(loadedDigitalPlayerCount))
		{
			return false;
		}

		for (int loadedDigitalPlayerIndex = 0; loadedDigitalPlayerIndex < loadedDigitalPlayerCount; loadedDigitalPlayerIndex++)
		{
			string loadedDigitalPlayerID;
			if (!digitalLoadContext.Read(loadedDigitalPlayerID))
			{
				return false;
			}

			m_ZenDigitalLockData.m_PermittedPlayers.Insert(loadedDigitalPlayerID);
		}

		string loadedDigitalOwnerID;
		if (!digitalLoadContext.Read(loadedDigitalOwnerID))
		{
			return false;
		}

		m_ZenDigitalLockData.m_PlayerOwner = loadedDigitalOwnerID;
		return true;
	}
}

modded class CodeLockComponent
{
	override protected void CheckCodeRequest(string pin, PlayerIdentity identity)
	{
		bool hadDigitalPINBeforeRequest = m_HasPIN;
		bool wasDigitalLockedBeforeRequest = m_IsLocked;
		string digitalPINBeforeRequest = m_LockPIN;

		super.CheckCodeRequest(pin, identity);

		DigitalCodeLock rememberedDigitalLock = DigitalCodeLock.Cast(m_Parent);
		if (!ZenComboLocks_IsEnabled() || !rememberedDigitalLock || !rememberedDigitalLock.CanSetCode() || !identity)
		{
			return;
		}

		if (!hadDigitalPINBeforeRequest && m_HasPIN && pin == m_LockPIN)
		{
			rememberedDigitalLock.ZenRememberDigitalIdentity(identity, true);
			return;
		}

		if (hadDigitalPINBeforeRequest && wasDigitalLockedBeforeRequest && !m_IsLocked && pin == digitalPINBeforeRequest)
		{
			rememberedDigitalLock.ZenRememberDigitalIdentity(identity, false);
		}
	}

	override void ResetPIN(bool sync = true)
	{
		super.ResetPIN(sync);

		DigitalCodeLock resetDigitalLock = DigitalCodeLock.Cast(m_Parent);
		if (ZenComboLocks_IsEnabled() && resetDigitalLock && resetDigitalLock.CanSetCode())
		{
			resetDigitalLock.ZenResetDigitalPermissions();
		}
	}
}
