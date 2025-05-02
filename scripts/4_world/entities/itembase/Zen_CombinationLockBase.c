class Zen_CombinationLockbase extends CombinationLock
{
	// Set lock digit count based on config.cpp
	override void SetBaseLockValues()
	{
		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			super.SetBaseLockValues();
			return;
		}
		#endif

		m_LockDigits = ConfigGetInt("lockDigits");
		string combination = "";
		string combinationLocked = "";

		for (int i = 0; i < m_LockDigits; i++)
		{
			combination = combination + "1";
			combinationLocked = combinationLocked + "9";
		}

		m_Combination = combination.ToInt();
		m_CombinationLocked = combinationLocked.ToInt();
	}

	// Set digit number in displayName
	override string GetDisplayName()
	{
		string displayName = ConfigGetString("displayName");

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return displayName;
		}
		#endif

		displayName.Replace("$", m_LockDigits.ToString());
		return displayName;
	}

	// Set digit number in description tooltip
	override string GetTooltip()
	{
		string description = ConfigGetString("descriptionShort");

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenComboLocks"))
		{
			return description;
		}
		#endif

		description.Replace("$", m_LockDigits.ToString());
		return description;
	}
}

// Define custom combo locks
class Zen_CombinationLock4 extends Zen_CombinationLockbase {}
class Zen_CombinationLock5 extends Zen_CombinationLockbase {}
class Zen_CombinationLock6 extends Zen_CombinationLockbase {}
class Zen_CombinationLock7 extends Zen_CombinationLockbase {}
class Zen_CombinationLock8 extends Zen_CombinationLockbase {}
class Zen_CombinationLock9 extends Zen_CombinationLockbase {}