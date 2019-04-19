
#include <cstdio>
#include <string.h>
#include <Windows.h>
#include <TlHelp32.h>
#include "LinkListSimple.hpp"

//Done
//修改插入到尾节点 ×
//按名字终止√

// TO DO
//string
//启动进程
//异常处理
//pharse解析命令  argument不带-,option带-
//快照时间ctime
//用map把option 和简称和帮助字符串映射起来.
//wchar兼容
//用表情当提示符

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
	//printf("#进入query#");

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
			printf("终止进程序号错误\n");
			throw "终止进程序号错误";
		}
		//printf("#返回query,%d:%lu#", ind, LL[LL.size() - ind - 1].th32ProcessID);
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
		printf("名字匹配失败\n");
		throw "名字匹配失败";
	}
	else if (!strcmp(buf, "this"))
	{
		return GetCurrentProcessId();
	}
	else
	{
		puts("命令错误,应该是pid/number/num");
		throw "进程序号命令错误";
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

//找到进程所在的程序的目录
//	char path[MAX_PATH+1];
bool find(DWORD ProcessId, LPSTR path, DWORD* size)
{
	HANDLE h_Process = OpenProcess(PROCESS_QUERY_INFORMATION , FALSE, ProcessId);//PROCESS_QUERY_INFORMATION | PROCESS_VM_READ
	if (h_Process == INVALID_HANDLE_VALUE)
	{
		printf("查找路径:获取HANDLE失败！\n");
		::CloseHandle(h_Process);
		throw "Handle";
		return false;
	}
	//printf("查找路径:获取HANDLE成功！\n");
	if (!QueryFullProcessImageNameA(h_Process, 0, path, size))
	{
		printf("查找路径:获取路径失败！\n");
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
 
	//在使用这个结构前，先设置它的大小
	pe32.dwSize = sizeof(PROCESSENTRY32);
 
	//给系统内所有的进程拍个快照
	hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("创建进程快照:获取进程快照句柄失败!\n");
		throw "PHandle";
		return -1;
	}
 
	//遍历进程快照，轮流显示每个进程的信息
	BOOL bRet = ::Process32First(hProcessSnap,&pe32);
	int ind = 0;
	while (bRet)
	{
		//插入到链表 
		Proc pr = Proc(pe32.th32ProcessID, pe32.cntThreads, pe32.th32ParentProcessID, pe32.pcPriClassBase, pe32.szExeFile);
		LL.insertAsFirst(pr);//链表当栈用,存贮的顺序和显示的是反的

		//std::cout << "进程名称："<<pe32.szExeFile<<std::endl;   //这里得到的应该是宽字符，用%ls或者%S,不然无法正常打印...是不是写错了,明明用%s才能打印 
		char buf[25];
		sprintf(buf, "序号:%d", ind++); printf("%-16s", buf);
		sprintf(buf, "ID: %lu", pe32.th32ProcessID); printf("%-16s", buf);
		printf("名称：%s\n",pe32.szExeFile);
		bRet = ::Process32Next(hProcessSnap,&pe32);
	}
	//不要忘记清除掉snapshot对象
	::CloseHandle(hProcessSnap);
	return 0;
}

//终止进程 
//https://www.write-bug.com/article/1868.html 
BOOL TerminateProcess(DWORD dwProcessId)
{
    // 打开进程, 获取进程句柄
    HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE, dwProcessId);
    if (NULL == hProcess)
    {
        printf("终止进程中:获取进程句柄失败!\n");
		throw "Handle";
        return FALSE;
    }
    // 结束进程
    BOOL bRet = ::TerminateProcess(hProcess, 0);
    if (FALSE == bRet)
    {
        ::CloseHandle(hProcess);
        printf("终止进程失败!\n");

        return FALSE;
    }
    // 关闭进程句柄
    ::CloseHandle(hProcess);
    return TRUE;
}



//暂停进程
//http://www.cnblogs.com/gccbuaa/p/7222131.html
void StopProcess(DWORD dwProcessId) 
{
        //创建线程快照
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessId);
        if ( hSnap == INVALID_HANDLE_VALUE )
        {
                printf("暂停进程中:获取进程句柄失败！\n");
				throw "Handle";
                return ;
        }

        THREADENTRY32 Te32 = { 0 };
        Te32.dwSize = sizeof(THREADENTRY32);
        BOOL bRet = Thread32First(hSnap, &Te32);
    
        while ( bRet )
        {
                //推断线程所属
                 
                if ( Te32.th32OwnerProcessID == dwProcessId )
                {
                	//暂停线程 
                	 
                        HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);
            
                        SuspendThread(hThread);
            
                        CloseHandle(hThread);
                        
                }

                bRet = Thread32Next(hSnap, &Te32);
        }
}


//恢复进程
//http://www.cnblogs.com/gccbuaa/p/7222131.html
void ResumeProcess(DWORD dwProcessId)
{
	
        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwProcessId);
        if ( hSnap == INVALID_HANDLE_VALUE )
        {
                printf("进程恢复:获取进程句柄失败！\n");
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
	puts("进程管理器 2019-4-17");
	puts("输入help查看使用方法");
	puts("输入ver或version查看历史功能演变");
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
			puts("进程管理器 2019-04-17");
			puts("可用命令:");
			puts("help\t显示帮助");
			puts("quit/exit\t退出程序");
			puts("list/refresh\t刷新显示进程");
			puts("terminate/kill  pid/num/number [pid值或者序号]\t终止进程");
			puts("freeze/stop/resume  pid/num/number [pid值或者序号]\t暂停/恢复进程的所有线程");
			puts("find  pid/num/number [pid值或者序号]\t查找进程所在路径");
			puts("pid/num/number [pid值或者序号]可以替换为this表示进程管理器自己的进程");
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
			puts("2019-04-17 进程管理器第一次写成 ");
			puts("实现了refresh,find,resume,stop,保留了SetPrivilege函数");
			puts("和穆柯橪交流,增加了命令提示符,refresh增加别名list,考虑增加运行新进程的功能");
			puts("命令提示符由>改为-)");
			puts("加入了smile,suiside命令");
			//puts("");
		}
		else if (!strcmp(buf, "smile"))
		{
			puts("(*^_^*)");
			//puts("");
		}
		else if (!strcmp(buf, "suicide"))
		{
			TerminateProcess(GetCurrentProcessId());
			//puts("");
		}
		else
		{
			puts("未知命令,请重新输入,或输入help.");
			continue;
		}
	}


	return 0;
}

/*
typedef struct tagPROCESSENTRY32
{
    DWORD dwSize; // 这个结构的长度 If you do not initialize dwSize, Process32First fails.
    DWORD cntUsage; //此进程的引用计数 这个成员已经不再被使用，总是设置为零。
    DWORD th32ProcessID;//进程ID=process identifier=PIDThe process identifier.
    ULONG_PTR th32DefaultHeapID;//进程默认堆 这个成员已经不再被使用，总是设置为零。
    DWORD th32ModuleID;//进程模块ID 这个成员已经不再被使用，总是设置为零。
    DWORD cntThreads;//这个成员执行线程开始的进程。
    DWORD th32ParentProcessID;//父进程的ID）The identifier of the process that created this process
    LONG pcPriClassBase;//线程优先权
    DWORD dwFlags;//这个成员已经不再被使用，总是设置为零。
    TCHAR szExeFile[MAX_PATH];//进程的可执行文件名称。要获得可执行文件的完整路径，应调用Module32First函数，再检查其返回的MODULEENTRY32结构的szExePath成员。但是，如果被调用进程是一个64位程序，您必须调用QueryFullProcessImageName函数去获取64位进程的可执行文件完整路径名。
} PROCESSENTRY32, *PPROCESSENTRY32;
*/
