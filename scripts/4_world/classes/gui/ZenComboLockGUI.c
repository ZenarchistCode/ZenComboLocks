class ZenComboLockGUI extends Managed
{
	// Layout constants
	static const string LAYOUT_FILE = "ZenComboLocks/data/gui/layouts/ZenComboLocksHUD.layout";

	// Layout components
	ref Widget m_LayoutRoot;
	ref TextWidget m_ComboLockWidget;
	ref WidgetFadeTimer m_FadeTimer;

	// Init widgets
	void ZenComboLockGUI()
	{
		// Load layout
		m_LayoutRoot = GetGame().GetWorkspace().CreateWidgets(LAYOUT_FILE);

		// If layout fails to load, stop here
		if (!m_LayoutRoot)
			return;

		// Load text widget
		m_LayoutRoot.Show(true);
		m_ComboLockWidget = TextWidget.Cast(m_LayoutRoot.FindAnyWidget("ComboLockDial"));

		// Set text widget to blank string
		if (m_ComboLockWidget)
		{
			m_ComboLockWidget.SetText("");

			if (GetZenComboLocksConfig().ClientSyncConfig.DisplayDigits == 2)
			{
				// Move text widget down if we're displaying full code
				float x;
				float y;
				m_ComboLockWidget.GetPos(x, y);
				m_ComboLockWidget.SetPos(x, y + 1);
			}
		}

		// Prepare widget fade timer for fadeout effect
		m_FadeTimer = new WidgetFadeTimer();
	}

	// Destroy ZenComboLockGUI object, remove any call queues etc.
	void ~ZenComboLockGUI()
	{
		if (m_FadeTimer && m_FadeTimer.IsRunning())
			m_FadeTimer.Stop();

		m_ComboLockWidget.Show(false);
		delete m_ComboLockWidget;
		delete m_FadeTimer;
		delete m_LayoutRoot;
	}

	// Set combo lock display text
	void SetText(string text)
	{
		if (m_ComboLockWidget)
		{
			m_ComboLockWidget.SetText(text);
			m_FadeTimer.FadeOut(m_ComboLockWidget, 1, false); // Fade out in 1 second
			ScheduleDeletion(5000);
		}
	}

	// Schedule deletion of this widget
	void ScheduleDeletion(int ms)
	{
		// Remove any existing DeleteWidget queue and add a new one to destroy widget in X miliseconds
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.DeleteWidget);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.DeleteWidget, ms, false);
	}

	// Delete this widget
	void DeleteWidget()
	{
		delete this;
	}
}