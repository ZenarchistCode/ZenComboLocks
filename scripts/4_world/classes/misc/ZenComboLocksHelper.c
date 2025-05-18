// Contains a bunch of functions I repeatedly use across this entire mod to keep things in one place and easier to debug
class ZenComboLocksHelper
{
	// Check if the given object is a door and it's open
	static bool IsOpen(Object doorGateThingy)
	{
		if (!doorGateThingy)
			return false;

		// Check vanilla
		Fence fence = Fence.Cast(doorGateThingy);
		if (fence)
		{
			return fence.IsOpened();
		}

		// Check BF
		#ifdef BuildingFort_Mod_1A
		BF_DoorBarricade bf_door = BF_DoorBarricade.Cast(doorGateThingy);
		if (bf_door)
		{
			return bf_door.IsOpened();
		}
		#endif

		// Check cabin
		#ifdef Cabin_Mod_RaGed
		Prefab_Cabin cabin = Prefab_Cabin.Cast(doorGateThingy);
		if (cabin)
		{
			return cabin.IsOpened();
		}
		#endif

		return false;
	}

	// Open the given door object
	static void Open(Object doorGateThingy)
	{
		if (!doorGateThingy)
			return;

		// Check vanilla
		Fence fence = Fence.Cast(doorGateThingy);
		if (fence)
		{
			fence.OpenFence();
			return;
		}

		// Check BF
		#ifdef BuildingFort_Mod_1A
		BuildingFortficationsCore bf_door = BuildingFortficationsCore.Cast(doorGateThingy);
		if (bf_door)
		{
			bf_door.OpenFence();
			return;
		}
		#endif

		// Check cabin
		#ifdef Cabin_Mod_RaGed
		Prefab_Cabin cabin = Prefab_Cabin.Cast(doorGateThingy);
		if (cabin)
		{
			cabin.OpenCabinFence();
			return;
		}
		#endif
	}

	// Gets the combination lock from the given door object
	static CombinationLock GetCombinationLock(Object doorGateThingy)
	{
		if (!doorGateThingy)
			return NULL;

		// Check vanilla
		Fence fence = Fence.Cast(doorGateThingy);
		if (fence)
		{
			return fence.GetCombinationLock();
		}

		// Check BF
		#ifdef BuildingFort_Mod_1A
		BF_DoorBarricade bf_door = BF_DoorBarricade.Cast(doorGateThingy);
		if (bf_door)
		{
			return bf_door.GetCombinationLock();
		}
		#endif

		#ifdef Cabin_Mod_RaGed
		// Check cabin
		Prefab_Cabin cabin = Prefab_Cabin.Cast(doorGateThingy);
		if (cabin)
		{
			return cabin.GetCombinationLock();
		}
		#endif

		EntityAI entityObject = EntityAI.Cast(doorGateThingy);
		if (!entityObject)
			return NULL;

		return CombinationLock.Cast(entityObject.FindAttachmentBySlotName("Att_CombinationLock"));
	}
}