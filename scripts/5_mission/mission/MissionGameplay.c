modded class MissionGameplay
{
    // Register RPC
    void MissionGameplay()
    {
        GetRPCManager().AddRPC("RPC_ZCL", "RPC_ReceiveZenComboLocksConfigOnClient", this, SingeplayerExecutionType.Client);
    }

    // Receive config sent from server -> client
    void RPC_ReceiveZenComboLocksConfigOnClient(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type == CallType.Client)
        {
            Param2<ref ZenComboLocksClientConfig, bool> data;

            // If data fails to read, stop here.
            if (!ctx.Read(data))
            {
                Print("IMPORTANT ERROR: ZenComboLocksConfig failed to be read on client! Please tell Zenarchist he fucked up!");
                return;
            }

            // Data received - sync to client config
            GetZenComboLocksConfig().ClientSyncConfig = data.param1;

            // Get client player and set admin authority from server
            PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
            if (player)
                player.SetIsAdminZCBL(data.param2);
        }
    }
}