class Zen_BoltCutter extends ToolBase
{
	// Play cutting sound
	protected int m_PlaySound = -1;
	protected int m_LastSound = -1;
	protected EffectSound m_Sound;

	// Register server/client sync variable
	override void InitItemVariables()
	{
		super.InitItemVariables();
		RegisterNetSyncVariableInt("m_PlaySound");
	}

	// Called whenever variables sync server->client
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (GetGame().IsClient())
		{
			if (m_PlaySound != m_LastSound)
			{
				PlaySoundSet(m_Sound, "Zen_BoltCutter_SoundSet", 0, 0);
				m_LastSound = m_PlaySound;
			}
		}
	}

	// Sync sound to client
	void SyncSound()
	{
		m_PlaySound = m_PlaySound + 1;
		if (m_PlaySound > 6)
			m_PlaySound = 0;

		SetSynchDirty();
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionMineBush);
		AddAction(ActionMineTreeBark);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionUnmountBarbedWire);
		AddAction(Zen_ActionRaidComboLockTarget);
		AddAction(Zen_ActionRaidComboLockFence);
	}
};