#pragma once

class AnalyticsEvent
{
public:
	std::string type;
	std::string postData;
	char authHeader[100];
};

class Analytics
{
	static std::vector<AnalyticsEvent> queue;
	static std::string session_id;
	static std::string session_num;
	static long servertimestamp;
	static std::string curlstring;
	static long starttime;
	static bool initialized;
	static std::string publickey;
	static std::string privatekey;
	static std::string GetUserID();
	static std::string GetUUID();
	static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
	static AnalyticsEvent CreateEvent(const std::string& category, const std::string& params);
	static bool QueueEvent(const std::string& category, const std::string& params = "");
	static bool SendEvent(const AnalyticsEvent& event, const long timeout);
public:
	static bool blocked;

	static bool Enable();
	static bool Disable();
	static void SetKeys(const std::string& gamekey, const std::string& secretkey);
	static bool SendGenericEvent(const std::string& eventid);
	static bool SendGenericEvent(const std::string& eventid, const float value);
	static bool SendErrorEvent(const std::string& severity, const std::string& message);
	static bool SendProgressEvent(const std::string& status, const std::string& levelname);
	static bool SendProgressEvent(const std::string& status, const std::string& levelname, const int score);
	static bool SendProgressEvent(const std::string& status, const std::string& levelname, const int score, const int attempt_num);
	static bool SendResourceEvent(const std::string& flowType, const std::string& currency, const std::string& itemType, const std::string& itemid, const float amount);
	static bool SendBusinessEvent(const std::string& itemType, const std::string& itemid, const int amount, const std::string& currency, const uint64_t transaction_num, const std::string& cart_type = "", const std::string& receipt_info="");
};