#include "Network.h"

bool Crysus::Network::IsInternetAvailable(void)
{
    return InternetCheckConnection("http://www.crysus.net/", FLAG_ICC_FORCE_CONNECTION, 0) == TRUE;
}

bool Crysus::Network::UpdateAvailable(DWORD nVersion)
{
	TCHAR *szHeaders = "Content-Type: text/html";
	PCTSTR rgpszAcceptTypes[] = {"text/*", NULL};
	CHAR pVersionBuf[READ_PAGE_BUFFER_SIZE+1] = {};
	DWORD dwRead = 0;

	HINTERNET hSession = InternetOpen("EterNexus Agent", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if(hSession == INVALID_HANDLE_VALUE)
		return false;
	
	HINTERNET hConnection = InternetConnect(hSession, "crysus.net", INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
	if(hConnection == INVALID_HANDLE_VALUE)
		return false;

	HINTERNET hRequest = HttpOpenRequest(hConnection, "GET", "x/eternexus/version", NULL, NULL, rgpszAcceptTypes, INTERNET_FLAG_RELOAD, 0);
	if(hRequest == INVALID_HANDLE_VALUE)
		return false;

	BOOL bRequestSent = HttpSendRequest(hRequest, szHeaders, strlen(szHeaders), NULL, 0);
	if(bRequestSent == FALSE)
		return false;

	BOOL bReadPage = InternetReadFile(hRequest, pVersionBuf, READ_PAGE_BUFFER_SIZE, &dwRead);
	if(bReadPage == FALSE || dwRead == 0)
		return false;

	InternetCloseHandle(hSession);
    InternetCloseHandle(hConnection);
    InternetCloseHandle(hRequest);

	if(!isdigit(pVersionBuf[0]))
		return false;

	return nVersion < atoi(pVersionBuf);
}