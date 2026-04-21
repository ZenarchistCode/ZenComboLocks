// If anyone's wondering why I put Zen_ in front of every god damn thing it's to stop conflicts with other mods not because I'm an egomaniac lol
modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);

        // Custom combo lock actions
        actions.Insert(Zen_ActionOpenComboLock);
        actions.Insert(Zen_ActionRemoveComboLock);
        actions.Insert(Zen_ActionOpenComboLockInstant);
        actions.Insert(Zen_ActionManageCombinationLockOnTarget);
        actions.Insert(Zen_ActionRemoveCombinationLockOnFence);

        // Vanilla combo lock action overrides
        actions.Insert(Zen_ActionDialCombinationLockOnTarget);
        actions.Insert(Zen_ActionNextCombinationLockDialOnTarget);

        // Fence interact anywhere actions
        actions.Insert(Zen_ActionOpenComboLockFence);
        actions.Insert(Zen_ActionOpenComboLockInstantFence);
        actions.Insert(Zen_ActionManageCombinationLockOnFence);
        actions.Insert(Zen_ActionNextCombinationLockDialOnFence);
        actions.Insert(Zen_ActionDialCombinationLockOnFence);

        // Raid actions
        actions.Insert(Zen_ActionRaidComboLockTarget);
        actions.Insert(Zen_ActionRaidComboLockFence);

        // Admin actions
        actions.Insert(Zen_ActionAdminCombinationLockOnTarget);
        actions.Insert(Zen_ActionAdminCombinationLockOnFence);
    }
}