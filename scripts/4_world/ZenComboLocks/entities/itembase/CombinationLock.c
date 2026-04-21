modded class CombinationLock
{
	//------------ <Client-side> ------------
	ref ZenComboLockGUI m_ZenComboLockGUI;
	protected bool m_RequestedPerms;
	protected bool m_ReceivedPerms;
	protected bool m_HasClientPermission;
	protected bool m_IsManagingLockClient;
	protected int m_LastSimulatedDialChanges;

	void CombinationLock()
	{
		m_RequestedPerms = false;
		m_ReceivedPerms = false;
		m_HasClientPermission = false;
		m_IsManagingLockClient = false;
		m_LastSimulatedDialChanges = -1;
	}

	void ~CombinationLock()
	{
		if (m_ZenComboLockGUI)
			delete m_ZenComboLockGUI;
	}

	override void EOnInit(IEntity other, int extra)
	{
		super.EOnInit(other, extra);

		#ifdef ZenModPack
		if (!ZenModEnabled("ZenComboLocks"))
			return;
		#endif

		if (g_Game.IsDedicatedServer())
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckLockRestart, 5000, false);
	}

	// Returns true if the player is currently managing this lock (allows digit change action for removing lock)
	bool IsManagingLockClient()
	{
		if (g_Game.IsDedicatedServer())
			return false;

		return m_IsManagingLockClient;
	}

	// Set whether or not the player is currently managing this lock (for client-side actions)
	void SetManagingLockClient(bool b)
	{
		m_IsManagingLockClient = b;
	}

	// Send request to server for combo lock permission sync
	void RequestCombolockPerms()
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());

		// Only request sync after player has been logged in for at least a few seconds to avoid potential nullpointer on server
		if (!player || player.GetSimulationTimeStamp() < 300)
		{
			// Don't request sync too quickly or server-side RPC playerbase check returns a null player?
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(RequestCombolockPerms);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RequestCombolockPerms, 300, false);
			return;
		}

		// If we have not received client-side combo lock data, request it
		if (!m_ReceivedPerms)
		{
			Print("[ZenComboLocks] Sending request for combination lock permissions @ " + player.GetSimulationTimeStamp() + " timestamp.");

			// Send request
			Param1<bool> params = new Param1<bool>(true);
			RPCSingleParam(ZENCOMBOLOCKRPCs.REQUEST_COMBO_DATA, params, true, NULL);
		}
	}

	// Display digit text (client-side only!)
	void DisplayDigitText(string prefix = "")
	{
		// Create new combo lock digit if needed
		if (!m_ZenComboLockGUI)
			m_ZenComboLockGUI = new ZenComboLockGUI;

		// Display code to GUI
		if (GetZenComboLocksConfig().ClientSyncConfig.DisplayDigits == 1)
		{
			m_ZenComboLockGUI.SetText(prefix + GetDialNumberText(false).Get(m_DialIndex));
		}
		else
		if (GetZenComboLocksConfig().ClientSyncConfig.DisplayDigits == 2)
		{
			m_ZenComboLockGUI.SetText(prefix + GetDialNumberText(true));
		}
	}

	// Copied from ConstructionActionData.c but removed the part that highlights current digit
	string GetDialNumberText(bool formatDigits)
	{
		string dial_text = "";
		string combination_text = GetCombination().ToString();

		// Insert zeros to dials with 0 value
		int length_diff = GetLockDigits() - combination_text.Length();
		for (int i = 0; i < length_diff; ++i)
		{
			combination_text = "0" + combination_text;
		}

		// Set numbers
		for (int j = 0; j < GetLockDigits(); ++j)
		{
			if (!formatDigits)
			{
				dial_text += string.Format("%1", combination_text.Get(j));
			}
			else
			{
				if (j == GetDialIndex())
				{
					dial_text += string.Format("[%1]", combination_text.Get(j));
				}
				else
				{
					dial_text += string.Format(" %1 ", combination_text.Get(j));
				}
			}
		}

		return dial_text;
	}
	//------------ </Client-side> ------------

	//------------- <Server-side> ------------
	protected ref ComboLockData m_ComboLockData = NULL;
	protected int m_SimulatedDialChanges = 0;

	// Check if lock should be locked
	void CheckLockRestart()
	{
		if (GetHierarchyParent() && GetHierarchyParent().IsInherited(BaseBuildingBase))
		{
			LockServer(GetHierarchyParent(), true);
		}
	}

	// Lock the... lock.. to the gate
	void LockTheLockToTheGate()
	{
		if (GetHierarchyParent() && (GetHierarchyParent().IsInherited(BaseBuildingBase) || GetHierarchyParent().IsKindOf("BBP_WALL_BASE")))
		{
			InventoryLocation inventory_location = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation(inventory_location);
			GetHierarchyParent().GetInventory().SetSlotLock(inventory_location.GetSlot(), true);
			m_LockActionPerformed = LockAction.LOCKED;
			SetTakeable(false);
			Synchronize();
		}
	}

	// Unlock the lock from the gate
	void UnlockTheLockFromTheGate()
	{
		if (GetHierarchyParent() && (GetHierarchyParent().IsInherited(BaseBuildingBase) || GetHierarchyParent().IsKindOf("BBP_WALL_BASE")))
		{
			InventoryLocation inventory_location = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation(inventory_location);
			GetHierarchyParent().GetInventory().SetSlotLock(inventory_location.GetSlot(), false);
			m_LockActionPerformed = LockAction.UNLOCKED;
			SetTakeable(true);
			Synchronize();
		}
	}

	// Reset simulated dial changes
	void ResetSimulatedDialChanges()
	{
		m_SimulatedDialChanges = 0;
	}

	// Increase simulated dial change count and sync sounds to clients
	void IncreaseSimulatedDialChanges()
	{
		m_SimulatedDialChanges = m_SimulatedDialChanges + 1;

		if (m_SimulatedDialChanges % ((GetLockDigits() * GetZenComboLocksConfig().ServerConfig.DigitMultiplier) / 2) == 0)
			m_LockActionPerformed = LockAction.DIAL_NUMBER_CHANED;
		else
			m_LockActionPerformed = LockAction.DIAL_INDEX_CHANGED;

		//if (m_SimulatedDialChanges >= GetLockDigits() * GetZenComboLocksConfig().ServerConfig.DigitMultiplier)
		//	m_LockActionPerformed = LockAction.UNLOCKED;

		Synchronize();
	}

	// Return currently simulated dial change count
	int GetSimulatedDialChangeCount()
	{
		return m_SimulatedDialChanges;
	}

	// Return combo lock permissions data
	ComboLockData GetComboLockData()
	{
		return m_ComboLockData;
	}

	// Trigger sync 
	void TriggerZenSync()
	{
		if (g_Game.IsDedicatedServer())
		{
			m_ServerSyncID++;
			SetSynchDirty();
		}
	}

	// Set owner
	bool SetOwnerPermission(string id)
	{
		if (!m_ComboLockData)
		{
			m_ComboLockData = new ComboLockData();
		}

		ZenComboLocksLogger.Log("Assigned new OWNER to combination lock: " + id + " - " + GetType() + " / digits=" + GetLockDigits() + " / loc=" + GetPosition());
		m_ComboLockData.m_PlayerOwner = id;
		TriggerZenSync();

		return true;
	}

	// Add guest permission
	bool AddGuestPermission(string id)
	{
		if (!m_ComboLockData)
		{
			m_ComboLockData = new ComboLockData();
		}

		// If ID is equal to owner's ID, no action required
		if (m_ComboLockData.m_PlayerOwner != id)
		{
			foreach(string s : m_ComboLockData.m_PermittedPlayers)
			{
				// Guest already exists, no need to add them again
				if (s == id)
				{
					return false;
				}
			}
		}

		ZenComboLocksLogger.Log("Assigned new GUEST to combination lock: " + id + " - " + GetType() + " / digits=" + GetLockDigits() + " / loc=" + GetPosition());
		m_ComboLockData.m_PermittedPlayers.Insert(id);
		TriggerZenSync();

		return true;
	}

	// Resets player ownership
	void ResetPlayerPermissions()
	{
		if (!m_ComboLockData)
		{
			m_ComboLockData = new ComboLockData();
		}

		m_ComboLockData.m_PlayerOwner = "";
		m_ComboLockData.m_PermittedPlayers.Clear();
		TriggerZenSync();
	}
	//------------ </Server-side> ------------

	//---------------- <Both> ----------------
	int m_ClientSyncID = -1;
	int m_ServerSyncID = 0;

	// Register server/client sync variable
	override void InitItemVariables()
	{
		super.InitItemVariables();

		RegisterNetSyncVariableInt("m_ServerSyncID");
		RegisterNetSyncVariableInt("m_SimulatedDialChanges");
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		#ifdef ZenModPack
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
		#endif

		// Server has forced re-sync
		if (m_ClientSyncID != m_ServerSyncID)
		{
			// Request server permission data
			RequestCombolockPerms();
			m_ClientSyncID = m_ServerSyncID;
			m_RequestedPerms = false;
			m_ReceivedPerms = false;
		}

		// If we've received a lock update but not perms, request them
		if (!m_ReceivedPerms)
		{
			RequestCombolockPerms();
		}

		// Check if we need to play a sound
		if (m_SimulatedDialChanges != m_LastSimulatedDialChanges)
		{
			m_LastSimulatedDialChanges = m_SimulatedDialChanges;
			UpdateSound();
		}
	}

	// Handle RPCs for combo lock
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		#ifdef ZenModPack
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
		#endif

		// Client-side receiver for combo lock data
		if (rpc_type == ZENCOMBOLOCKRPCs.RECEIVE_COMBO_DATA)
		{
			Param1<bool> client_params = new Param1<bool>(m_HasClientPermission);
			if (ctx.Read(client_params))
			{
				m_HasClientPermission = client_params.param1;
				m_ReceivedPerms = true;
			}

			return;
		}

		// Server-side receiver for request
		if (rpc_type == ZENCOMBOLOCKRPCs.REQUEST_COMBO_DATA)
		{
			// If we can't identify the player who sent this request, stop here.
			if (!sender)
				return;

			Param1<bool> server_params = new Param1<bool>(false);
			if (ctx.Read(server_params))
			{
				if (server_params.param1)
				{
					// If combo data is null, create it
					if (!m_ComboLockData)
					{
						m_ComboLockData = new ComboLockData;
					}

					// Get player (note: this returns NULL if player requests sync right on login? Added short delay)
					PlayerBase player = PlayerBase.Cast(sender.GetPlayer());

					if (!player)
					{
						Error("PLAYER NETWORK OBJECT NOT FOUND! Please tell Zenarchist: https://steamcommunity.com/sharedfiles/filedetails/?id=2864891982");
						return;
					}

					Print("[ZenComboLocks] Sending requested combo lock permissions for lock @ " + GetPosition() + " to " + player.GetIdentity().GetId());

					// Sync combo lock permissions to client
					SyncComboLockPermissions(player);
				}
			}

			return;
		}
	}

	// Return true if we've already received permission data, otherwise request perms from server
	bool HasReceivedClientsidePerms()
	{
		// If this is called on the server just return true
		if (g_Game.IsDedicatedServer())
			return true;

		// If we've already received permission data return true
		if (m_ReceivedPerms)
			return true;

		// If we haven't received permissions, request them from server
		if (!m_RequestedPerms)
		{
			// RPC-> client perms (set up a repeated call)
			m_RequestedPerms = true;
			RequestCombolockPerms();
		}

		// This will only return true once combo lock permission data has been received by the client
		return false;
	}

	// Send permissions from server to client
	void SyncComboLockPermissions(notnull PlayerBase player)
	{
		// If player identity doesn't exist, stop here
		if (!player.GetIdentity())
			return;

		// Get combo lock permissions
		bool hasPerms = IsPermittedToOpen(player);

		// Send combo lock permissions
		Param1<bool> data = new Param1<bool>(hasPerms);
		RPCSingleParam(ZENCOMBOLOCKRPCs.RECEIVE_COMBO_DATA, data, true, player.GetIdentity());
	}

	// Is the given player ID permitted to open this combo lock?
	bool IsPermittedToOpen(PlayerBase player = NULL)
	{
		// If we're checking client perms, return client bool
		if (g_Game.IsClient() || player == NULL)
		{
			return m_HasClientPermission;
		}
		
		PlayerIdentity playerID = player.GetIdentity();

		if (!playerID)
			return false;

		string id = playerID.GetPlainId();

		// Check server admin permissions
		if (GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen)
		{
			if (GetZenCoreConfig().IsAdmin(playerID))
				return true;
		}

		// Check for combo data
		if (!m_ComboLockData)
		{
			m_ComboLockData = new ComboLockData();
			return false;
		}

		// Check owner
		if (m_ComboLockData.m_PlayerOwner == id)
			return true;

		// Check "guests"
		foreach(string guestID : m_ComboLockData.m_PermittedPlayers)
		{
			if (guestID == id)
				return true;
		}

		return false;
	}

	//---------------- </Both> ----------------

	// VANILLA OVERRIDES

	// Necessary because I override combination lock behaviour - when a lock is unlocked it stays attached to the fence but is technically unlocked
	// So we need to lock the combolock slot to the fence whenever a gate is closed/combo lock attached/server restarts and lock is unlocked etc
	override void LockServer(EntityAI parent, bool ignore_combination = false)
	{
		super.LockServer(parent, ignore_combination);

		#ifdef ZenModPack
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
		#endif

		// Check if we're on a gate/door, if so, lock the combination lock to parent so it can't be taken to hands/inventory
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(LockTheLockToTheGate);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(LockTheLockToTheGate, 100, false);
	}

	override bool IsLocked()
	{
		#ifdef ZenModPack
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return super.IsLocked();
		}
		#endif

		if (IsTakeable())
			return false;

		return super.IsLocked();
	}

	// BF compatibility
	override bool IsLockedOnGate()
	{
		if (IsTakeable())
			return false;

		if (IsLocked())
		{
			#ifdef BuildingFort_Mod_1A
			BuildingFortficationsCore bf_door = BuildingFortficationsCore.Cast(GetHierarchyParent());
			if (bf_door)
				return true;
			#endif
		}

		// super just checks if parent is Fence
		return super.IsLockedOnGate();
	}

	// BBP does not call super, so unfortunately I need to do this. All my custom actions call UnlockServerZen directly, so this is purely for BBP compatibility
	override void UnlockServer(EntityAI player, EntityAI parent)
	{
		#ifdef ZenModPack
		if (!ZenModEnabled("ZenComboLocks"))
		{
			super.UnlockServer(player, parent);
			return;
		}
		#endif

		EntityAI hparent = GetHierarchyParent();
		if (hparent != NULL)
		{
			if (GetHierarchyParent().IsKindOf("BBP_WALL_BASE"))
			{
				UnlockServerZen(player, parent);
				return;
			}
		}

		super.UnlockServer(player, parent);
	}

	// This is a custom version of UnlockServer(), because some mods change this function without calling super() and it breaks my mod's compatibility with them
	void UnlockServerZen(EntityAI player, EntityAI parent)
	{
		if (IsLockAttached() && parent)
		{
			// Check player permissions
			if (player)
			{
				PlayerBase pb = PlayerBase.Cast(player);
				if (pb && pb.GetIdentity())
				{
					if (!m_ComboLockData)
					{
						m_ComboLockData = new ComboLockData;
					}

					// If player is not on permission list, add them
					if (!IsPermittedToOpen(pb))
					{
						if (m_ComboLockData.m_PlayerOwner == "")
						{
							// If there is no assigned owner, set this player as owner
							SetOwnerPermission(pb.GetIdentity().GetPlainId());
						}
						else
						{
							// Otherwise if the lock already has an owner, add player as guest
							AddGuestPermission(pb.GetIdentity().GetPlainId());
						}

						// Force re-sync nearby client permissions for this lock
						TriggerZenSync();
					}
				}
			}

			Synchronize();
		}

		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(UnlockTheLockFromTheGate);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UnlockTheLockFromTheGate, 100, false);
	}

	// Detect item location changed - ie. attached to a fence or object
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		#ifdef ZenModPack
		if (!ZenModEnabled("ZenComboLocks"))
		{
			super.OnItemLocationChanged(old_owner, new_owner);
			return;
		}
		#endif

		if (!old_owner || !old_owner.IsInherited(PlayerBase))
			return;

		if (g_Game.IsDedicatedServer())
		{
			if (IsInitialized() && new_owner && (new_owner.IsInherited(BaseBuildingBase) || new_owner.IsKindOf("BBP_WALL_BASE")))
			{
				LockServer(new_owner); // Lock combo lock to gate

				PlayerBase player = PlayerBase.Cast(old_owner);
				if (player && player.GetIdentity())
				{
					// Player has attached combo lock to new fence or with new code, set new owner
					SetOwnerPermission(player.GetIdentity().GetPlainId());

					// Wipe guests since owner has changed or code has changed
					m_ComboLockData.m_PermittedPlayers.Clear();
					TriggerZenSync(); // Force nearby clients to re-sync
					Synchronize();
				}
			}
		}
		else
		{
			// Reset lock management boo-lean
			m_IsManagingLockClient = false;
		}
	}

	// Use CF_Load/Save because this will NOT break server persistence if the mod is added/removed mid-wipe.
	
	override void CF_OnStoreSave(CF_ModStorageMap storage)
	{
		super.CF_OnStoreSave(storage);

		auto ctx = storage["ZenComboLocks"];
		if (!ctx) return;
		
		if (!m_ComboLockData)
			m_ComboLockData = new ComboLockData();
		
		int playerCount = m_ComboLockData.m_PermittedPlayers.Count();
		
		ctx.Write(playerCount);
		for (int i = 0; i < playerCount; i++)
		{
			ctx.Write(m_ComboLockData.m_PermittedPlayers.Get(i));
		}
		
		ctx.Write(m_ComboLockData.m_PlayerOwner);
	}

	override bool CF_OnStoreLoad(CF_ModStorageMap storage)
	{
		if (!super.CF_OnStoreLoad(storage)) return false;

		auto ctx = storage["ZenComboLocks"];
		if (!ctx) return true;

		if (ctx.GetVersion() >= 1)
		{
			m_ComboLockData = new ComboLockData();
			
			int playerCount;
			string ownerPlayerID;
			
			if (!ctx.Read(playerCount)) return false;
			
			for (int i = 0; i < playerCount; i++)
			{
				string loadID;
				if (!ctx.Read(loadID))
					return false;
				
				m_ComboLockData.m_PermittedPlayers.Insert(loadID);
			}
			
			string ownerID;
			if (!ctx.Read(ownerID))
				return false;
			
			m_ComboLockData.m_PlayerOwner = ownerID;
		}

		return true;
	}

	// Set new combo lock actions
	override void SetActions()
	{
		super.SetActions();

		// Add my vanilla action changes
		AddAction(Zen_ActionNextCombinationLockDialOnTarget);
		AddAction(Zen_ActionDialCombinationLockOnTarget);

		// Add my custom actions
		AddAction(Zen_ActionOpenComboLock);
		AddAction(Zen_ActionOpenComboLockInstant);
		AddAction(Zen_ActionManageCombinationLockOnTarget);
		AddAction(Zen_ActionAdminCombinationLockOnTarget);
	}
}