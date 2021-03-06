#include <winsock2.h>
#include <iphlpapi.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

string ProcessIdToName(DWORD processId)
{
    std::string ret;
    HANDLE handle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        processId
    );
    if (handle)
    {
        DWORD buffSize = 1024;
        CHAR buffer[1024];
        if (QueryFullProcessImageNameA(handle, 0, buffer, &buffSize))
        {
            ret = buffer;
        }
        else
        {
            printf("Error GetModuleBaseNameA : %lu", GetLastError());
        }
        CloseHandle(handle);
    }
    else
    {
        printf("Error OpenProcess : %lu", GetLastError());
    }
    return ret;
}

boolean ProcessCheck(DWORD processId)
{
    std::string ret;
    HANDLE handle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        processId
    );
    if (handle)
    {
        DWORD buffSize = 1024;
        CHAR buffer[1024];
        if (QueryFullProcessImageNameA(handle, 0, buffer, &buffSize))
        {
            CloseHandle(handle);
            return true;
        }
        else
        {
            CloseHandle(handle);
            return false;
        }
    }
    else
    {
        return false;
    }
    return false;
}


int main()
{

    vector<unsigned char> buffer;
    DWORD dwSize = sizeof(MIB_TCPTABLE_OWNER_PID);
    DWORD dwRetValue = 0;

    do
    {
        buffer.resize(dwSize, 0);
        dwRetValue = GetExtendedTcpTable(buffer.data(), &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);

    } while (dwRetValue == ERROR_INSUFFICIENT_BUFFER);

    if (dwRetValue == ERROR_SUCCESS)
    {

        PMIB_TCPTABLE_OWNER_PID ptTable = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(buffer.data());

        cout << "Number of Entries: " << ptTable->dwNumEntries << endl << endl;


        for (DWORD i = 0; i < ptTable->dwNumEntries; i++)
        {
            if (ProcessCheck(ptTable->table[i].dwOwningPid)) {

                cout << "PID: " << ptTable->table[i].dwOwningPid << endl;
                cout << "Name: " << ProcessIdToName(ptTable->table[i].dwOwningPid) << endl;
                cout << "State: " << ptTable->table[i].dwState << endl;

                cout << "Local: "
                    << (ptTable->table[i].dwLocalAddr & 0xFF)
                    << "."
                    << ((ptTable->table[i].dwLocalAddr >> 8) & 0xFF)
                    << "."
                    << ((ptTable->table[i].dwLocalAddr >> 16) & 0xFF)
                    << "."
                    << ((ptTable->table[i].dwLocalAddr >> 24) & 0xFF)
                    << ":"
                    << htons((unsigned short)ptTable->table[i].dwLocalPort)
                    << endl;

                cout << "Remote: "
                    << (ptTable->table[i].dwRemoteAddr & 0xFF)
                    << "."
                    << ((ptTable->table[i].dwRemoteAddr >> 8) & 0xFF)
                    << "."
                    << ((ptTable->table[i].dwRemoteAddr >> 16) & 0xFF)
                    << "."
                    << ((ptTable->table[i].dwRemoteAddr >> 24) & 0xFF)
                    << ":"
                    << htons((unsigned short)ptTable->table[i].dwRemotePort)
                    << endl;

                cout << endl;
            }
        }

    }
    else
    {

    }

    cin.get();

}