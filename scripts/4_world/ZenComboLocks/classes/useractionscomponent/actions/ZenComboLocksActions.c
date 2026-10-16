class ZenComboLocksActionHelper
{
	static bool UsesInstantOpen(PlayerBase player)
	{
		if (!player || !GetZenComboLocksConfig().ClientSyncConfig)
		{
			return false;
		}

		if (GetZenComboLocksConfig().ClientSyncConfig.InstantOpen)
		{
			return true;
		}

		return GetZenComboLocksConfig().ClientSyncConfig.AllowAdminOpen && player.IsZenAdmin();
	}

	static bool CanOpen(PlayerBase player, ActionTarget target, bool instantAction)
	{
		if (!ZenComboLocks_IsEnabled() || !player || !target || !target.GetObject() || !GetZenComboLocksConfig().ClientSyncConfig)
		{
			return false;
		}

		if (UsesInstantOpen(player) != instantAction)
		{
			return false;
		}

		if (!ZenComboLocksHelper.IsDirectLockTarget(target) && !GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
		{
			return false;
		}

		CombinationLock actionLock = ZenComboLocksHelper.ResolveActionLock(target);
		if (!actionLock || !actionLock.IsLockedOnParent() || ZenComboLocksHelper.IsOpenForLock(actionLock))
		{
			return false;
		}

		BuildingBase actionBuilding = BuildingBase.Cast(target.GetObject());
		if (actionBuilding && !actionBuilding.DoorsDirectionalCheck(player, target.GetComponentIndex()))
		{
			return false;
		}

		if (!actionLock.ZenHasReceivedClientPermissions())
		{
			return false;
		}

		return actionLock.ZenCanOpen(player);
	}

	static bool OpenOnServer(PlayerBase player, ActionTarget target)
	{
		if (!player || !target)
		{
			return false;
		}

		CombinationLock serverLock = ZenComboLocksHelper.ResolveActionLock(target);
		if (!serverLock || !serverLock.IsLockedOnParent() || !serverLock.ZenCanOpen(player))
		{
			return false;
		}

		if (!serverLock.ZenOpenForRememberedPlayer(target))
		{
			return false;
		}

		if (player.GetIdentity())
		{
			ZenComboLocksLogger.Log("Remembered player " + player.GetIdentity().GetPlainId() + " opened " + serverLock.GetType() + " at " + serverLock.GetPosition());
		}

		return true;
	}

	static bool CanRemoveWithoutCode(PlayerBase player, ActionTarget target)
	{
		if (!ZenComboLocks_IsEnabled() || !player || !target || !target.GetObject() || !GetZenComboLocksConfig().ClientSyncConfig)
		{
			return false;
		}

		if (!ZenComboLocksHelper.IsDirectLockTarget(target) && !GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
		{
			return false;
		}

		CombinationLock removableLock = ZenComboLocksHelper.ResolveActionLock(target);
		if (!removableLock || !removableLock.IsLockAttached() || ZenComboLocksHelper.IsOpenForLock(removableLock))
		{
			return false;
		}

		if (!removableLock.ZenHasReceivedClientPermissions())
		{
			return false;
		}

		return removableLock.ZenCanRemoveWithoutCode(player);
	}
}

class Zen_ActionOpenRememberedLockCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float repeatTime = 0.5;
		if (GetZenComboLocksConfig().ClientSyncConfig && GetZenComboLocksConfig().ClientSyncConfig.DialTime > 0.05)
		{
			repeatTime = GetZenComboLocksConfig().ClientSyncConfig.DialTime;
		}

		m_ActionData.m_ActionComponent = new CAContinuousRepeat(repeatTime);
	}
}

class Zen_ActionOpenRememberedLock : ActionContinuousBase
{
	void Zen_ActionOpenRememberedLock()
	{
		m_CallbackClass = Zen_ActionOpenRememberedLockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#open";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool HasProneException()
	{
		return true;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return ZenComboLocksActionHelper.CanOpen(player, target, false);
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		CombinationLock startingLock = ZenComboLocksHelper.ResolveActionLock(action_data.m_Target);
		if (startingLock)
		{
			startingLock.ZenResetOpenProgress();
		}
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		CombinationLock progressingLock = ZenComboLocksHelper.ResolveActionLock(action_data.m_Target);
		if (!progressingLock || !progressingLock.IsLockedOnParent() || !progressingLock.ZenCanOpen(action_data.m_Player))
		{
			return;
		}

		progressingLock.StartItemSoundServer(SoundConstants.ITEM_COMBINATION_LOCK_CHANGE_DIAL);

		if (progressingLock.ZenAdvanceOpenProgress())
		{
			ZenComboLocksActionHelper.OpenOnServer(action_data.m_Player, action_data.m_Target);
		}
	}
}

class Zen_ActionOpenRememberedLockInstant : ActionInteractBase
{
	void Zen_ActionOpenRememberedLockInstant()
	{
		m_Text = "#open";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool IsInstant()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return ZenComboLocksActionHelper.CanOpen(player, target, true);
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		ZenComboLocksActionHelper.OpenOnServer(action_data.m_Player, action_data.m_Target);
	}
}

class ZenComboLocksDigitalActionHelper
{
	static bool CanOpen(PlayerBase player, ActionTarget target, bool instantAction)
	{
		if (!ZenComboLocks_IsEnabled() || !player || !target || !target.GetObject() || !GetZenComboLocksConfig().ClientSyncConfig)
		{
			return false;
		}

		if (ZenComboLocksActionHelper.UsesInstantOpen(player) != instantAction)
		{
			return false;
		}

		if (!ZenComboLocksHelper.IsDirectDigitalLockTarget(target) && !GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
		{
			return false;
		}

		DigitalCodeLock actionDigitalLock = ZenComboLocksHelper.ResolveDigitalActionLock(target);
		if (!actionDigitalLock || !actionDigitalLock.CanSetCode() || !actionDigitalLock.HasPIN() || !actionDigitalLock.HasEnergy() || !actionDigitalLock.IsLocked() || ZenComboLocksHelper.IsOpenForDigitalLock(actionDigitalLock))
		{
			return false;
		}

		BuildingBase actionDigitalBuilding = BuildingBase.Cast(target.GetObject());
		if (actionDigitalBuilding && !actionDigitalBuilding.DoorsDirectionalCheck(player, target.GetComponentIndex()))
		{
			return false;
		}

		if (!actionDigitalLock.ZenHasReceivedDigitalClientPermissions())
		{
			return false;
		}

		return actionDigitalLock.ZenCanOpenDigitalLock(player);
	}

	static bool OpenOnServer(PlayerBase player, ActionTarget target)
	{
		if (!player || !target)
		{
			return false;
		}

		DigitalCodeLock serverDigitalLock = ZenComboLocksHelper.ResolveDigitalActionLock(target);
		if (!serverDigitalLock || !serverDigitalLock.CanSetCode() || !serverDigitalLock.HasPIN() || !serverDigitalLock.HasEnergy() || !serverDigitalLock.IsLocked() || !serverDigitalLock.ZenCanOpenDigitalLock(player))
		{
			return false;
		}

		if (!serverDigitalLock.ZenOpenForRememberedDigitalPlayer(target))
		{
			return false;
		}

		if (player.GetIdentity())
		{
			ZenComboLocksLogger.Log("Remembered player " + player.GetIdentity().GetPlainId() + " opened " + serverDigitalLock.GetType() + " at " + serverDigitalLock.GetPosition());
		}

		return true;
	}
}

class Zen_ActionOpenRememberedDigitalLockCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float digitalRepeatTime = 0.5;
		if (GetZenComboLocksConfig().ClientSyncConfig && GetZenComboLocksConfig().ClientSyncConfig.DialTime > 0.05)
		{
			digitalRepeatTime = GetZenComboLocksConfig().ClientSyncConfig.DialTime;
		}

		m_ActionData.m_ActionComponent = new CAContinuousRepeat(digitalRepeatTime);
	}
}

class Zen_ActionOpenRememberedDigitalLock : ActionContinuousBase
{
	void Zen_ActionOpenRememberedDigitalLock()
	{
		m_CallbackClass = Zen_ActionOpenRememberedDigitalLockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#open";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool HasProneException()
	{
		return true;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return ZenComboLocksDigitalActionHelper.CanOpen(player, target, false);
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		DigitalCodeLock startingDigitalLock = ZenComboLocksHelper.ResolveDigitalActionLock(action_data.m_Target);
		if (startingDigitalLock)
		{
			startingDigitalLock.ZenResetDigitalOpenProgress();
		}
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		DigitalCodeLock progressingDigitalLock = ZenComboLocksHelper.ResolveDigitalActionLock(action_data.m_Target);
		if (!progressingDigitalLock || !progressingDigitalLock.IsLocked() || !progressingDigitalLock.ZenCanOpenDigitalLock(action_data.m_Player))
		{
			return;
		}

		progressingDigitalLock.StartItemSoundServer(SoundConstants.ITEM_DIGITAL_LOCK_DIGIT_INPUT);

		if (progressingDigitalLock.ZenAdvanceDigitalOpenProgress())
		{
			ZenComboLocksDigitalActionHelper.OpenOnServer(action_data.m_Player, action_data.m_Target);
		}
	}
}

class Zen_ActionOpenRememberedDigitalLockInstant : ActionInteractBase
{
	void Zen_ActionOpenRememberedDigitalLockInstant()
	{
		m_Text = "#open";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool IsInstant()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return ZenComboLocksDigitalActionHelper.CanOpen(player, target, true);
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		ZenComboLocksDigitalActionHelper.OpenOnServer(action_data.m_Player, action_data.m_Target);
	}
}

class Zen_ActionRemoveRememberedLockCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(3);
	}
}

class Zen_ActionRemoveRememberedLock : ActionContinuousBase
{
	void Zen_ActionRemoveRememberedLock()
	{
		m_CallbackClass = Zen_ActionRemoveRememberedLockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;
		m_Text = "Remove combination lock";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool HasProneException()
	{
		return true;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return ZenComboLocksActionHelper.CanRemoveWithoutCode(player, target);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		if (!ZenComboLocksActionHelper.CanRemoveWithoutCode(action_data.m_Player, action_data.m_Target))
		{
			return;
		}

		CombinationLock removableLock = ZenComboLocksHelper.ResolveActionLock(action_data.m_Target);
		if (!removableLock)
		{
			return;
		}

		EntityAI removableParent = removableLock.GetHierarchyParent();
		if (!removableParent)
		{
			return;
		}

		string removedLockType = removableLock.GetType();
		vector removedLockPosition = removableLock.GetPosition();
		removableLock.ZenForceUnlockAndDrop(action_data.m_Player, removableParent);

		if (action_data.m_Player.GetIdentity())
		{
			ZenComboLocksLogger.Log("Player " + action_data.m_Player.GetIdentity().GetPlainId() + " removed " + removedLockType + " without its PIN at " + removedLockPosition);
		}
	}
}

class Zen_ActionRaidCombinationLockCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float raidTime = 1;
		CombinationLock callbackLock = ZenComboLocksHelper.ResolveActionLock(m_ActionData.m_Target);
		if (callbackLock)
		{
			ZenLockConfig callbackRaidConfig = GetZenComboLocksConfig().GetRaidConfigForType(callbackLock.GetType());
			if (callbackRaidConfig && callbackRaidConfig.TimeRequired > 0)
			{
				raidTime = callbackRaidConfig.TimeRequired;
			}
		}

		m_ActionData.m_ActionComponent = new CAContinuousTime(raidTime);
	}
}

class Zen_ActionRaidCombinationLock : ActionContinuousBase
{
	private int m_LastSoundUpdate;
	private int m_NextSoundUpdate;

	void Zen_ActionRaidCombinationLock()
	{
		m_CallbackClass = Zen_ActionRaidCombinationLockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#destroy";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.BASEBUILDING);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!ZenComboLocks_IsEnabled() || !player || !target || !target.GetObject() || !Zen_BoltCutter.Cast(item))
		{
			return false;
		}

		if (!GetZenComboLocksConfig().ClientSyncConfig || !GetZenComboLocksConfig().ClientSyncConfig.RaidConfig || !GetZenComboLocksConfig().ClientSyncConfig.RaidConfig.Enabled)
		{
			return false;
		}

		if (!ZenComboLocksHelper.IsDirectLockTarget(target) && !GetZenComboLocksConfig().ClientSyncConfig.InteractAnywhere)
		{
			return false;
		}

		CombinationLock raidLock = ZenComboLocksHelper.ResolveActionLock(target);
		if (!raidLock || !raidLock.IsLockedOnParent())
		{
			return false;
		}

		ZenLockConfig raidConfig = GetZenComboLocksConfig().GetRaidConfigForType(raidLock.GetType());
		if (!raidConfig)
		{
			return false;
		}

		m_Text = "#destroy " + raidLock.GetDisplayName();
		return true;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		Zen_BoltCutter boltCutter = Zen_BoltCutter.Cast(action_data.m_MainItem);
		if (!boltCutter || boltCutter.IsRuined() || !action_data.m_Player || !action_data.m_Player.GetIdentity())
		{
			return;
		}

		CombinationLock raidedLock = ZenComboLocksHelper.ResolveActionLock(action_data.m_Target);
		if (!raidedLock || !raidedLock.IsLockedOnParent())
		{
			return;
		}

		ZenLockConfig activeRaidConfig = GetZenComboLocksConfig().GetRaidConfigForType(raidedLock.GetType());
		if (!activeRaidConfig)
		{
			return;
		}

		EntityAI raidedParent = raidedLock.GetHierarchyParent();
		if (!raidedParent)
		{
			return;
		}

		string raidedLockType = raidedLock.GetType();
		vector raidedLockPosition = raidedLock.GetPosition();
		raidedLock.ZenForceUnlockAndDrop(action_data.m_Player, raidedParent);
		raidedLock.SetHealth(0);
		boltCutter.SetHealth(boltCutter.GetHealth() - activeRaidConfig.DamageTool);
		ZenComboLocksLogger.Log("Player " + action_data.m_Player.GetIdentity().GetPlainId() + " destroyed " + raidedLockType + " with bolt cutters at " + raidedLockPosition);
	}

	override void Do(ActionData action_data, int state)
	{
		super.Do(action_data, state);

		if (!GetGame().IsDedicatedServer() || !action_data.m_MainItem)
		{
			return;
		}

		if (g_Game.GetTime() - m_LastSoundUpdate >= m_NextSoundUpdate)
		{
			m_LastSoundUpdate = g_Game.GetTime();
			m_NextSoundUpdate = Math.RandomIntInclusive(3000, 10000);

			Zen_BoltCutter activeBoltCutter = Zen_BoltCutter.Cast(action_data.m_MainItem);
			if (activeBoltCutter)
			{
				activeBoltCutter.SyncSound();
			}
		}
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		m_LastSoundUpdate = 0;
		m_NextSoundUpdate = 0;
	}
}
