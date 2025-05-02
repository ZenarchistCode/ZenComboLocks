modded class ActionCloseFence
{
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return;
		}
		#endif

		Fence fence = Fence.Cast(action_data.m_Target.GetObject());
		if (fence)
		{
			// Lock the combo lock when fence is shut 
			// Unfortunately I need to do this here to cover all bases - some mods (like RaG basebuilding) do not call super.CloseFence()
			// and when mods don't call their super for vanilla functions it breaks all kinds of shit, like my mod - very frustrating.
			// If you're a mod author and you're reading this - PLEASE CALL SUPER() in your mods unless you have a very very good reason not to!
			if (fence.GetCombinationLock() != NULL && fence.GetCombinationLock().IsTakeable())
			{
				fence.GetCombinationLock().LockServer(fence, true);
			}
		}
	}
}