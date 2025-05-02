modded class MissionServer
{
	// Called when server initializes
	override void OnInit()
	{
		super.OnInit();

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
		#endif

		Print("[ZenComboLocks] OnInit");

		// Load config
		GetZenComboLocksConfig();
	}

	// Called when a player connects to the server
	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity) 
	{
		super.InvokeOnConnect(player, identity);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
		#endif

		if (!player || !identity) 
			return;

		SendComboLockConfig(player);
	}

	// Send combo lock config to player from server
	void SendComboLockConfig(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;

		// Send client config
		Param2<ref ZenComboLocksClientConfig, bool> configParams = new Param2<ref ZenComboLocksClientConfig, bool>(
			GetZenComboLocksConfig().ClientSyncConfig,
			GetZenComboLocksConfig().IsAdminZCBL(player.GetIdentity().GetPlainId()));
		GetRPCManager().SendRPC("RPC_ZCL", "RPC_ReceiveZenComboLocksConfigOnClient", configParams, true, player.GetIdentity());
	}
}