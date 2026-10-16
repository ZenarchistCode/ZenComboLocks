// Persistence-only migration classes for locks spawned by ZenComboLocks 1.29.
// Every legacy type is hidden and now uses the native four-dial 1.30 lock.
class Zen_CombinationLockBase extends CombinationLock4
{
	override void AfterStoreLoad()
	{
		m_Combination = m_Combination % 10000;
		m_CombinationInside = m_CombinationInside % 10000;
		m_CombinationLocked = m_CombinationLocked % 10000;
		m_LockDigitsCount = m_CombinationLocked.ToString().Length();

		super.AfterStoreLoad();
	}
}

class Zen_CombinationLock4 extends Zen_CombinationLockBase {}
class Zen_CombinationLock5 extends Zen_CombinationLockBase {}
class Zen_CombinationLock6 extends Zen_CombinationLockBase {}
class Zen_CombinationLock7 extends Zen_CombinationLockBase {}
class Zen_CombinationLock8 extends Zen_CombinationLockBase {}
class Zen_CombinationLock9 extends Zen_CombinationLockBase {}
