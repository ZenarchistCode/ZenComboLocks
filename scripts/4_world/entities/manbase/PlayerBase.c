modded class PlayerBase
{
	// ZCBL keyword is to prevent potential mod conflicts with other mods that might use IsAdmin
	protected bool IsAdminZCBL = false;

	// Set player admin authority
	void SetIsAdminZCBL(bool b)
	{
		IsAdminZCBL = b;
	}

	// Returns whether player is admin based on client config or server-side config wherever it is called
	bool IsAdminZCBL()
	{
		if (GetGame().IsClient())
			return IsAdminZCBL;

		if (GetIdentity())
			return GetZenComboLocksConfig().IsAdminZCBL(GetIdentity().GetPlainId());

		return false;
	}
}