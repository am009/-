
#include <cstdio>
#include <string.h>
#include <Windows.h>
#include <TlHelp32.h>
#include "LinkListSimple.hpp"

//Done
//�޸Ĳ��뵽β�ڵ� ��
//��������ֹ��

// TO DO
//string
//��������
//�쳣����
//pharse��������  argument����-,option��-
//����ʱ��ctime
//��map��option �ͼ�ƺͰ����ַ���ӳ������.
//wchar����
//�ñ��鵱��ʾ��

typedef struct Processstru
{
	DWORD th32ProcessID;
	DWORD cntThreads;
	DWORD th32ParentProcessID;
	LONG pcPriClassBase;
	TCHAR szExeFile[MAX_PATH];
	Processstru(DWORD ath32ProcessID, DWORD acntThreads, DWORD ath32ParentProcessID, LONG apcPriClassBase, TCHAR* aszExeFile)
	{
		th32ProcessID = ath32ProcessID;
		cntThreads = acntThreads;
		pcPriClassBase = apcPriClassBase;
		strcpy(szExeFile, aszExeFile);
	}
	Processstru() { th32ProcessID = 0; cntThreads = 0; th32ParentProcessID = 0; pcPriClassBase = 0; }
	struct Processstru& operator= (const struct Processstru& b)
	{
		this->th32ProcessID = b.th32ProcessID;
		this->cntThreads = b.cntThreads;
		this->pcPriClassBase = b.pcPriClassBase;
		strcpy(this->szExeFile, b.szExeFile);
		return *this;
	}

} Proc;


DWORD queryPid(LinkList<Proc>& LL)
{
	char buf[50];
	scanf("%s", buf);
	//printf("#����query#");

	if (!strcmp(buf, "pid"))
	{
		DWORD pid;
		scanf("%lu", &pid);
		return pid;

	}
	else if ((!strcmp(buf, "num")) || (!strcmp(buf, "number")))
	{
		int ind;
		scanf("%d", &ind);
		if ((ind > LL.size()) || (ind < 0))
		{
			printf("��ֹ������Ŵ���\n");
			throw "��ֹ������Ŵ���";
		}
		//printf("#����query,%d:%lu#", ind, LL[LL.size() - ind - 1].th32ProcessID);
		return LL[LL.size() - ind - 1].th32ProcessID;
	}
	else if (!strcmp(buf, "name"))
	{
		char buf2[100];
		scanf("%s", buf2);
		int size = LL.size();
		LinkNode<Proc>* a = LL.first();
		while(a->succ != NULL)
		{
			if (!strcmp(a->data.szExeFile,buf2))
			{
				return a->data.th32ProcessID;
			}
			a = a->succ;
		}
		printf("����ƥ��ʧ��\n");
		throw "����ƥ��ʧ��";
	}
	else if (!strcmp(buf, "this"))
	{
		return GetCurrentProcessId();
	}
	else
	{
		puts("�������,Ӧ����pid/number/num");
		throw "��������������";
	}
}




//https://www.cnblogs.com/ini_always/archive/2011/06/05/2073173.html
BOOL SetPrivilege(
	HANDLE hToken,          // access token handle
	LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid))        // receives LUID of privilege
	{
		printf("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		printf("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

//https://www.cnblogs.com/autumoonchina/p/8315419.html
//void OutputSelfpath()
//{
//    char szFile[MAX_PATH] = {0};
//    GetModuleFileNameA(NULL, szFile, MAX_PATH);
//    printf("GetModuleFileName:\n\r%s\n\n", szFile);
//
//    memset(szFile, 0, MAX_PATH);
//
//    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
//    if (!hProcess)
//    {
//        printf("OpenProcess failed!\n");
//    }
//    else
//    {
//        DWORD dwRet = GetProcessImageFileNameA(hProcess, szFile, MAX_PATH);
//        if (dwRet)
//        {
//            printf("GetProcessImageFileName:\n\r%s\n\n", szFile);
//        }
//        else
//        {
//            printf("GetProcessImageFileName failed!\n");
//        }
//
//        DWORD dwSize = MAX_PATH;
//        if (QueryFullProcessImageNameA(hProcess, 0, szFile, &dwSize))
//        {
//            printf("QueryFullProcessImageName:\n\r%s\n\n", szFile);
//        }
//        else
//        {
//            printf("QueryFullProcessImageName failed\n");
//        }
//    }
//}

//�ҵ��������ڵĳ����Ŀ¼
//	char path[MAX_PATH+1];
bool find(DWORD ProcessId, LPSTR path, DWORD* size)
{
	HANDLE h_Process = OpenProcess(PROCESS_QUERY_INFORMATION , FALSE, ProcessId);//PROCESS_QUERY_INFORMATION | PROCESS_VM_READ
	if (h_Process == INVALID_HANDLE_VALUE)
	{
		printf("����·��:��ȡHANDLEʧ�ܣ�\n");
		::CloseHandle(h_Process);
		throw "Handle";
		return false;
	}
	//printf("����·��:��ȡHANDLE�ɹ���\n");
	if (!QueryFullProcessImageNameA(h_Process, 0, path, size))
	{
		printf("����·��:��ȡ·��ʧ�ܣ�\n");
		::CloseHandle(h_Process);
		throw "Handle";
		return false;
	}
	::CloseHandle(h_Process);
	return true;
}


int refresh (LinkList<Proc>& LL)
{
	LL.clear();
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};
 
	//��ʹ������ṹǰ�����������Ĵ�С
	pe32.dwSize = sizeof(PROCESSENTRY32);
 
	//��ϵͳ�����еĽ����ĸ�����
	hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("�������̿���:��ȡ���̿��վ��ʧ��!\n");
		throw "PHandle";
		return -1;
	}
 
	//�������̿��գ�������ʾÿ�����̵���Ϣ
	BOOL bRet = ::Process32First(hProcessSnap,&pe32);
	int ind = 0;
	while (bRet)
	{
		//���뵽���� 
		Proc pr = Proc(pe32.th32ProcessID, pe32.cntThreads, pe32.th32ParentProcessID, pe32.pcPriClassBase, pe32.szExeFile);
		LL.insertAsFirst(pr);//����ջ��,������˳�����ʾ���Ƿ���

		//std::cout << "�������ƣ�"<<pe32.szExeFile<<std::endl;   //����õ���Ӧ���ǿ��ַ�����%ls����%S,��Ȼ�޷�������ӡ...�ǲ���д����,������%s���ܴ�ӡ 
		char buf[25];
		sprintf(buf, "���:%d", ind++); printf("%-16s", buf);
		sprintf(buf, "ID: %lu", pe32.th32ProcessID); printf("%-16s", buf);
		printf("���ƣ�%s\n",pe32.szExeFile);
		bRet = ::Process32Next(hProcessSnap,&pe32);
	}
	//��Ҫ���������snapshot����
	::CloseHandle(hProcessSnap);
	return 0;
}

//��ֹ���� 
//https://www.write-bug.com/article/1868.html 
BOOL TerminateProcess(DWORD dwProcessId)
{
    // �򿪽���, ��ȡ���̾��
    HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE, dwProcessId);
    if (NULL == hProcess)
    {
        printf("��ֹ������:��ȡ���̾��ʧ��!\n");
		throw "Handle";
        return FALSE;
    }
    // ��������
    BOOL bRet = ::TerminateProcess(hProcess, 0);
    if (FALSE == bRet)
    {
        ::CloseHandle(hProcess);
        printf("��ֹ����ʧ��!\n");

        return FALSE;
    }
    // �رս��̾��
    ::CloseHandle(hProcess);
    return TRUE;
}



//��ͣ����
//http://www.cnblogs.com/gccbuaa/p/7222131.html
void StopProcess(DWORD dwProcessId) 
{
        //�����߳̿���
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessId);
        if ( hSnap == INVALID_HANDLE_VALUE )
        {
                printf("��ͣ������:��ȡ���̾��ʧ�ܣ�\n");
				throw "Handle";
                return ;
        }

        THREADENTRY32 Te32 = { 0 };
        Te32.dwSize = sizeof(THREADENTRY32);
        BOOL bRet = Thread32First(hSnap, &Te32);
    
        while ( bRet )
        {
                //�ƶ��߳�����
                 
                if ( Te32.th32OwnerProcessID == dwProcessId )
                {
                	//��ͣ�߳� 
                	 
                        HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);
            
                        SuspendThread(hThread);
            
                        CloseHandle(hThread);
                        
                }

                bRet = Thread32Next(hSnap, &Te32);
        }
}


//�ָ�����
//http://www.cnblogs.com/gccbuaa/p/7222131.html
void ResumeProcess(DWORD dwProcessId)
{
	
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessId);
        if ( hSnap == INVALID_HANDLE_VALUE )
        {
                printf("���ָ̻�:��ȡ���̾��ʧ�ܣ�\n");
				throw "Handle";
                return ;
        }
    
        THREADENTRY32 Te32 = { 0 };
        Te32.dwSize = sizeof(THREADENTRY32);
        BOOL bRet = Thread32First(hSnap, &Te32);
    
        while ( bRet )
        {
                if ( Te32.th32OwnerProcessID == dwProcessId )
                {
                        HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);
            
                        ResumeThread(hThread);
            
                        CloseHandle(hThread);
                }
        
                bRet = Thread32Next(hSnap, &Te32);
        }	
} 



int main(int argc, char* argv[])
{
	LinkList<Proc> ll = LinkList<Proc>();
	//OutputSelfpath();
	char* a = NULL;
	char buf[256] = { 0 };
	refresh(ll);
	puts("���̹����� 2019-4-17");
	puts("����help�鿴ʹ�÷���");
	puts("����ver��version�鿴��ʷ�����ݱ�");
	while (printf("-)"),scanf("%s", buf))
	{
		
		if ((!strcmp(buf, "exit"))|| (!strcmp(buf, "quit")))
		{
			break;
		}
		else if ((!strcmp(buf, "refresh")) || (!strcmp(buf, "list")))
		{
			try
			{
				refresh(ll);
			}
			catch(...)
			{
				continue;
			}
		}
		else if (!strcmp(buf, "help"))
		{
			puts("���̹����� 2019-04-17");
			puts("��������:");
			puts("help\t��ʾ����");
			puts("quit/exit\t�˳�����");
			puts("list/refresh\tˢ����ʾ����");
			puts("terminate/kill  pid/num/number [pidֵ�������]\t��ֹ����");
			puts("freeze/stop/resume  pid/num/number [pidֵ�������]\t��ͣ/�ָ����̵������߳�");
			puts("find  pid/num/number [pidֵ�������]\t���ҽ�������·��");
			puts("pid/num/number [pidֵ�������]�����滻Ϊthis��ʾ���̹������Լ��Ľ���");
		}
		else if ((!strcmp(buf, "terminate")) || (!strcmp(buf, "kill")))
		{
			try
			{
				TerminateProcess(queryPid(ll));
			}
			catch (...)
			{
				continue;
			}
		}
		else if ((!strcmp(buf, "stop")) || (!strcmp(buf, "freeze")))
		{
			try
			{
				StopProcess(queryPid(ll));
			}
			catch (...)
			{
				continue;
			}
		}
		else if (!strcmp(buf, "resume"))
		{
			try
			{
				ResumeProcess(queryPid(ll));
			}
			catch (...)
			{
				continue;
			}
		}
		else if (!strcmp(buf, "find"))
		{
			DWORD size = MAX_PATH;
			char path[MAX_PATH + 1] = { 0 };
			try
			{
				find(queryPid(ll), path, &size);

			}
			catch (...)
			{
				continue;
			}
			printf("%s\n", path);
		}
		else if ((!strcmp(buf, "version")) || (!strcmp(buf, "ver")) )
		{
			puts("2019-04-17 ���̹�������һ��д�� ");
			puts("ʵ����refresh,find,resume,stop,������SetPrivilege����");
			puts("���¿L����,������������ʾ��,refresh���ӱ���list,�������������½��̵Ĺ���");
			puts("������ʾ����>��Ϊ-)");
			puts("������smile,suiside����");
			//puts("");
		}
		else if (!strcmp(buf, "smile"))
		{
			puts("(*^_^*)");
			//puts("");
		}
		else if (!strcmp(buf, "suiside"))
		{
			TerminateProcess(GetCurrentProcessId());
			//puts("");
		}
		else
		{
			puts("δ֪����,����������,������help.");
			continue;
		}
	}


	return 0;
}

/*
typedef struct tagPROCESSENTRY32
{
    DWORD dwSize; // ����ṹ�ĳ��� If you do not initialize dwSize, Process32First fails.
    DWORD cntUsage; //�˽��̵����ü��� �����Ա�Ѿ����ٱ�ʹ�ã���������Ϊ�㡣
    DWORD th32ProcessID;//����ID=process identifier=PIDThe process identifier.
    ULONG_PTR th32DefaultHeapID;//����Ĭ�϶� �����Ա�Ѿ����ٱ�ʹ�ã���������Ϊ�㡣
    DWORD th32ModuleID;//����ģ��ID �����Ա�Ѿ����ٱ�ʹ�ã���������Ϊ�㡣
    DWORD cntThreads;//�����Աִ���߳̿�ʼ�Ľ��̡�
    DWORD th32ParentProcessID;//�����̵�ID��The identifier of the process that created this process
    LONG pcPriClassBase;//�߳�����Ȩ
    DWORD dwFlags;//�����Ա�Ѿ����ٱ�ʹ�ã���������Ϊ�㡣
    TCHAR szExeFile[MAX_PATH];//���̵Ŀ�ִ���ļ����ơ�Ҫ��ÿ�ִ���ļ�������·����Ӧ����Module32First�������ټ���䷵�ص�MODULEENTRY32�ṹ��szExePath��Ա�����ǣ���������ý�����һ��64λ�������������QueryFullProcessImageName����ȥ��ȡ64λ���̵Ŀ�ִ���ļ�����·������
} PROCESSENTRY32, *PPROCESSENTRY32;
*/
