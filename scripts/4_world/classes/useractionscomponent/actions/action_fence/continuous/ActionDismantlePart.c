modded class ActionDismantlePart
{
	override bool DismantleCondition(PlayerBase player, ActionTarget target, ItemBase item, bool camera_check)
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return super.DismantleCondition(player, target, item, camera_check);
		}
		#endif

		// If dismantle with lock is disabled, just return vanilla condition
		if (!GetZenComboLocksConfig().ClientSyncConfig || !GetZenComboLocksConfig().ClientSyncConfig.DismantleWithLock)
		{
			return super.DismantleCondition(player, target, item, camera_check);
		}
		
		// Dismantle with combo lock attached is enabled, override vanilla conditions
		if (player && !player.IsPlacingLocal() && !player.IsPlacingServer())
		{
			Object target_object = target.GetObject();
			EntityAI target_entity;
			if (target_object && target_object.CanUseConstruction())
			{
				//invalid if is flag and flag is raised (ONLY MODIFICATION: REMOVED COMBO LOCK CHECK)
				if (Class.CastTo(target_entity, target_object) && target_entity.FindAttachmentBySlotName("Material_FPole_Flag"))
				{
					return false;
				}

				string part_name = target_object.GetActionComponentName(target.GetComponentIndex());

				BaseBuildingBase base_building = BaseBuildingBase.Cast(target_object);
				Construction construction = base_building.GetConstruction();
				ConstructionPart construction_part = construction.GetConstructionPartToDismantle(part_name, item);

				if (construction_part)
				{
					//invalid on gate if the gate is opened
					if (construction_part.IsGate() && base_building.IsOpened())
						return false;

					//camera and position checks
					bool checked = false;

					if (construction_part.IsBase())
						checked = true;

					if (!checked && base_building.IsPlayerInside(player, part_name) && !player.GetInputController().CameraIsFreeLook())
					{
						//Camera check (client-only)
						if (camera_check)
						{
							if (GetGame() && (!GetGame().IsDedicatedServer()))
							{
								if (base_building.IsFacingCamera(part_name))
								{
									return false;
								}
							}
						}

						checked = true;
					}

					if (checked)
					{
						ConstructionActionData construction_action_data = player.GetConstructionActionData();
						construction_action_data.SetTargetPart(construction_part);

						return true;
					}
				}
			}
		}

		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
		#endif

		if (!action_data.m_Target || !action_data.m_Target.GetObject())
			return;

		// Check if target object is a fence and if it contains a combination lock
		Fence fence = Fence.Cast(action_data.m_Target.GetObject());
		if (!fence || !fence.GetCombinationLock())
			return;

		// Check if "gate" is constructed (ie. metal wire is built onto fence)
		Construction construction = BaseBuildingBase.Cast(action_data.m_Target.GetObject()).GetConstruction();
		bool wireFound = false;
		if (construction)
		{
			for (int i = 0; i < construction.GetConstructionParts().Count(); ++i)
			{
				string key = construction.GetConstructionParts().GetKey(i);
				ConstructionPart value = construction.GetConstructionParts().Get(key);

				if (value.IsBuilt())
				{
					if (value.GetPartName() == "wall_gate")
					{
						wireFound = true;
						break;
					}
				}
			}
		}

		// Wire has been dismantled - drop lock
		if (!wireFound)
		{
			fence.GetCombinationLock().UnlockServer(action_data.m_Player, fence);
			fence.GetCombinationLock().SetHealth(fence.GetCombinationLock().GetHealth() - 500);
			fence.GetInventory().DropEntity(InventoryMode.SERVER, fence, fence.GetCombinationLock());
		}
	}
};