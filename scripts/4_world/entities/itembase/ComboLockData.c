class ComboLockData
{
	ref array<string> m_PermittedPlayers;
	string m_PlayerOwner;

	void ComboLockData()
	{
		m_PermittedPlayers = new array<string>;
		m_PlayerOwner = "";
	}
}