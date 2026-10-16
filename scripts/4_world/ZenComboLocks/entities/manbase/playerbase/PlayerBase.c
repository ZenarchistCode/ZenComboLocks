modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);

		AddAction(Zen_ActionOpenRememberedLock, InputActionMap);
		AddAction(Zen_ActionOpenRememberedLockInstant, InputActionMap);
		AddAction(Zen_ActionOpenRememberedDigitalLock, InputActionMap);
		AddAction(Zen_ActionOpenRememberedDigitalLockInstant, InputActionMap);
		AddAction(Zen_ActionRemoveRememberedLock, InputActionMap);
	}
}
