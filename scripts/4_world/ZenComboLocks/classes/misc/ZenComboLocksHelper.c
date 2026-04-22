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
		#ifdef BuildingFortifications
		BF_DoorBarricade bfDoor = BF_DoorBarricade.Cast(doorGateThingy);
		if (bfDoor)
		{
			return bfDoor.IsOpened();
		}
		
		BF_DoubleDoorBarricade bfdDoor = BF_DoubleDoorBarricade.Cast(doorGateThingy);
		if (bfdDoor)
		{
			return bfdDoor.IsOpened();
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
		#ifdef BuildingFortifications
		BF_DoorBarricade bfDoor = BF_DoorBarricade.Cast(doorGateThingy);
		if (bfDoor)
		{
			bfDoor.OpenFence();
			return;
		}
		
		BF_DoubleDoorBarricade bfdDoor = BF_DoubleDoorBarricade.Cast(doorGateThingy);
		if (bfdDoor)
		{
			bfdDoor.OpenFence();
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
		#ifdef BuildingFortifications
		BF_DoorBarricade bfDoor = BF_DoorBarricade.Cast(doorGateThingy);
		if (bfDoor)
		{
			bfDoor.GetCombinationLock();
		}
		
		BF_DoubleDoorBarricade bfdDoor = BF_DoubleDoorBarricade.Cast(doorGateThingy);
		if (bfdDoor)
		{
			bfdDoor.GetCombinationLock();
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