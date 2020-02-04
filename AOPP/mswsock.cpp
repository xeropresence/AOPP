
#include "mswsock.h"
#include <windows.h>

HINSTANCE hLThis = 0;
FARPROC p[63];
HINSTANCE hL = 0;
bool mswsock::Dettach()
{
	FreeLibrary(hL);
	return 1;
}

bool mswsock::Attach()
{

	char lib_path[MAX_PATH];
	GetSystemDirectoryA(lib_path, MAX_PATH);
	strcat_s(lib_path, "\\mswsock.dll");
	const auto hL = LoadLibraryA(lib_path);

	if (!hL) 
		return false;
	
	p[0] = GetProcAddress(hL, "AcceptEx");
	p[1] = GetProcAddress(hL, "EnumProtocolsA");
	p[2] = GetProcAddress(hL, "EnumProtocolsW");
	p[3] = GetProcAddress(hL, "GetAcceptExSockaddrs");
	p[4] = GetProcAddress(hL, "GetAddressByNameA");
	p[5] = GetProcAddress(hL, "GetAddressByNameW");
	p[6] = GetProcAddress(hL, "GetNameByTypeA");
	p[7] = GetProcAddress(hL, "GetNameByTypeW");
	p[8] = GetProcAddress(hL, "GetServiceA");
	p[9] = GetProcAddress(hL, "GetServiceW");
	p[10] = GetProcAddress(hL, "GetSocketErrorMessageW");
	p[11] = GetProcAddress(hL, "GetTypeByNameA");
	p[12] = GetProcAddress(hL, "GetTypeByNameW");
	p[13] = GetProcAddress(hL, "MigrateWinsockConfiguration");
	p[14] = GetProcAddress(hL, "MigrateWinsockConfigurationEx");
	p[15] = GetProcAddress(hL, "NPLoadNameSpaces");
	p[16] = GetProcAddress(hL, "NSPStartup");
	p[17] = GetProcAddress(hL, "SetServiceA");
	p[18] = GetProcAddress(hL, "SetServiceW");
	p[19] = GetProcAddress(hL, "StartWsdpService");
	p[20] = GetProcAddress(hL, "StopWsdpService");
	p[21] = GetProcAddress(hL, "Tcpip4_WSHAddressToString");
	p[22] = GetProcAddress(hL, "Tcpip4_WSHEnumProtocols");
	p[23] = GetProcAddress(hL, "Tcpip4_WSHGetBroadcastSockaddr");
	p[24] = GetProcAddress(hL, "Tcpip4_WSHGetProviderGuid");
	p[25] = GetProcAddress(hL, "Tcpip4_WSHGetSockaddrType");
	p[26] = GetProcAddress(hL, "Tcpip4_WSHGetSocketInformation");
	p[27] = GetProcAddress(hL, "Tcpip4_WSHGetWSAProtocolInfo");
	p[28] = GetProcAddress(hL, "Tcpip4_WSHGetWildcardSockaddr");
	p[29] = GetProcAddress(hL, "Tcpip4_WSHGetWinsockMapping");
	p[30] = GetProcAddress(hL, "Tcpip4_WSHIoctl");
	p[31] = GetProcAddress(hL, "Tcpip4_WSHJoinLeaf");
	p[32] = GetProcAddress(hL, "Tcpip4_WSHNotify");
	p[33] = GetProcAddress(hL, "Tcpip4_WSHOpenSocket");
	p[34] = GetProcAddress(hL, "Tcpip4_WSHOpenSocket2");
	p[35] = GetProcAddress(hL, "Tcpip4_WSHSetSocketInformation");
	p[36] = GetProcAddress(hL, "Tcpip4_WSHStringToAddress");
	p[37] = GetProcAddress(hL, "Tcpip6_WSHAddressToString");
	p[38] = GetProcAddress(hL, "Tcpip6_WSHEnumProtocols");
	p[39] = GetProcAddress(hL, "Tcpip6_WSHGetProviderGuid");
	p[40] = GetProcAddress(hL, "Tcpip6_WSHGetSockaddrType");
	p[41] = GetProcAddress(hL, "Tcpip6_WSHGetSocketInformation");
	p[42] = GetProcAddress(hL, "Tcpip6_WSHGetWSAProtocolInfo");
	p[43] = GetProcAddress(hL, "Tcpip6_WSHGetWildcardSockaddr");
	p[44] = GetProcAddress(hL, "Tcpip6_WSHGetWinsockMapping");
	p[45] = GetProcAddress(hL, "Tcpip6_WSHIoctl");
	p[46] = GetProcAddress(hL, "Tcpip6_WSHJoinLeaf");
	p[47] = GetProcAddress(hL, "Tcpip6_WSHNotify");
	p[48] = GetProcAddress(hL, "Tcpip6_WSHOpenSocket");
	p[49] = GetProcAddress(hL, "Tcpip6_WSHOpenSocket2");
	p[50] = GetProcAddress(hL, "Tcpip6_WSHSetSocketInformation");
	p[51] = GetProcAddress(hL, "Tcpip6_WSHStringToAddress");
	p[52] = GetProcAddress(hL, "TransmitFile");
	p[53] = GetProcAddress(hL, "WSARecvEx");
	p[54] = GetProcAddress(hL, "WSPStartup");
	p[55] = GetProcAddress(hL, "dn_expand");
	p[56] = GetProcAddress(hL, "getnetbyname");
	p[57] = GetProcAddress(hL, "inet_network");
	p[58] = GetProcAddress(hL, "rcmd");
	p[59] = GetProcAddress(hL, "rexec");
	p[60] = GetProcAddress(hL, "rresvport");
	p[61] = GetProcAddress(hL, "s_perror");
	p[62] = GetProcAddress(hL, "sethostname");

	return 1;
}



extern "C"
{
	void PROXY_AcceptEx() {
		__asm
		{			jmp p[1 * 4]
		}
	}
	void PROXY_EnumProtocolsA() {
		__asm
		{			jmp p[2 * 4]
		}
	}
	void PROXY_EnumProtocolsW() {
		__asm
		{			jmp p[3 * 4]
		}
	}
	void PROXY_GetAcceptExSockaddrs() {
		__asm
		{			jmp p[4 * 4]
		}
	}
	void PROXY_GetAddressByNameA() {
		__asm
		{			jmp p[5 * 4]
		}
	}
	void PROXY_GetAddressByNameW() {
		__asm
		{			jmp p[6 * 4]
		}
	}
	void PROXY_GetNameByTypeA() {
		__asm
		{			jmp p[7 * 4]
		}
	}
	void PROXY_GetNameByTypeW() {
		__asm
		{			jmp p[8 * 4]
		}
	}
	void PROXY_GetServiceA() {
		__asm
		{			jmp p[9 * 4]
		}
	}
	void PROXY_GetServiceW() {
		__asm
		{			jmp p[10 * 4]
		}
	}
	void PROXY_GetSocketErrorMessageW() {
		__asm
		{			jmp p[11 * 4]
		}
	}
	void PROXY_GetTypeByNameA() {
		__asm
		{			jmp p[12 * 4]
		}
	}
	void PROXY_GetTypeByNameW() {
		__asm
		{			jmp p[13 * 4]
		}
	}
	void PROXY_MigrateWinsockConfiguration() {
		__asm
		{			jmp p[14 * 4]
		}
	}
	void PROXY_MigrateWinsockConfigurationEx() {
		__asm
		{			jmp p[15 * 4]
		}
	}
	void PROXY_NPLoadNameSpaces() {
		__asm
		{			jmp p[16 * 4]
		}
	}
	void PROXY_NSPStartup() {
		__asm
		{			jmp p[17 * 4]
		}
	}
	void PROXY_SetServiceA() {
		__asm
		{			jmp p[18 * 4]
		}
	}
	void PROXY_SetServiceW() {
		__asm
		{			jmp p[19 * 4]
		}
	}
	void PROXY_StartWsdpService() {
		__asm
		{			jmp p[20 * 4]
		}
	}
	void PROXY_StopWsdpService() {
		__asm
		{			jmp p[21 * 4]
		}
	}
	void PROXY_Tcpip4_WSHAddressToString() {
		__asm
		{			jmp p[22 * 4]
		}
	}
	void PROXY_Tcpip4_WSHEnumProtocols() {
		__asm
		{			jmp p[23 * 4]
		}
	}
	void PROXY_Tcpip4_WSHGetBroadcastSockaddr() {
		__asm
		{			jmp p[24 * 4]
		}
	}
	void PROXY_Tcpip4_WSHGetProviderGuid() {
		__asm
		{			jmp p[25 * 4]
		}
	}
	void PROXY_Tcpip4_WSHGetSockaddrType() {
		__asm
		{			jmp p[26 * 4]
		}
	}
	void PROXY_Tcpip4_WSHGetSocketInformation() {
		__asm
		{			jmp p[27 * 4]
		}
	}
	void PROXY_Tcpip4_WSHGetWSAProtocolInfo() {
		__asm
		{			jmp p[28 * 4]
		}
	}
	void PROXY_Tcpip4_WSHGetWildcardSockaddr() {
		__asm
		{			jmp p[29 * 4]
		}
	}
	void PROXY_Tcpip4_WSHGetWinsockMapping() {
		__asm
		{			jmp p[30 * 4]
		}
	}
	void PROXY_Tcpip4_WSHIoctl() {
		__asm
		{			jmp p[31 * 4]
		}
	}
	void PROXY_Tcpip4_WSHJoinLeaf() {
		__asm
		{			jmp p[32 * 4]
		}
	}
	void PROXY_Tcpip4_WSHNotify() {
		__asm
		{			jmp p[33 * 4]
		}
	}
	void PROXY_Tcpip4_WSHOpenSocket() {
		__asm
		{			jmp p[34 * 4]
		}
	}
	void PROXY_Tcpip4_WSHOpenSocket2() {
		__asm
		{			jmp p[35 * 4]
		}
	}
	void PROXY_Tcpip4_WSHSetSocketInformation() {
		__asm
		{			jmp p[36 * 4]
		}
	}
	void PROXY_Tcpip4_WSHStringToAddress() {
		__asm
		{			jmp p[37 * 4]
		}
	}
	void PROXY_Tcpip6_WSHAddressToString() {
		__asm
		{			jmp p[38 * 4]
		}
	}
	void PROXY_Tcpip6_WSHEnumProtocols() {
		__asm
		{			jmp p[39 * 4]
		}
	}
	void PROXY_Tcpip6_WSHGetProviderGuid() {
		__asm
		{			jmp p[40 * 4]
		}
	}
	void PROXY_Tcpip6_WSHGetSockaddrType() {
		__asm
		{			jmp p[41 * 4]
		}
	}
	void PROXY_Tcpip6_WSHGetSocketInformation() {
		__asm
		{			jmp p[42 * 4]
		}
	}
	void PROXY_Tcpip6_WSHGetWSAProtocolInfo() {
		__asm
		{			jmp p[43 * 4]
		}
	}
	void PROXY_Tcpip6_WSHGetWildcardSockaddr() {
		__asm
		{			jmp p[44 * 4]
		}
	}
	void PROXY_Tcpip6_WSHGetWinsockMapping() {
		__asm
		{			jmp p[45 * 4]
		}
	}
	void PROXY_Tcpip6_WSHIoctl() {
		__asm
		{			jmp p[46 * 4]
		}
	}
	void PROXY_Tcpip6_WSHJoinLeaf() {
		__asm
		{			jmp p[47 * 4]
		}
	}
	void PROXY_Tcpip6_WSHNotify() {
		__asm
		{			jmp p[48 * 4]
		}
	}
	void PROXY_Tcpip6_WSHOpenSocket() {
		__asm
		{			jmp p[49 * 4]
		}
	}
	void PROXY_Tcpip6_WSHOpenSocket2() {
		__asm
		{			jmp p[50 * 4]
		}
	}
	void PROXY_Tcpip6_WSHSetSocketInformation() {
		__asm
		{			jmp p[51 * 4]
		}
	}
	void PROXY_Tcpip6_WSHStringToAddress() {
		__asm
		{			jmp p[52 * 4]
		}
	}
	void PROXY_TransmitFile() {
		__asm
		{			jmp p[53 * 4]
		}
	}
	void PROXY_WSARecvEx() {
		__asm
		{			jmp p[54 * 4]
		}
	}
	void PROXY_WSPStartup() {
		__asm
		{			jmp p[55 * 4]
		}
	}
	void PROXY_dn_expand() {
		__asm
		{			jmp p[56 * 4]
		}
	}
	void PROXY_getnetbyname() {
		__asm
		{			jmp p[57 * 4]
		}
	}
	void PROXY_inet_network() {
		__asm
		{			jmp p[58 * 4]
		}
	}
	void PROXY_rcmd() {
		__asm
		{			jmp p[59 * 4]
		}
	}
	void PROXY_rexec() {
		__asm
		{			jmp p[60 * 4]
		}
	}
	void PROXY_rresvport() {
		__asm
		{			jmp p[61 * 4]
		}
	}
	void PROXY_s_perror() {
		__asm
		{			jmp p[62 * 4]
		}
	}
	void PROXY_sethostname() {
		__asm
		{			jmp p[63 * 4]
		}
	}
}

