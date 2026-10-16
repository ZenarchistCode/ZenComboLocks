class CfgPatches
{
	class ZenComboLocks
	{
		units[] =
		{
			"Zen_BoltCutter"
		};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data",
			"DZ_Scripts",
			"ZenModCore"
		};
	};
};

class CfgMods
{
	class ZenComboLocks
	{
		author = "Zenarchist";
		type = "mod";
		storageVersion = 1;
		dependencies[] =
		{
			"Game",
			"World",
			"Mission"
		};

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] =
				{
					"ZenComboLocks/Scripts/3_Game"
				};
			};

			class worldScriptModule
			{
				value = "";
				files[] =
				{
					"ZenComboLocks/Scripts/4_World"
				};
			};

			class missionScriptModule
			{
				value = "";
				files[] =
				{
					"ZenComboLocks/Scripts/5_Mission"
				};
			};
		};
	};
};

class CfgVehicles
{
	// Hidden migration aliases. Existing 1.29 Zen locks remain loadable, but all
	// now use the native four-dial 1.30 implementation and are no longer spawnable.
	class CombinationLock4;
	class Zen_CombinationLockBase : CombinationLock4
	{
		scope = 1;
		scopeCurator = 0;
	};

	class Zen_CombinationLock4 : Zen_CombinationLockBase {};
	class Zen_CombinationLock5 : Zen_CombinationLockBase {};
	class Zen_CombinationLock6 : Zen_CombinationLockBase {};
	class Zen_CombinationLock7 : Zen_CombinationLockBase {};
	class Zen_CombinationLock8 : Zen_CombinationLockBase {};
	class Zen_CombinationLock9 : Zen_CombinationLockBase {};

	class Inventory_Base;
	class Zen_BoltCutter : Inventory_Base
	{
		scope = 2;
		displayName = "$STR_ZenBoltCutterName";
		descriptionShort = "$STR_ZenBoltCutterDesc";
		model = "\ZenComboLocks\data\model\zen_boltcutter.p3d";
		rotationFlags = 4;
		weight = 4400;
		itemSize[] = { 2, 6 };
		lootCategory = "Tools";
		lootTag[] =
		{
			"Work"
		};
		inventorySlot[] = {};
		suicideAnim = "woodaxe";
		isMeleeWeapon = 1;

		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1000;
					healthLevels[] =
					{
						{ 1, { "ZenComboLocks\data\model\zen_boltcutters.rvmat" } },
						{ 0.7, { "ZenComboLocks\data\model\zen_boltcutters.rvmat" } },
						{ 0.5, { "ZenComboLocks\data\model\zen_boltcutters_damage.rvmat" } },
						{ 0.3, { "ZenComboLocks\data\model\zen_boltcutters_damage.rvmat" } },
						{ 0, { "ZenComboLocks\data\model\zen_boltcutters_destruct.rvmat" } }
					};
				};
			};
		};

		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeCrowbar";
				range = 1.8;
			};

			class Heavy
			{
				ammo = "MeleeCrowbar_Heavy";
				range = 1.8;
			};

			class Sprint
			{
				ammo = "MeleeCrowbar_Heavy";
				range = 4.1;
			};
		};

		class AnimEvents
		{
			class SoundWeapon
			{
				class drop
				{
					soundset = "crowbar_drop_SoundSet";
					id = 898;
				};

				class Crowbar_loop
				{
					soundSet = "Crowbar_loop_SoundSet";
					id = 1119;
				};

				class Crowbar_end
				{
					soundSet = "Crowbar_end_SoundSet";
					id = 1120;
				};

				class animalSkinning_in
				{
					soundSet = "animalSkinning_in_SoundSet";
					id = 516;
				};

				class animalSkinning
				{
					soundSet = "animalSkinning_SoundSet";
					id = 517;
				};

				class animalSkinning_out
				{
					soundSet = "animalSkinning_out_SoundSet";
					id = 518;
				};
			};
		};
	};
};

class CfgSoundSets
{
	class Zen_BoltCutter_SoundSet
	{
		soundShaders[] =
		{
			"Zen_BoltCutter_SoundShader"
		};
		volumeFactor = 1.0;
	};
};

class CfgSoundShaders
{
	class Zen_BoltCutter_SoundShader
	{
		samples[] =
		{
			{ "ZenComboLocks\data\sounds\zen_boltcutter_cut1", 1 },
			{ "ZenComboLocks\data\sounds\zen_boltcutter_cut2", 1 },
			{ "ZenComboLocks\data\sounds\zen_boltcutter_cut3", 1 },
			{ "ZenComboLocks\data\sounds\zen_boltcutter_cut4", 1 },
			{ "ZenComboLocks\data\sounds\zen_boltcutter_cut5", 1 },
			{ "ZenComboLocks\data\sounds\zen_boltcutter_cut6", 1 }
		};
		frequency = 1;
		range = 30;
		volume = 1;
	};
};
