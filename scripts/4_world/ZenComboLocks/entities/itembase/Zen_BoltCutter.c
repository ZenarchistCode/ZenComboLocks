class Zen_BoltCutter extends ToolBase
{
	protected int m_PlaySound = -1;
	protected int m_LastSound = -1;
	protected EffectSound m_Sound;

	override void InitItemVariables()
	{
		super.InitItemVariables();
		RegisterNetSyncVariableInt("m_PlaySound");
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (GetGame().IsClient() && m_PlaySound != m_LastSound)
		{
			PlaySoundSet(m_Sound, "Zen_BoltCutter_SoundSet", 0, 0);
			m_LastSound = m_PlaySound;
		}
	}

	void SyncSound()
	{
		m_PlaySound++;
		if (m_PlaySound > 6)
		{
			m_PlaySound = 0;
		}

		SetSynchDirty();
	}

	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionMineBush);
		AddAction(ActionMineTreeBark);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionUnmountBarbedWire);
		AddAction(Zen_ActionRaidCombinationLock);
	}
}
