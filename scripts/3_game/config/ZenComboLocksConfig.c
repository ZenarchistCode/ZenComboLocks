class ZenComboLocksConfig
{
	// Static constant config version (is NOT saved to json)
	static const string CONFIG_VERSION = "2";

	// Config location
	private const static string zenModFolder = "$profile:\\Zenarchist\\";
	private const static string zenConfigName = "ZenComboLocksConfig.json";

	// Main config data
	string ConfigVersion = "";
	ref ZenComboLocksServerConfig ServerConfig; // Config that is server-only
	ref ZenComboLocksClientConfig ClientSyncConfig; // Config that sync's to client

	// Load config file or create default file if config doesn't exsit
	void Load()
	{
		if (GetGame().IsDedicatedServer())
		{
			if (FileExist(zenModFolder + zenConfigName))
			{ 
				// If config exists, load file
				JsonFileLoader<ZenComboLocksConfig>.JsonLoadFile(zenModFolder + zenConfigName, this);

				// If version mismatch, backup old version of json before replacing it
				if (ConfigVersion != CONFIG_VERSION)
				{
					JsonFileLoader<ZenComboLocksConfig>.JsonSaveFile(zenModFolder + zenConfigName + "_old", this);
				}
				else
				{
					// Config exists and version matches, stop here.
					return;
				}
			}

			// Set new config version
			ConfigVersion = CONFIG_VERSION;

			// Set default config
			ServerConfig = new ZenComboLocksServerConfig;
			ClientSyncConfig = new ZenComboLocksClientConfig;
			ServerConfig.AdminSteamIDs = new array<string>;
			ServerConfig.AdminSteamIDs.Clear();
			ServerConfig.AdminSteamIDs.Insert("www.steamidfinder.com");
			ClientSyncConfig.RaidConfig = new ZenComboLocksRaidConfig;
			ClientSyncConfig.RaidConfig.LockConfig = new array<ref ZenLockConfig>;

			// Vanilla 3-dial
			ZenLockConfig lockConfig3 = new ZenLockConfig;
			lockConfig3.LockType = "CombinationLock";
			lockConfig3.TimeRequired = 30; // 30 seconds
			lockConfig3.DamageTool = 250; // Durability: 4 locks
			ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig3);

			// Vanilla 4-dial
			ZenLockConfig lockConfig4 = new ZenLockConfig;
			lockConfig4.LockType = "CombinationLock4";
			lockConfig4.TimeRequired = 60; // 1 minute
			lockConfig4.DamageTool = 500; // Durability: 2 locks
			ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig4);

			// Vanilla 4-dial
			ZenLockConfig lockConfig4Zen = new ZenLockConfig;
			lockConfig4Zen.LockType = "Zen_CombinationLock4";
			lockConfig4Zen.TimeRequired = 60; // 1 minute
			lockConfig4Zen.DamageTool = 500; // Durability: 2 locks
			ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig4Zen);

			// Zen 5-dial
			ZenLockConfig lockConfig5 = new ZenLockConfig;
			lockConfig5.LockType = "Zen_CombinationLock5";
			lockConfig5.TimeRequired = 90; // 1.5 minute
			lockConfig5.DamageTool = 600;
			ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig5);

			// Zen 6-dial
			ZenLockConfig lockConfig6 = new ZenLockConfig;
			lockConfig6.LockType = "Zen_CombinationLock6";
			lockConfig6.TimeRequired = 120; // 2 minute
			lockConfig6.DamageTool = 700;
			ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig6);

			// Zen 7-dial
			ZenLockConfig lockConfig7 = new ZenLockConfig;
			lockConfig7.LockType = "Zen_CombinationLock7";
			lockConfig7.TimeRequired = 150; // 2.5 minute
			lockConfig7.DamageTool = 800;
			ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig7);

			// Zen 8-dial
			ZenLockConfig lockConfig8 = new ZenLockConfig;
			lockConfig8.LockType = "Zen_CombinationLock8";
			lockConfig8.TimeRequired = 180; // 3 minutes
			lockConfig8.DamageTool = 900;
			ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig8);

			// Zen 9-dial
			ZenLockConfig lockConfig9 = new ZenLockConfig;
			lockConfig9.LockType = "Zen_CombinationLock9";
			lockConfig9.TimeRequired = 210; // 3.5 minutes
			lockConfig9.DamageTool = 1000;
			ClientSyncConfig.RaidConfig.LockConfig.Insert(lockConfig9);

			// Save config
			Save();
		}
	}

	// Save config
	void Save()
	{
		if (!FileExist(zenModFolder))
		{	
			// If config folder doesn't exist, create it.
			MakeDirectory(zenModFolder);
		}

		// Save JSON config
		JsonFileLoader<ZenComboLocksConfig>.JsonSaveFile(zenModFolder + zenConfigName, this);
	}

	// Check if given player ID is an admin
	bool IsAdminZCBL(string playerID)
	{
		if (!ClientSyncConfig.AllowAdminOpen)
			return false;

		foreach (string s : ServerConfig.AdminSteamIDs)
		{
			if (s == playerID)
			{
				return true;
			}
		}

		return false;
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
	bool AllowAdminOpen = false;
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
	ref array<string> AdminSteamIDs;
}

// Raid config
class ZenComboLocksRaidConfig
{
	bool Enabled = false; // Disabled by default
	ref array<ref ZenLockConfig> LockConfig;
}

// Lock config
class ZenLockConfig
{
	string LockType;
	int TimeRequired;
	int DamageTool; // BoltCutter Max HP = 1000
}

// Save config data
ref ZenComboLocksConfig m_ZenComboLocksConfig;

// Helper function to return Config data storage object
static ZenComboLocksConfig GetZenComboLocksConfig()
{
	if (!m_ZenComboLocksConfig)
	{
		Print("[ZenComboLocksConfig] Init");
		m_ZenComboLocksConfig = new ZenComboLocksConfig;

		// Only load JSON config on the server
		if (GetGame().IsDedicatedServer())
		{
			m_ZenComboLocksConfig.Load();
		}
	}

	return m_ZenComboLocksConfig;
}