#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        puts("usage: handle.exe <PID>");
        return EXIT_FAILURE;
    }
    
    DWORD PID = atoi(argv[1]);
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    printf("got a handle to the process: 0x%p\n", hProcess);
    puts("press enter to exit");
    getchar();
    CloseHandle(hProcess);
    return EXIT_SUCCESS;

}