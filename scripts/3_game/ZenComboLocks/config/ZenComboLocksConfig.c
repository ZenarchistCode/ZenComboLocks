ref ZenComboLocksConfig g_ZenComboLocksConfig;

static bool ZenComboLocks_IsEnabled()
{
	#ifdef ZenModPack
	return ZenModEnabled("ZenComboLocks");
	#endif

	return true;
}

static ZenComboLocksConfig GetZenComboLocksConfig()
{
	if (!g_ZenComboLocksConfig)
	{
		GetZenConfigRegister().RegisterConfig(ZenComboLocksConfig);
	}

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
	ref ZenComboLocksServerConfig ServerConfig;
	ref ZenComboLocksClientConfig ClientSyncConfig;

	override void OnRegistered()
	{
		g_ZenComboLocksConfig = this;
	}

	override string GetCurrentVersion()
	{
		return "1.30.1";
	}

	override bool ShouldLoadOnServer()
	{
		return ZenComboLocks_IsEnabled();
	}

	override bool ShouldSyncToClient()
	{
		return ZenComboLocks_IsEnabled();
	}

	override bool ReadJson(string path, out string err)
	{
		bool loadedConfig = JsonFileLoader<ZenComboLocksConfig>.LoadFile(path, this, err);
		if (loadedConfig)
		{
			EnsureContainers();
		}

		return loadedConfig;
	}

	override bool WriteJson(string path, out string err)
	{
		return JsonFileLoader<ZenComboLocksConfig>.SaveFile(path, this, err);
	}

	override bool BuildSyncPayload(out string payload, out string err)
	{
		EnsureContainers();

		ZenComboLocksConfig_SyncPayload syncSnapshot = new ZenComboLocksConfig_SyncPayload();
		syncSnapshot.ClientSyncConfig = ClientSyncConfig;

		return JsonFileLoader<ZenComboLocksConfig_SyncPayload>.MakeData(syncSnapshot, payload, err, false);
	}

	override bool ApplySyncPayload(string payload, out string err)
	{
		ClientSyncConfig = new ZenComboLocksClientConfig();

		ZenComboLocksConfig_SyncPayload syncSnapshot = new ZenComboLocksConfig_SyncPayload();
		if (!JsonFileLoader<ZenComboLocksConfig_SyncPayload>.LoadData(payload, syncSnapshot, err))
		{
			return false;
		}

		ClientSyncConfig = syncSnapshot.ClientSyncConfig;
		EnsureContainers();
		return true;
	}

	override void SetDefaults()
	{
		ServerConfig = new ZenComboLocksServerConfig();
		ClientSyncConfig = new ZenComboLocksClientConfig();
		ClientSyncConfig.RaidConfig = new ZenComboLocksRaidConfig();
		ClientSyncConfig.RaidConfig.LockConfig = new array<ref ZenLockConfig>;

		AddDefaultRaidConfig("CombinationLock", 30, 250);
		AddDefaultRaidConfig("CombinationLock4", 60, 500);

		// Hidden 1.29 migration aliases. They all behave as four-dial locks in 1.30.
		AddDefaultRaidConfig("Zen_CombinationLock4", 60, 500);
		AddDefaultRaidConfig("Zen_CombinationLock5", 60, 500);
		AddDefaultRaidConfig("Zen_CombinationLock6", 60, 500);
		AddDefaultRaidConfig("Zen_CombinationLock7", 60, 500);
		AddDefaultRaidConfig("Zen_CombinationLock8", 60, 500);
		AddDefaultRaidConfig("Zen_CombinationLock9", 60, 500);
	}

	protected void EnsureContainers()
	{
		if (!ServerConfig)
		{
			ServerConfig = new ZenComboLocksServerConfig();
		}

		if (!ClientSyncConfig)
		{
			ClientSyncConfig = new ZenComboLocksClientConfig();
		}

		if (!ClientSyncConfig.RaidConfig)
		{
			ClientSyncConfig.RaidConfig = new ZenComboLocksRaidConfig();
		}

		if (!ClientSyncConfig.RaidConfig.LockConfig)
		{
			ClientSyncConfig.RaidConfig.LockConfig = new array<ref ZenLockConfig>;
		}
	}

	protected void AddDefaultRaidConfig(string lockType, int timeRequired, int toolDamage)
	{
		ZenLockConfig newLockConfig = new ZenLockConfig();
		newLockConfig.LockType = lockType;
		newLockConfig.TimeRequired = timeRequired;
		newLockConfig.DamageTool = toolDamage;
		ClientSyncConfig.RaidConfig.LockConfig.Insert(newLockConfig);
	}

	ZenLockConfig GetRaidConfigForType(string type)
	{
		EnsureContainers();

		string requestedType = type;
		requestedType.ToLower();

		foreach (ZenLockConfig raidLockConfig : ClientSyncConfig.RaidConfig.LockConfig)
		{
			if (!raidLockConfig)
			{
				continue;
			}

			string configuredType = raidLockConfig.LockType;
			configuredType.ToLower();

			if (configuredType == requestedType)
			{
				return raidLockConfig;
			}
		}

		return NULL;
	}
}

class ZenComboLocksClientConfig
{
	bool AllowAdminOpen = true;
	bool InteractAnywhere = true;
	bool InstantOpen = false;
	float DialTime = 0.5;
	bool OwnerCanRemoveLockWithoutCode = false;
	ref ZenComboLocksRaidConfig RaidConfig;
}

class ZenComboLocksServerConfig
{
	bool PrintLogs = true;
	int DigitMultiplier = 1;
}

class ZenComboLocksRaidConfig
{
	bool Enabled = true;
	ref array<ref ZenLockConfig> LockConfig;
}

class ZenLockConfig
{
	string LockType;
	int TimeRequired;
	int DamageTool;
}
