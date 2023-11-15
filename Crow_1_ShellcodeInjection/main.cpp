#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    printf("injecting into pid: %s...\n", argv[1]);
    int pid = atoi(argv[1]);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid); 
    if(!hProcess){
        printf("failed to open process. error: %ld", GetLastError());
    }

    return EXIT_SUCCESS; 
}