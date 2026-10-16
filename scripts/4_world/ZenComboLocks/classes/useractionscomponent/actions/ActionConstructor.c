modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);

		actions.Insert(Zen_ActionOpenRememberedLock);
		actions.Insert(Zen_ActionOpenRememberedLockInstant);
		actions.Insert(Zen_ActionOpenRememberedDigitalLock);
		actions.Insert(Zen_ActionOpenRememberedDigitalLockInstant);
		actions.Insert(Zen_ActionRemoveRememberedLock);
		actions.Insert(Zen_ActionRaidCombinationLock);
	}
}
