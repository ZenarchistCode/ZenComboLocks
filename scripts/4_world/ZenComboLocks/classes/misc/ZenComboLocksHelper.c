class ZenComboLocksHelper
{
	// 1.29 helper API retained for dependent mods.
	static bool IsOpen(Object targetObject)
	{
		if (!targetObject)
		{
			return false;
		}

		Fence targetFence = Fence.Cast(targetObject);
		if (targetFence)
		{
			return targetFence.IsOpened();
		}

		#ifdef BuildingFortifications
		BF_DoorBarricade targetBFDoor = BF_DoorBarricade.Cast(targetObject);
		if (targetBFDoor)
		{
			return targetBFDoor.IsOpened();
		}

		BF_DoubleDoorBarricade targetBFDoubleDoor = BF_DoubleDoorBarricade.Cast(targetObject);
		if (targetBFDoubleDoor)
		{
			return targetBFDoubleDoor.IsOpened();
		}
		#endif

		#ifdef Cabin_Mod_RaGed
		Prefab_Cabin targetCabin = Prefab_Cabin.Cast(targetObject);
		if (targetCabin)
		{
			return targetCabin.IsOpened();
		}
		#endif

		return false;
	}

	static void Open(Object targetObject)
	{
		if (!targetObject)
		{
			return;
		}

		Fence targetFence = Fence.Cast(targetObject);
		if (targetFence)
		{
			targetFence.OpenFence();
			return;
		}

		#ifdef BuildingFortifications
		BF_DoorBarricade targetBFDoor = BF_DoorBarricade.Cast(targetObject);
		if (targetBFDoor)
		{
			targetBFDoor.OpenFence();
			return;
		}

		BF_DoubleDoorBarricade targetBFDoubleDoor = BF_DoubleDoorBarricade.Cast(targetObject);
		if (targetBFDoubleDoor)
		{
			targetBFDoubleDoor.OpenFence();
			return;
		}
		#endif

		#ifdef Cabin_Mod_RaGed
		Prefab_Cabin targetCabin = Prefab_Cabin.Cast(targetObject);
		if (targetCabin)
		{
			targetCabin.OpenCabinFence();
		}
		#endif
	}

	static CombinationLock ResolveActionLock(ActionTarget actionTarget)
	{
		if (!actionTarget || !actionTarget.GetObject())
		{
			return NULL;
		}

		return GetCombinationLock(actionTarget.GetObject(), actionTarget.GetComponentIndex());
	}

	static DigitalCodeLock ResolveDigitalActionLock(ActionTarget actionTarget)
	{
		if (!actionTarget || !actionTarget.GetObject())
		{
			return NULL;
		}

		return GetDigitalCodeLock(actionTarget.GetObject(), actionTarget.GetComponentIndex());
	}

	static DigitalCodeLock GetDigitalCodeLock(Object targetObject, int componentIndex = -1)
	{
		if (!targetObject)
		{
			return NULL;
		}

		DigitalCodeLock directDigitalLock = DigitalCodeLock.Cast(targetObject);
		if (directDigitalLock)
		{
			return directDigitalLock;
		}

		Fence digitalFenceTarget = Fence.Cast(targetObject);
		if (digitalFenceTarget)
		{
			return digitalFenceTarget.GetCodeLock();
		}

		BuildingBase digitalBuildingTarget = BuildingBase.Cast(targetObject);
		if (digitalBuildingTarget)
		{
			if (componentIndex >= 0)
			{
				int digitalTargetDoorIndex = digitalBuildingTarget.GetDoorIndex(componentIndex);
				if (digitalTargetDoorIndex >= 0)
				{
					return GetBuildingDoorDigitalCodeLock(digitalBuildingTarget, digitalTargetDoorIndex);
				}

				return NULL;
			}

			return GetFirstBuildingDigitalCodeLock(digitalBuildingTarget);
		}

		EntityAI genericDigitalTarget = EntityAI.Cast(targetObject);
		if (genericDigitalTarget)
		{
			return DigitalCodeLock.Cast(genericDigitalTarget.FindAttachmentBySlotName("Att_CodeLock"));
		}

		return NULL;
	}

	static DigitalCodeLock GetBuildingDoorDigitalCodeLock(BuildingBase buildingTarget, int doorIndex)
	{
		if (!buildingTarget || doorIndex < 0)
		{
			return NULL;
		}

		AdditionalDoorInfo digitalDoorInfo = buildingTarget.GetDoorInfo(doorIndex);
		if (!digitalDoorInfo || !digitalDoorInfo.m_RelatedInventorySlotNames)
		{
			return NULL;
		}

		foreach (string digitalSlotName : digitalDoorInfo.m_RelatedInventorySlotNames)
		{
			DigitalCodeLock digitalDoorLock = DigitalCodeLock.Cast(buildingTarget.FindAttachmentBySlotName(digitalSlotName));
			if (digitalDoorLock)
			{
				return digitalDoorLock;
			}
		}

		return NULL;
	}

	static DigitalCodeLock GetFirstBuildingDigitalCodeLock(BuildingBase buildingTarget)
	{
		if (!buildingTarget)
		{
			return NULL;
		}

		int digitalBuildingDoorCount = buildingTarget.GetDoorCount();
		for (int digitalBuildingDoorIndex = 0; digitalBuildingDoorIndex < digitalBuildingDoorCount; digitalBuildingDoorIndex++)
		{
			DigitalCodeLock firstDigitalDoorLock = GetBuildingDoorDigitalCodeLock(buildingTarget, digitalBuildingDoorIndex);
			if (firstDigitalDoorLock)
			{
				return firstDigitalDoorLock;
			}
		}

		return NULL;
	}

	static CombinationLock GetCombinationLock(Object targetObject, int componentIndex = -1)
	{
		if (!targetObject)
		{
			return NULL;
		}

		CombinationLock directLock = CombinationLock.Cast(targetObject);
		if (directLock)
		{
			return directLock;
		}

		Fence fenceTarget = Fence.Cast(targetObject);
		if (fenceTarget)
		{
			return fenceTarget.GetCombinationLock();
		}

		#ifdef BuildingFortifications
		BF_DoorBarricade bfDoorTarget = BF_DoorBarricade.Cast(targetObject);
		if (bfDoorTarget)
		{
			return bfDoorTarget.GetCombinationLock();
		}

		BF_DoubleDoorBarricade bfDoubleDoorTarget = BF_DoubleDoorBarricade.Cast(targetObject);
		if (bfDoubleDoorTarget)
		{
			return bfDoubleDoorTarget.GetCombinationLock();
		}
		#endif

		#ifdef Cabin_Mod_RaGed
		Prefab_Cabin cabinTarget = Prefab_Cabin.Cast(targetObject);
		if (cabinTarget)
		{
			return cabinTarget.GetCombinationLock();
		}
		#endif

		BuildingBase buildingTarget = BuildingBase.Cast(targetObject);
		if (buildingTarget)
		{
			if (componentIndex >= 0)
			{
				int targetDoorIndex = buildingTarget.GetDoorIndex(componentIndex);
				if (targetDoorIndex >= 0)
				{
					return GetBuildingDoorCombinationLock(buildingTarget, targetDoorIndex);
				}

				return NULL;
			}

			return GetFirstBuildingCombinationLock(buildingTarget);
		}

		EntityAI genericTarget = EntityAI.Cast(targetObject);
		if (genericTarget)
		{
			return CombinationLock.Cast(genericTarget.FindAttachmentBySlotName("Att_CombinationLock"));
		}

		return NULL;
	}

	static CombinationLock GetBuildingDoorCombinationLock(BuildingBase buildingTarget, int doorIndex)
	{
		if (!buildingTarget || doorIndex < 0)
		{
			return NULL;
		}

		AdditionalDoorInfo targetDoorInfo = buildingTarget.GetDoorInfo(doorIndex);
		if (!targetDoorInfo || !targetDoorInfo.m_RelatedInventorySlotNames)
		{
			return NULL;
		}

		foreach (string targetSlotName : targetDoorInfo.m_RelatedInventorySlotNames)
		{
			CombinationLock targetDoorLock = CombinationLock.Cast(buildingTarget.FindAttachmentBySlotName(targetSlotName));
			if (targetDoorLock)
			{
				return targetDoorLock;
			}
		}

		return NULL;
	}

	static CombinationLock GetFirstBuildingCombinationLock(BuildingBase buildingTarget)
	{
		if (!buildingTarget)
		{
			return NULL;
		}

		int buildingDoorCount = buildingTarget.GetDoorCount();
		for (int buildingDoorIndex = 0; buildingDoorIndex < buildingDoorCount; buildingDoorIndex++)
		{
			CombinationLock buildingDoorLock = GetBuildingDoorCombinationLock(buildingTarget, buildingDoorIndex);
			if (buildingDoorLock)
			{
				return buildingDoorLock;
			}
		}

		return NULL;
	}

	static int FindBuildingDoorIndexForLock(BuildingBase buildingTarget, CombinationLock targetLock)
	{
		if (!buildingTarget || !targetLock)
		{
			return -1;
		}

		int searchDoorCount = buildingTarget.GetDoorCount();
		for (int searchDoorIndex = 0; searchDoorIndex < searchDoorCount; searchDoorIndex++)
		{
			CombinationLock searchDoorLock = GetBuildingDoorCombinationLock(buildingTarget, searchDoorIndex);
			if (searchDoorLock == targetLock)
			{
				return searchDoorIndex;
			}
		}

		return -1;
	}

	static bool IsDirectLockTarget(ActionTarget actionTarget)
	{
		if (!actionTarget)
		{
			return false;
		}

		return CombinationLock.Cast(actionTarget.GetObject()) != NULL;
	}

	static bool IsDirectDigitalLockTarget(ActionTarget actionTarget)
	{
		if (!actionTarget)
		{
			return false;
		}

		return DigitalCodeLock.Cast(actionTarget.GetObject()) != NULL;
	}

	static bool IsOpenForDigitalLock(DigitalCodeLock targetLock)
	{
		if (!targetLock)
		{
			return false;
		}

		EntityAI digitalLockParent = targetLock.GetHierarchyParent();
		if (!digitalLockParent)
		{
			return false;
		}

		Fence digitalFenceParent = Fence.Cast(digitalLockParent);
		if (digitalFenceParent)
		{
			return digitalFenceParent.IsOpened();
		}

		BuildingBase digitalBuildingParent = BuildingBase.Cast(digitalLockParent);
		if (digitalBuildingParent)
		{
			int digitalLockDoorIndex = targetLock.GetCodeLockComponent().GetDoorIndex();
			if (digitalLockDoorIndex >= 0)
			{
				return digitalBuildingParent.IsDoorOpen(digitalLockDoorIndex);
			}
		}

		return false;
	}

	static bool OpenForDigitalLock(DigitalCodeLock targetLock)
	{
		if (!targetLock)
		{
			return false;
		}

		EntityAI digitalOpenParent = targetLock.GetHierarchyParent();
		if (!digitalOpenParent)
		{
			return false;
		}

		Fence digitalOpenFence = Fence.Cast(digitalOpenParent);
		if (digitalOpenFence)
		{
			if (!digitalOpenFence.IsOpened())
			{
				digitalOpenFence.OpenFence();
			}

			return true;
		}

		BuildingBase digitalOpenBuilding = BuildingBase.Cast(digitalOpenParent);
		if (digitalOpenBuilding)
		{
			int digitalOpenDoorIndex = targetLock.GetCodeLockComponent().GetDoorIndex();
			if (digitalOpenDoorIndex >= 0)
			{
				if (!digitalOpenBuilding.IsDoorOpen(digitalOpenDoorIndex))
				{
					digitalOpenBuilding.OpenDoor(digitalOpenDoorIndex);
				}

				return true;
			}
		}

		return false;
	}

	static bool OpenForDigitalActionTarget(DigitalCodeLock targetLock, ActionTarget actionTarget)
	{
		if (!targetLock || !actionTarget || !actionTarget.GetObject())
		{
			return false;
		}

		BuildingBase digitalActionBuilding = BuildingBase.Cast(actionTarget.GetObject());
		if (digitalActionBuilding)
		{
			int digitalActionDoorIndex = digitalActionBuilding.GetDoorIndex(actionTarget.GetComponentIndex());
			if (digitalActionDoorIndex < 0)
			{
				return false;
			}

			DigitalCodeLock digitalActionDoorLock = GetBuildingDoorDigitalCodeLock(digitalActionBuilding, digitalActionDoorIndex);
			if (digitalActionDoorLock != targetLock)
			{
				return false;
			}

			if (!digitalActionBuilding.IsDoorOpen(digitalActionDoorIndex))
			{
				digitalActionBuilding.OpenDoor(digitalActionDoorIndex);
			}

			return true;
		}

		return OpenForDigitalLock(targetLock);
	}

	static bool IsOpenForLock(CombinationLock targetLock)
	{
		if (!targetLock)
		{
			return false;
		}

		EntityAI lockParent = targetLock.GetHierarchyParent();
		if (!lockParent)
		{
			return false;
		}

		Fence fenceParent = Fence.Cast(lockParent);
		if (fenceParent)
		{
			return fenceParent.IsOpened();
		}

		#ifdef BuildingFortifications
		BF_DoorBarricade bfDoorParent = BF_DoorBarricade.Cast(lockParent);
		if (bfDoorParent)
		{
			return bfDoorParent.IsOpened();
		}

		BF_DoubleDoorBarricade bfDoubleDoorParent = BF_DoubleDoorBarricade.Cast(lockParent);
		if (bfDoubleDoorParent)
		{
			return bfDoubleDoorParent.IsOpened();
		}
		#endif

		#ifdef Cabin_Mod_RaGed
		Prefab_Cabin cabinParent = Prefab_Cabin.Cast(lockParent);
		if (cabinParent)
		{
			return cabinParent.IsOpened();
		}
		#endif

		BuildingBase buildingParent = BuildingBase.Cast(lockParent);
		if (buildingParent)
		{
			int lockDoorIndex = FindBuildingDoorIndexForLock(buildingParent, targetLock);
			if (lockDoorIndex >= 0)
			{
				return buildingParent.IsDoorOpen(lockDoorIndex);
			}
		}

		return false;
	}

	static bool OpenForLock(CombinationLock targetLock)
	{
		if (!targetLock)
		{
			return false;
		}

		EntityAI lockParent = targetLock.GetHierarchyParent();
		if (!lockParent)
		{
			return false;
		}

		Fence fenceParent = Fence.Cast(lockParent);
		if (fenceParent)
		{
			if (!fenceParent.IsOpened())
			{
				fenceParent.OpenFence();
			}

			return true;
		}

		#ifdef BuildingFortifications
		BF_DoorBarricade bfDoorParent = BF_DoorBarricade.Cast(lockParent);
		if (bfDoorParent)
		{
			if (!bfDoorParent.IsOpened())
			{
				bfDoorParent.OpenFence();
			}

			return true;
		}

		BF_DoubleDoorBarricade bfDoubleDoorParent = BF_DoubleDoorBarricade.Cast(lockParent);
		if (bfDoubleDoorParent)
		{
			if (!bfDoubleDoorParent.IsOpened())
			{
				bfDoubleDoorParent.OpenFence();
			}

			return true;
		}
		#endif

		#ifdef Cabin_Mod_RaGed
		Prefab_Cabin cabinParent = Prefab_Cabin.Cast(lockParent);
		if (cabinParent)
		{
			if (!cabinParent.IsOpened())
			{
				cabinParent.OpenCabinFence();
			}

			return true;
		}
		#endif

		BuildingBase buildingParent = BuildingBase.Cast(lockParent);
		if (buildingParent)
		{
			int lockDoorIndex = FindBuildingDoorIndexForLock(buildingParent, targetLock);
			if (lockDoorIndex >= 0)
			{
				if (!buildingParent.IsDoorOpen(lockDoorIndex))
				{
					buildingParent.OpenDoor(lockDoorIndex);
				}

				return true;
			}
		}

		return false;
	}

	static bool OpenForActionTarget(CombinationLock targetLock, ActionTarget actionTarget)
	{
		if (!targetLock || !actionTarget || !actionTarget.GetObject())
		{
			return false;
		}

		BuildingBase buildingTarget = BuildingBase.Cast(actionTarget.GetObject());
		if (buildingTarget)
		{
			int actionDoorIndex = buildingTarget.GetDoorIndex(actionTarget.GetComponentIndex());
			if (actionDoorIndex < 0)
			{
				return false;
			}

			CombinationLock actionDoorLock = GetBuildingDoorCombinationLock(buildingTarget, actionDoorIndex);
			if (actionDoorLock != targetLock)
			{
				return false;
			}

			if (!buildingTarget.IsDoorOpen(actionDoorIndex))
			{
				buildingTarget.OpenDoor(actionDoorIndex);
			}

			return true;
		}

		return OpenForLock(targetLock);
	}

	static void RelockIfNeeded(CombinationLock targetLock)
	{
		if (!targetLock || targetLock.IsLocked())
		{
			return;
		}

		EntityAI lockParent = targetLock.GetHierarchyParent();
		if (lockParent && lockParent.CanUseConstruction())
		{
			targetLock.LockServer(lockParent, true);
		}
	}

	static void RelockDigitalIfNeeded(DigitalCodeLock targetLock)
	{
		if (!targetLock || targetLock.IsLocked() || !targetLock.HasPIN() || !targetLock.HasEnergy())
		{
			return;
		}

		targetLock.SetLockState(true);
	}

	static void RelockActionTarget(ActionTarget actionTarget)
	{
		CombinationLock targetLock = ResolveActionLock(actionTarget);
		RelockIfNeeded(targetLock);
	}

	static void RelockDigitalActionTarget(ActionTarget actionTarget)
	{
		DigitalCodeLock targetDigitalLock = ResolveDigitalActionLock(actionTarget);
		RelockDigitalIfNeeded(targetDigitalLock);
	}
}
