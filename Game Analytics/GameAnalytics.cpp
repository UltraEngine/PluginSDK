#include "pch.h"

std::vector<AnalyticsEvent> Analytics::queue;
std::string Analytics::session_id;
std::string Analytics::session_num;
long Analytics::servertimestamp = 0;
bool Analytics::blocked = false;
bool Analytics::initialized = false;
long Analytics::starttime = 0;
std::string Analytics::curlstring;
std::string Analytics::publickey;
std::string Analytics::privatekey;

std::string Analytics::GetUserID()
{
	std::string userid;
	if (Steamworks::initialized)
	{
		//Create user ID from Steam ID. It can't be reversed to the Steam ID but will always be unique.
		uint64_t steamid = Steamworks::GetUserID();
		steamid *= Int(Right(String(steamid), 2));
		userid = String(steamid);
	}
	else
	{
		//Create a UUID and store it in the game settings
		userid = GetProperty("userid");
		if (userid == "")
		{
			userid = GetUUID();
			SetProperty("userid", userid);
		}
	}
	return userid;
}

std::string Analytics::GetUUID()
{
#ifdef _WIN32
	UUID uuid;

	// Create uuid or load from a string by UuidFromString() function
	UuidCreate(&uuid);

	// If you want to convert uuid to string, use UuidToString() function
	CHAR* szUuid = NULL;
	UuidToString(&uuid, (RPC_CSTR*)&szUuid);

	std::string s;
	if (szUuid != NULL)
	{
		s = std::string(szUuid);
		RpcStringFree((RPC_CSTR*)&szUuid);
		szUuid = NULL;
	}
	return s;
#else
	//uuid_t id;
	//uuid_generate(id);
	//char *s = char[100];
	//uuid_unparse(id, &s);
	//uuid = s;
	return "";
#endif
}

/*int Analytics::GetUserTeam()
{
	std::string userid = GetUserID();
	return ((int(userid[userid.length() - 1]) & 2));
}*/

void Analytics::SetKeys(const std::string& gamekey, const std::string& secretkey)
{
	publickey = gamekey;
	privatekey = secretkey;
}

bool Analytics::Disable()
{
	bool result = true;
	if (initialized)
	{
		long start = Millisecs();
		int maxtime = 5000;
		int eventtimeout = 3;
		QueueEvent("session_end", "\"length\": " + String((Millisecs() - starttime) / 1000));
#ifdef _WIN32
		for (auto i = 0; i < queue.size(); ++i)
		{
			//Send end session event and exit loop if taking too long
			if (Millisecs() - start > maxtime)
			{
				SendEvent(queue[queue.size()-1], eventtimeout);
				Print("Warning: analytics taking too long to send, skipping events.");
				result = false;
				break;
			}
			if (!SendEvent(queue[i], eventtimeout)) result = false;
		}
		queue.clear();
		curl_global_cleanup();
#endif
		initialized = false;
	}
	return result;
}

void hmac(unsigned char* key, int keylen, unsigned char* message, int messagelen, unsigned char* out)
{
#ifdef _WIN32
	HMAC_CTX *ctx = HMAC_CTX_new();
	unsigned int len = 32;

	// Using sha1 hash engine here.
	// You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
	HMAC_Init_ex(ctx, key, keylen, EVP_sha256(), NULL);
	HMAC_Update(ctx, message, messagelen);
	HMAC_Final(ctx, out, &len);
	HMAC_CTX_free(ctx);
#endif
}

size_t Analytics::write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	char c[1024];
	memset(c, 0, 1024);
	int sz = size * nmemb;
	memcpy(c, buffer, min(1024-1, sz));
	curlstring = std::string(c);
	return sz;
}

bool Analytics::Enable()
{
	//System::Initialize();
	if (initialized) return true;
	if (blocked) return false;
#ifndef _WIN32
	return false;
#else
	session_id = GetUUID();
	session_num = String(Int(GetProperty("session_number", "0")));

	AnalyticsEvent event;

	//Post data
	event.postData = std::string("{\"platform\":\"") + Lower(GetOS()) + "\", \"os_version\":\"0\", \"sdk_version\":\"rest api v2\"}";
	event.type = "init";

	//Construct authentication token
	unsigned char buff[32];
	hmac((unsigned char*)privatekey.c_str(), privatekey.length(), (unsigned char*)event.postData.c_str(), event.postData.length(), buff);
	std::string encoding = base64_encode((const unsigned char*)buff, 32);
	int len = sprintf(event.authHeader, "Authorization: %s", encoding.c_str());

	curl_global_init(CURL_GLOBAL_DEFAULT);

	if (!SendEvent(event,4)) return false;

	//Parse result to get server timestamp
	std::vector<std::string> sarr = Split(curlstring, ",");
	for (auto i = 0; i < sarr.size(); i++)
	{
		std::vector<std::string> sarr2 = Split(sarr[i], ":");
		if (sarr2[0] == "\"server_ts\"")
		{
			servertimestamp = UInt64(sarr2[1]);
			break;
		}
	}

	initialized = true;
	starttime = Millisecs();
	QueueEvent("user");
	return true;
#endif
}

bool Analytics::SendGenericEvent(const std::string& eventid)
{
	return QueueEvent("design", "\"event_id\": \"" + eventid + "\"");
}

bool Analytics::SendGenericEvent(const std::string& eventid, const float value)
{
	return QueueEvent("design", "\"event_id\": \"" + eventid + "\", \"value\": " + String(value));
}

bool Analytics::SendErrorEvent(const std::string& severity, const std::string& message)
{
	return QueueEvent("error", "\"severity\": \"" + severity + "\", \"message\": \"" + message + "\"");
}

bool Analytics::SendProgressEvent(const std::string& status, const std::string& levelname, const int score, const int attempt_num)
{
	std::string eventid = status + ":" + levelname;
	std::string params;
	params += "\"event_id\": \"" + eventid + "\", ";
	params += "\"score\": " + String(score) + ", ";
	params += "\"attempt_num\": " + String(attempt_num);
	return QueueEvent("progression", params);
}

bool Analytics::SendProgressEvent(const std::string& status, const std::string& levelname, const int score)
{
	std::string eventid = status + ":" + levelname;
	std::string params;
	params += "\"event_id\": \"" + eventid + "\", ";
	params += "\"score\": " + String(score);
	return QueueEvent("progression", params);
}

bool Analytics::SendProgressEvent(const std::string& status, const std::string& levelname)
{
	std::string eventid = status + ":" + levelname;
	std::string params;
	params += "\"event_id\": \"" + eventid + "\"";
	return QueueEvent("progression", params);
}

bool Analytics::SendResourceEvent(const std::string& flowType, const std::string& currency, const std::string& itemType, const std::string& itemid, const float amount)
{
	std::string flowType_ = flowType;
	if (flowType_ == "Earn") flowType_ = "Source";
	if (flowType_ == "Spend") flowType_ = "Sink";
	std::string eventid = flowType_ + ":" + currency + ":" + itemType + ":" + itemid;
	eventid += ":" + currency + ":" + itemType + ":" + itemid;
	std::string params;
	params += "\"event_id\": \"" + eventid + "\", ";
	params += "\"amount\": " + String(amount);
	return QueueEvent("resource", params);
}

bool Analytics::SendBusinessEvent(const std::string& itemType, const std::string& itemid, const int amount, const std::string& currency, const uint64_t transaction_num, const std::string& cart_type, const std::string& receipt_info)
{
	std::string eventid = itemType + ":" + itemid;
	std::string params;
	params += "\"event_id\": \"" + eventid + "\", ";
	params += "\"amount\": " + String(amount) + ", ";
	params += "\"currency\": \"" + currency + "\", ";
	params += "\"transaction_num\": " + String(transaction_num);
	if (cart_type!="") params += ", \"cart_type\": \"" + cart_type + "\"";
	if (receipt_info != "") params += ", \"receipt_info\": \"" + receipt_info + "\"";
	return QueueEvent("business", params);
}

AnalyticsEvent Analytics::CreateEvent(const std::string& category, const std::string& params)
{
	AnalyticsEvent event;
#ifdef _WIN32
	std::string token;

	event.type = "events";

	//Post data
	event.postData = "[{";

	//Requires parameters
	event.postData += "\"category\": \"" + category + "\", ";
	event.postData += "\"device\": \"unknown\", ";
	event.postData += "\"v\": 2, ";
	event.postData += "\"user_id\": \"" + Analytics::GetUserID() + "\", ";
	event.postData += "\"client_ts\": " + String(Analytics::servertimestamp + (Millisecs() - Analytics::starttime)) + ", ";
	event.postData += "\"sdk_version\": \"rest api v2\", ";
	event.postData += "\"os_version\": \"" + Lower(GetOS()) + " 10.0\", ";
	event.postData += "\"manufacturer\": \"unknown\", ";
	event.postData += "\"platform\": \"" + Lower(GetOS()) + "\", ";
	event.postData += "\"session_id\": \"" + Analytics::session_id + "\", ";// needs this added
	event.postData += "\"session_num\": " + Analytics::session_num /*+ "\", "*/;// needs this added
	//int version = System::GetVersion();
	//postData += "\"engine_version\": \"leadwerks " + Left(String(version),1) + "." + Right(String(version),2) + "\"";//no comma on last argument

	//Optional parameters
	if (params != "") event.postData += ", " + params;

	event.postData += "}]";

	//Construct authentication token
	unsigned char buff[32];
	hmac((unsigned char*)privatekey.c_str(), privatekey.length(), (unsigned char*)event.postData.c_str(), event.postData.length(), buff);
	std::string encoding = base64_encode((const unsigned char*)buff, 32);
	int len = sprintf(event.authHeader, "Authorization: %s", encoding.c_str());
#endif
	return event;
}

bool Analytics::QueueEvent(const std::string& category, const std::string& params)
{
#ifndef _WIN32
	return false;
#endif
	if (blocked == true || initialized == false) return false;
	queue.push_back(Analytics::CreateEvent(category, params));
	return true;
}

bool Analytics::SendEvent(const AnalyticsEvent& event, const long timeout)
{
#ifndef _WIN32
	return false;
#else
	struct MemoryStruct
	{
		char *memory;
		size_t size;
	};

	//Send
	CURL* curl = NULL;
	CURLcode res;
	struct curl_slist *headers = NULL;

	curl = curl_easy_init();

	//Disable printing
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

	//Construct header
	headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: text/xml");
	headers = curl_slist_append(headers, event.authHeader);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	//Set URL
	curl_easy_setopt(curl, CURLOPT_URL, (std::string("http://api.gameanalytics.com/v2/") + publickey + "/" + event.type).c_str());

	//Set body
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, event.postData.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)event.postData.length());

	//Timeout
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

	//Send
	curlstring = "";
	res = curl_easy_perform(curl);

	//Get the result
	if (res != CURLE_OK)
	{
		fprintf(stderr, "Error: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		return false;
	}
	else
	{
		if (event.type=="events")
		{
			if (curlstring != "{}") Print(curlstring);
		}
		/*
		struct MemoryStruct chunk;
		chunk.memory = (char*)malloc(1);
		chunk.size = 0;
		printf("%s\n", chunk.memory);*/
	}

	curl_easy_cleanup(curl);
	return true;
#endif
}