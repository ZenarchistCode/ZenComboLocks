class ComboLockData
{
	ref array<string> m_PermittedPlayers;
	string m_PlayerOwner;

	void ComboLockData()
	{
		m_PermittedPlayers = new array<string>;
		m_PlayerOwner = "";
	}

	string GetOwner()
	{
		return m_PlayerOwner;
	}

	void SetOwner(string steamID)
	{
		m_PlayerOwner = steamID;
	}

	array<string> GetGuests()
	{
		return m_PermittedPlayers;
	}

	void ClearGuests()
	{
		m_PermittedPlayers.Clear();
	}
}