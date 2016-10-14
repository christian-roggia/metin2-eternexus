#include <Windows.h>
#include <iphlpapi.h>
#include <wininet.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wininet.lib")

#define READ_PAGE_BUFFER_SIZE 1024

namespace Crysus { namespace Network {
	bool IsInternetAvailable(void);
	bool UpdateAvailable(DWORD szVersion);

	bool SendData(char *m_URL, char *m_content, INT32 m_size);
	bool RecvData(char *m_URL, char *&m_content, INT32 &m_size);
}}