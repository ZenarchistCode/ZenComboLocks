ref ZenComboLocksConfig g_ZenComboLocksConfig;

static ZenComboLocksConfig GetZenComboLocksConfig()
{
	if (!g_ZenComboLocksConfig) GetZenConfigRegister().RegisterConfig(ZenComboLocksConfig);
	return g_ZenComboLocksConfig;
}

modded class ZenConfigRegister
{
	override void RegisterPreload()
	{
		super.RegisterPreload(); 
		RegisterType(ZenComboLocksConfig);
	}
}

class ZenComboLocksConfig_SyncPayload
{
	ref ZenComboLocksClientConfig ClientSyncConfig;
	
	void ZenComboLocksConfig_SyncPayload()
	{
		ClientSyncConfig = new ZenComboLocksClientConfig();
	}
}

class ZenComboLocksConfig : ZenConfigBase
{
	// -------------------------
	// CONFIG SETTINGS
	// -------------------------
	override void OnRegistered()
	{
		g_ZenComboLocksConfig = this;
	}
	
	override string GetCurrentVersion() { return "1.29.2"; }
	
	override bool ShouldLoadOnServer() 		
	{ 
		#ifdef ZenModPack
		return ZenModEnabled("ZenComboLocks");
		#endif
		
		return true; 
	}
	
	override bool ShouldSyncToClient()		
	{ 
		#ifdef ZenModPack
		return ZenModEnabled("ZenComboLocks");
		#endif
		
		return true; 
	}
	
	override bool ReadJson(string path, out string err)
	{
		return JsonFileLoader<ZenComboLocksConfig>.LoadFile(path, this, err);
	}

	override bool WriteJson(string path, out string err)
	{
		return JsonFileLoader<ZenComboLocksConfig>.SaveFile(path, this, err);
	}

	override bool BuildSyncPayload(out string payload, out string err)
	{
		ZenComboLocksConfig_SyncPayload snap = new ZenComboLocksConfig_SyncPayload();

		snap.ClientSyncConfig = ClientSyncConfig;

		// Serialize payload only (NOT the whole config)
		return JsonFileLoader<ZenComboLocksConfig_SyncPayload>.MakeData(snap, payload, err, false);
	}

	override bool ApplySyncPayload(string payload, out string err)
	{
		// Create default containers in case an error happens on read
		ClientSyncConfig = new ZenComboLocksClientConfig();

		ZenComboLocksConfig_SyncPayload snap = new ZenComboLocksConfig_SyncPayload();
		if (!JsonFileLoader<ZenComboLocksConfig_SyncPayload>.LoadData(payload, snap, err))
		{
			return false;
		}

		ClientSyncConfig = snap.ClientSyncConfig;
		
		return true;
	}
	
	// -------------------------
	// CONFIG VARIABLES
	// -------------------------
	ref ZenComboLocksServerConfig ServerConfig; // Config that is server-only
	ref ZenComboLocksClientConfig ClientSyncConfig; // Config that sync's to client
	
	// -------------------------
	// CONFIG FUNCTIONS
	// -------------------------
	override void SetDefaults()
	{
		ServerConfig = new ZenComboLocksServerConfig();
		ClientSyncConfig = new ZenComboLocksClientConfig();
		
		ClientSyncConfig.RaidConfig = new ZenComboLocksRaidConfig();
		ClientSyncConfig.RaidConfig.LockConfig = new array<ref ZenLockConfig>;

		// Vanilla 3-dial
		ZenLockConfig lockConfig3 = new ZenLockConfig();
		lockConfig3.LockType = "CombinationLock";
		lockConfig3.TimeRequired = 30; // 30 seconds
		lockConfig3.DamageTool = 250; // Durability: 4 locks
		ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig3);

		// Vanilla 4-dial
		ZenLockConfig lockConfig4 = new ZenLockConfig();
		lockConfig4.LockType = "CombinationLock4";
		lockConfig4.TimeRequired = 60; // 1 minute
		lockConfig4.DamageTool = 500; // Durability: 2 locks
		ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig4);

		// Vanilla 4-dial
		ZenLockConfig lockConfig4Zen = new ZenLockConfig();
		lockConfig4Zen.LockType = "Zen_CombinationLock4";
		lockConfig4Zen.TimeRequired = 60; // 1 minute
		lockConfig4Zen.DamageTool = 500; // Durability: 2 locks
		ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig4Zen);

		// Zen 5-dial
		ZenLockConfig lockConfig5 = new ZenLockConfig();
		lockConfig5.LockType = "Zen_CombinationLock5";
		lockConfig5.TimeRequired = 90; // 1.5 minute
		lockConfig5.DamageTool = 600;
		ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig5);

		// Zen 6-dial
		ZenLockConfig lockConfig6 = new ZenLockConfig();
		lockConfig6.LockType = "Zen_CombinationLock6";
		lockConfig6.TimeRequired = 120; // 2 minute
		lockConfig6.DamageTool = 700;
		ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig6);

		// Zen 7-dial
		ZenLockConfig lockConfig7 = new ZenLockConfig();
		lockConfig7.LockType = "Zen_CombinationLock7";
		lockConfig7.TimeRequired = 150; // 2.5 minute
		lockConfig7.DamageTool = 800;
		ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig7);

		// Zen 8-dial
		ZenLockConfig lockConfig8 = new ZenLockConfig();
		lockConfig8.LockType = "Zen_CombinationLock8";
		lockConfig8.TimeRequired = 180; // 3 minutes
		lockConfig8.DamageTool = 900;
		ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig8);

		// Zen 9-dial
		ZenLockConfig lockConfig9 = new ZenLockConfig();
		lockConfig9.LockType = "Zen_CombinationLock9";
		lockConfig9.TimeRequired = 210; // 3.5 minutes
		lockConfig9.DamageTool = 1000;
		ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig9);
	}

	// Get raid config for given combo lock item type
	ZenLockConfig GetRaidConfigForType(string type)
	{
		string toFind = type;
		toFind.ToLower();

		foreach (ZenLockConfig config : ClientSyncConfig.RaidConfig.LockConfig)
		{
			string lockType = config.LockType;
			lockType.ToLower();

			if (lockType == toFind)
				return config;
		}

		return NULL;
	}
}

// Config sync'd to client
class ZenComboLocksClientConfig
{
	bool AllowAdminOpen = true;
	bool InteractAnywhere = true;
	bool InstantOpen = false;
	bool DismantleWithLock = false;
	int DisplayDigits = 2; // 0 = none, 1 = single digit, 2 = entire code
	float DialTime = 0.5;
	bool OwnerCanRemoveLockWithoutCode = false;
	ref ZenComboLocksRaidConfig RaidConfig;
}

// Server-side only config
class ZenComboLocksServerConfig
{
	bool PrintLogs = true;
	bool UnlockOnOpen = false;
	int DigitMultiplier = 1;
}

// Raid config
class ZenComboLocksRaidConfig
{
	bool Enabled = true;
	ref array<ref ZenLockConfig> LockConfig;
}

// Lock config
class ZenLockConfig
{
	string LockType;
	int TimeRequired;
	int DamageTool; // BoltCutter Max HP = 1000
}