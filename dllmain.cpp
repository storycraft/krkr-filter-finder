#include "windows.h"
#include <stdio.h>

#pragma pack(push, 4)
struct tTVPXP3ExtractionFilterInfo
{
    const unsigned __int32 size_of_self; // structure size of tTVPXP3ExtractionFilterInfo itself
    const unsigned __int64 offset; // offset of the buffer data in uncompressed stream position
    void *buffer; // target data buffer
    const unsigned __int32 buffer_size; // buffer size in bytes pointed by "Buffer"
    const unsigned __int32 file_hash; // hash value of the file (since inteface v2)

    tTVPXP3ExtractionFilterInfo(unsigned long long offset, void* buffer, unsigned int buffersize, unsigned int filehash) :
        offset(offset), buffer(buffer), buffer_size(buffersize),
        file_hash(filehash),
        size_of_self(sizeof(tTVPXP3ExtractionFilterInfo)) {

    }
};
#pragma pack(pop)

typedef void (_stdcall *FilterFunc)(tTVPXP3ExtractionFilterInfo* info);

DWORD process_handle;
FilterFunc original_filter;

void _stdcall HookedFilter(tTVPXP3ExtractionFilterInfo *info) {
    printf("self: %ld\noffset: %ld\nbuffer: %x\nbuffer_size: %d\nfile_hash: %x\n\n", info -> size_of_self, info -> offset, info -> buffer, info -> buffer_size, info -> file_hash);
    original_filter(info);
}

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

    DWORD* filter_ptr = *(DWORD**)(pattern_res + 7);
    printf("filter: %x\n", filter_ptr);
    if (filter_ptr == 0) {
        printf("Filter didn't set or is not loaded yet");
        return;
    }
    original_filter = (FilterFunc) *filter_ptr;
    printf("original: %x\n", original_filter);

    DWORD buf = (DWORD) HookedFilter;

    BOOL result = WriteProcessMemory(GetCurrentProcess(), filter_ptr, &buf, 4, NULL);
    if (!result) {
        printf("Cannot hook filter");
        return;
    }
    printf("Hooked filter");

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

