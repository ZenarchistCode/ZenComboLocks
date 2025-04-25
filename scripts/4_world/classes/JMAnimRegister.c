modded class JMAnimRegister
{
	override void OnRegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.OnRegisterOneHanded(pType, pBehavior);
		pType.AddItemInHandsProfileIK("Zen_BoltCutter", "dz/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", pBehavior, "dz/anims/anm/player/ik/gear/crowbar.anm");
	}
};