#include "HTTP.h"

#include <Windows.h>
#include <WinInet.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#pragma comment(lib, "wininet.lib")

struct sHINTERNET
{
	HINTERNET hInet;

	sHINTERNET(HINTERNET AInet = 0) : hInet(AInet) {}
	~sHINTERNET() { InternetCloseHandle(hInet); }

	operator HINTERNET() { return hInet; }
	bool operator!() const { return !hInet; }
};

std::string getRemoteFile(std::string hostName, std::string uri) {
	std::wstring userAgent = L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.82 Safari/537.36";

	HINTERNET hInternet = InternetOpenW(userAgent.c_str(), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	if (hInternet == NULL) {
		return "";
	}
	else
	{
		std::wstring widestr;
		for (int i = 0; i < hostName.length(); ++i)
		{
			widestr += wchar_t(hostName[i]);
		}
		const wchar_t* site_name = widestr.c_str();

		std::wstring widestr2;
		for (int i = 0; i < uri.length(); ++i)
		{
			widestr2 += wchar_t(uri[i]);
		}
		const wchar_t* site_param = widestr2.c_str();

		HINTERNET hConnect = InternetConnectW(hInternet, site_name, 443, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);

		if (hConnect == NULL)
		{
			return "InternetConnectW failed(hConnect == NULL): " + GetLastError();
		}
		else
		{
			const wchar_t* parrAcceptTypes[] = { L"text/*", NULL }; // accepted types. We'll choose text.

			HINTERNET hRequest = HttpOpenRequestW(hConnect, L"GET", site_param, NULL, NULL, parrAcceptTypes, INTERNET_FLAG_SECURE, 0);

			if (hRequest == NULL)
			{
				return "HttpOpenRequestW failed(hRequest == NULL): " + GetLastError();
			}
			else
			{
				BOOL bRequestSent = HttpSendRequestW(hRequest, NULL, 0, NULL, 0);

				if (!bRequestSent)
				{
					return "!bRequestSent    HttpSendRequestW failed with error code " + GetLastError();
				}
				else
				{
					std::string strResponse;
					const int nBuffSize = 1024;
					char buff[nBuffSize];

					BOOL bKeepReading = true;
					DWORD dwBytesRead = -1;

					while (bKeepReading && dwBytesRead != 0)
					{
						bKeepReading = InternetReadFile(hRequest, buff, nBuffSize, &dwBytesRead);
						strResponse.append(buff, dwBytesRead);
					}
					return strResponse;
				}
				InternetCloseHandle(hRequest);
			}
			InternetCloseHandle(hConnect);
		}
		InternetCloseHandle(hInternet);
	}
}
