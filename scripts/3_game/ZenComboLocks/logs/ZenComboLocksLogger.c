class ZenComboLocksLogger
{
	const static string LOG_FOLDER = "$profile:\\Zenarchist\\Logs\\";
	const static string LOG_FILE = "ZenComboLocks";

	static void Log(string text)
	{
		if (!ZenComboLocks_IsEnabled() || !GetZenComboLocksConfig().ServerConfig.PrintLogs)
		{
			return;
		}

		string logPath = LOG_FOLDER + LOG_FILE + ".log";
		if (!FileExist(LOG_FOLDER))
		{
			MakeDirectory(LOG_FOLDER);
		}

		FileHandle logFile = OpenFile(logPath, FileMode.APPEND);
		if (logFile != 0)
		{
			FPrintln(logFile, GetDate() + " [ZenComboLocks] " + text);
			CloseFile(logFile);
		}
	}

	static protected string GetDate()
	{
		int logYear;
		int logMonth;
		int logDay;
		int logHour;
		int logMinute;
		int logSecond;

		GetYearMonthDay(logYear, logMonth, logDay);
		GetHourMinuteSecond(logHour, logMinute, logSecond);

		return logDay.ToStringLen(2) + "." + logMonth.ToStringLen(2) + "." + logYear.ToStringLen(4) + " " + logHour.ToStringLen(2) + ":" + logMinute.ToStringLen(2) + ":" + logSecond.ToStringLen(2);
	}
}
