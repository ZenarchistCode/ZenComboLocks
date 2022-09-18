class CfgPatches
{
    class ZenComboLocks
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]=
        {
            "DZ_Data",
            "DZ_Scripts"
        };
		units[] =
		{
			"Zen_BoltCutter",
			"Zen_CombinationLock5",
			"Zen_CombinationLock6",
			"Zen_CombinationLock7",
			"Zen_CombinationLock8",
			"Zen_CombinationLock9"
		};
    };
};

class CfgMods
{
    class ZenComboLocks
    {
        type="mod";
        author="";
        dir="ZenComboLocks";
        name="ZenComboLocks";
        dependencies[]=
        {
            "Game",
            "World",
            "Mission"
        };
        class defs
        {
            class gameScriptModule
            {
                value="";
                files[]=
                {
                    "ZenComboLocks/Scripts/3_Game"
                };
            };
            class worldScriptModule
            {
                value="";
                files[]=
                {
                    "ZenComboLocks/Scripts/4_World"
                };
            };
            class missionScriptModule
            {
                value="";
                files[]=
                {
                    "ZenComboLocks/Scripts/5_Mission"
                };
            };
        };
    };
};

class CfgVehicles
{
	// Define extra combo locks with more digits than vanilla
	class CombinationLock;
	class Zen_CombinationLockBase : CombinationLock
	{
		scope = 2;
		displayName = "$STR_ZenComboLockName"; // This gets overridden in Zen_CombinationLockBase.c
		descriptionShort = "$STR_ZenComboLockDesc"; // This gets overridden in Zen_CombinationLockBase.c
		model = "\DZ\gear\camping\combination_lock4.p3d";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1500;
					healthLevels[] =
					{

						{
							1,

							{
								"DZ\gear\camping\data\combilock4_metal.rvmat",
								"DZ\gear\camping\data\combination_lock_chain.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_rubber.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_rubber1.rvmat"
							}
						},

						{
							0.69999999,

							{
								"DZ\gear\camping\data\combilock4_metal.rvmat",
								"DZ\gear\camping\data\combination_lock_chain.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_rubber.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_rubber1.rvmat"
							}
						},

						{
							0.5,

							{
								"DZ\gear\camping\data\combilock4_metal_damage.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_damage.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_rubber_damage.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_rubber1_damage.rvmat"
							}
						},

						{
							0.30000001,

							{
								"DZ\gear\camping\data\combilock4_metal_damage.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_damage.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_rubber_damage.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_rubber1_damage.rvmat"
							}
						},

						{
							0,

							{
								"DZ\gear\camping\data\combilock4_metal_destruct.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_destruct.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_rubber_destruct.rvmat",
								"DZ\gear\camping\data\combination_lock_chain_rubber1_destruct.rvmat"
							}
						}
					};
				};
			};
			// To make combo locks destructable from ballistics/explosives, modify these Health values
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage = 0;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage = 0;
					};
					class Blood
					{
						damage = 0;
					};
					class Shock
					{
						damage = 0;
					};
				};
			};
		};
	};
	class Zen_CombinationLock5 : Zen_CombinationLockBase
	{
		scope = 2;
		lockDigits = 5;
	};
	class Zen_CombinationLock6 : Zen_CombinationLockBase
	{
		scope = 2;
		lockDigits = 6;
	};
	class Zen_CombinationLock7 : Zen_CombinationLockBase
	{
		scope = 2;
		lockDigits = 7;
	};
	class Zen_CombinationLock8 : Zen_CombinationLockBase
	{
		scope = 2;
		lockDigits = 8;
	};
	class Zen_CombinationLock9 : Zen_CombinationLockBase
	{
		scope = 2;
		lockDigits = 9;
	};

	// Define bolt 
	class Inventory_Base;
	class Zen_BoltCutter : Inventory_Base
	{
		scope = 2;
		displayName = "$STR_ZenBoltCutterName";
		descriptionShort = "$STR_ZenBoltCutterDesc";
		model = "\ZenComboLocks\data\model\zen_boltcutter.p3d";
		rotationFlags = 1;
		weight = 4400;
		itemSize[] = { 2,6 };
		lootCategory = "Tools";
		lootTag[] =
		{
			"Work"
		};
		inventorySlot[] =
		{
			//"Shoulder",
			//"Melee"
		};
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

						{
							1,

							{
								"ZenComboLocks\data\model\zen_boltcutters.rvmat"
							}
						},

						{
							0.69999999,

							{
								"ZenComboLocks\data\model\zen_boltcutters.rvmat"
							}
						},

						{
							0.5,

							{
								"ZenComboLocks\data\model\zen_boltcutters_damage.rvmat"
							}
						},

						{
							0.30000001,

							{
								"ZenComboLocks\data\model\zen_boltcutters_damage.rvmat"
							}
						},

						{
							0,

							{
								"ZenComboLocks\data\model\zen_boltcutters_destruct.rvmat"
							}
						}
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
				range = 4.0999999;
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
	class Zen_BoltCutter_SoundSet { soundShaders[] = { "Zen_BoltCutter_SoundSet" }; volumeFactor = 1.0; };
}

class CfgSoundShaders
{
	class Zen_BoltCutter_SoundShader_Base
	{
		samples[] = {};
		frequency = 1;
		range = 30;
		volume = 1;
	};

	class Zen_BoltCutter_SoundSet : Zen_BoltCutter_SoundShader_Base 
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
	};
}