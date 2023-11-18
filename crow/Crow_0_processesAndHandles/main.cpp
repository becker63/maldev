#include <stdio.h>
#include <windows.h>
#include <string>

// crows original code example did not actually wait on the process to exit because often apps create a chain of processess, each of them need to be waited on. to do this I followed this blog post about jobs https://devblogs.microsoft.com/oldnewthing/20130405-00/?p=4743

// This code example creates a job, a object that allows us to monitor the state of groups or chains of processes. It then attaches that job objection to a completion port that allows and then attaches the process we create to that job.

JOBOBJECT_BASIC_PROCESS_ID_LIST getPids(HANDLE Job)
{
    // get number of processes
    JOBOBJECT_BASIC_ACCOUNTING_INFORMATION jobInfo;
    QueryInformationJobObject(Job, JobObjectBasicProcessIdList, &jobInfo, sizeof(jobInfo), nullptr);

    // get pids of processes
    JOBOBJECT_BASIC_PROCESS_ID_LIST pidArr;
    pidArr.NumberOfAssignedProcesses = jobInfo.TotalProcesses;
    QueryInformationJobObject(Job, JobObjectBasicProcessIdList, &pidArr, sizeof(pidArr), nullptr);
    return pidArr;
}

void printProcessJobInfo(HANDLE Job)
{

    JOBOBJECT_BASIC_PROCESS_ID_LIST pids = getPids(Job);
    for (DWORD i = 0; i < pids.NumberOfAssignedProcesses; i++)
    {
        ULONG_PTR processId = pids.ProcessIdList[i];
        printf("pid: %d\n", processId);
    }
}

int main(void)
{

    // create a job object to attach our process to
    HANDLE Job(CreateJobObject(nullptr, nullptr));
    if (!Job)
    {
        printf("CreateJobObject, error %ld\n", GetLastError());
        return EXIT_FAILURE;
    }

    // create a completion port
    HANDLE IOPort(CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1));
    if (!IOPort)
    {
        printf("CreateIoCompletionPort, error %ldd\n", GetLastError());
        return EXIT_FAILURE;
    }
    JOBOBJECT_ASSOCIATE_COMPLETION_PORT Port;
    Port.CompletionKey = Job;
    Port.CompletionPort = IOPort;
    if (!SetInformationJobObject(Job, JobObjectAssociateCompletionPortInformation, &Port, sizeof(Port)))
    {
        printf("SetInformation, error %ld\n", GetLastError());
        return EXIT_FAILURE;
    }

    // create process that will start our processes
    STARTUPINFOW si = {0};
    PROCESS_INFORMATION pi = {0};
    if (!CreateProcessW(L"C:\\Windows\\System32\\notepad.exe", NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_BREAKAWAY_FROM_JOB, NULL, NULL, &si, &pi))
    {
        printf("(-) failed to create process, error: %ld", GetLastError());
        return EXIT_FAILURE;
    }

    DWORD TID = pi.dwThreadId;
    DWORD PID = pi.dwProcessId;
    HANDLE hThread = pi.hThread;
    HANDLE hProcess = pi.hProcess;
    DWORD CompletionCode;
    ULONG_PTR CompletionKey;
    LPOVERLAPPED Overlapped;

    // main
    printf("(+) got handle to start process, assigning it to job and getting it started\n");
    if (!AssignProcessToJobObject(Job, hProcess))
    {
        printf("job assign, error %ld\n", GetLastError());
        return 0;
    }

    ResumeThread(hThread);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    while (GetQueuedCompletionStatus(IOPort, &CompletionCode, &CompletionKey, &Overlapped, INFINITE) && !((HANDLE)CompletionKey == Job && CompletionCode == JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO))
    {
        printf("job is running printing information.. \n");
        printProcessJobInfo(Job);
        // todo get info about child processes in job
        //  use QueryInformationJobObject
    }

    printf("(+) finished! exiting...");

    return EXIT_SUCCESS;
}