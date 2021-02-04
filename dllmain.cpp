#include "windows.h"
#include <stdio.h>

DWORD process_handle;

bool BCompare(const BYTE *data, const BYTE *pattern, const char *pattern_mask) {
    for (; *pattern_mask; ++pattern_mask, ++data, ++pattern) {
        if (*pattern_mask == 'x' && *data != *pattern) return false;
    }

	return (*pattern_mask) == NULL;
}

DWORD FindPattern(DWORD addr, DWORD data_len, const BYTE *pattern, const char *mask) {
	for (DWORD i = 0; i < data_len; i++)
		if (BCompare((BYTE*)addr + i, pattern, mask)) return addr + i;
	return 0;
}

void Start() {
    do {
        process_handle = (DWORD)GetModuleHandleA(NULL);
        Sleep(10);
    } while (!process_handle);

    printf("krkr: %x\n", process_handle);
    DWORD pattern_res = FindPattern(process_handle, 0x200000, (PBYTE) "\x55\x8B\xEC\x8B\x45\x08\xA3\x00\x00\x00\x00", "xxxxxxx????");

    printf("pattern_res: %x\n", pattern_res);
    if (pattern_res == 0) {
        printf("Cannot find pattern!");
        return;
    }

    DWORD filter_addr = *(DWORD*)(pattern_res + 7);
    printf("filter: %x\n", filter_addr);
    if (filter_addr == 0) {
        printf("Filter didn't set or is not loaded yet");
        return;
    }

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            AllocConsole();
            freopen("CON", "w", stdout);
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) Start, NULL, NULL, NULL);
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

