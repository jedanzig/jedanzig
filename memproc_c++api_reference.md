Skip to content
ufrisk
MemProcFS
Repository navigation
Code
Issues
6
 (6)
Pull requests
Agents
Actions
Projects
Wiki
Security
Insights
API_C
Ulf Frisk edited this page on Nov 26, 2024 · 38 revisions
Native C/C++ API
All functionality in the Memory Process File System is exported in a C/C++ API for use by developers. The header file is named: vmmdll.h which use vmm.dll / vmm.lib (Windows) or vmm.so (Linux).

It may also be interesting to look into the more basic API related to read/write physical memory exported by the LeechCore library.

The complete documentation is found in vmmdll.h. This wiki entry contains an overview of the C/C++ API.

Linux is supported on x64 and ARM64 in addition to Windows x64. Wide-Char *W versions of API functions are only supported on Windows while UTF-8 *U versions of API functions are supported on both Linux and Windows.

NB! As of version 5 MemProcFS support multiple analysis tasks in the same process. To support this the API has undergone substantial changes and are no longer compatible with version earlier versions.

Example:
An example file containing a lot of use cases are found in the file vmmdll_example.c in the vmmdll_example project in the visual studio solution.

Functionality:
Initialization API
After vmm.dll is loaded it has to be initialized.

Depending on whether it should be initialized from file, fpga or something else different VMMDLL_Initialize should be called with a different list of string parameters in the first argument. The arguments are the same as given as options when starting The Memory Process File System except for argv[0] which is recommended to set to blank.

The initialization function return a VMM_HANDLE upon success. The VMM_HANDLE should be used in subsequent API requests.

VMM_HANDLE VMMDLL_Initialize(_In_ DWORD argc, _In_ LPSTR argv[]);
VMM_HANDLE VMMDLL_InitializeEx(_In_ DWORD argc, _In_ LPSTR argv[], _Out_opt_ PPLC_CONFIG_ERRORINFO ppLcErrorInfo);
BOOL VMMDLL_InitializePlugins();
BOOL VMMDLL_InitializePlugins(_In_ VMM_HANDLE hVMM);
VOID VMMDLL_Close(_In_opt_ _Post_ptr_invalid_ VMM_HANDLE hVMM);
VOID VMMDLL_CloseAll();
SIZE_T VMMDLL_MemSize(_In_ PVOID pvMem);
VOID VMMDLL_MemFree(_Frees_ptr_opt_ PVOID pvMem);
Configuration API
BOOL VMMDLL_ConfigGet(_In_ VMM_HANDLE hVMM, _In_ ULONG64 fOption, _Out_ PULONG64 pqwValue);
BOOL VMMDLL_ConfigSet(_In_ VMM_HANDLE hVMM, _In_ ULONG64 fOption, _In_ ULONG64 qwValue);
File System API
The MemProcFS.exe file is just a wrapper around the API below:

BOOL VMMDLL_VfsListU(
    _In_ VMM_HANDLE hVMM, 
    _In_ LPSTR  uszPath,
    _Inout_ PVMMDLL_VFS_FILELIST2 pFileList
);

BOOL VMMDLL_VfsListW(
    _In_ VMM_HANDLE hVMM, 
    _In_ LPWSTR wszPath,
    _Inout_ PVMMDLL_VFS_FILELIST2 pFileList
);

NTSTATUS VMMDLL_VfsReadU(
    _In_ VMM_HANDLE hVMM, 
    _In_ LPSTR  uszFileName,
    _Out_writes_to_(cb, *pcbRead) PBYTE pb,
    _In_ DWORD cb,
    _Out_ PDWORD pcbRead,
    _In_ ULONG64 cbOffset
);

NTSTATUS VMMDLL_VfsReadW(
    _In_ VMM_HANDLE hVMM, 
    _In_ LPWSTR wszFileName,
    _Out_writes_to_(cb, *pcbRead) PBYTE pb,
    _In_ DWORD cb,
    _Out_ PDWORD pcbRead,
    _In_ ULONG64 cbOffset
);

NTSTATUS VMMDLL_VfsWriteU(
    _In_ VMM_HANDLE hVMM, 
    _In_ LPSTR  uszFileName,
    _In_reads_(cb) PBYTE pb,
    _In_ DWORD cb,
    _Out_ PDWORD pcbWrite,
    _In_ ULONG64 cbOffset
);

NTSTATUS VMMDLL_VfsWriteW(
    _In_ VMM_HANDLE hVMM, 
    _In_ LPWSTR wszFileName,
    _In_reads_(cb) PBYTE pb,
    _In_ DWORD cb,
    _Out_ PDWORD pcbWrite,
    _In_ ULONG64 cbOffset
);

NTSTATUS VMMDLL_UtilVfsReadFile_FromPBYTE(
    _In_ PBYTE pbFile,
    _In_ ULONG64 cbFile,
    _Out_writes_to_(cb, *pcbRead) PBYTE pb,
    _In_ DWORD cb,
    _Out_ PDWORD pcbRead,
    _In_ ULONG64 cbOffset
);

NTSTATUS VMMDLL_UtilVfsReadFile_FromQWORD(
    _In_ ULONG64 qwValue,
    _Out_writes_to_(cb, *pcbRead) PBYTE pb,
    _In_ DWORD cb,
    _Out_ PDWORD pcbRead,
    _In_ ULONG64 cbOffset,
    _In_ BOOL fPrefix
);

NTSTATUS VMMDLL_UtilVfsReadFile_FromDWORD(
    _In_ DWORD dwValue,
    _Out_writes_to_(cb, *pcbRead) PBYTE pb,
    _In_ DWORD cb,
    _Out_ PDWORD pcbRead,
    _In_ ULONG64 cbOffset,
    _In_ BOOL fPrefix
);

NTSTATUS VMMDLL_UtilVfsReadFile_FromBOOL(
    _In_ BOOL fValue,
    _Out_writes_to_(cb, *pcbRead) PBYTE pb,
    _In_ DWORD cb,
    _Out_ PDWORD pcbRead,
    _In_ ULONG64 cbOffset
);

NTSTATUS VMMDLL_UtilVfsWriteFile_BOOL(
    _Inout_ PBOOL pfTarget,
    _In_reads_(cb) PBYTE pb,
    _In_ DWORD cb,
    _Out_ PDWORD pcbWrite,
    _In_ ULONG64 cbOffset
);

NTSTATUS VMMDLL_UtilVfsWriteFile_DWORD(
    _Inout_ PDWORD pdwTarget,
    _In_reads_(cb) PBYTE pb,
    _In_ DWORD cb,
    _Out_ PDWORD pcbWrite,
    _In_ ULONG64 cbOffset,
    _In_ DWORD dwMinAllow
);
Memory Read/Write API:
Read and write both physical and virtual memory via the functions listed below. In most instances it's possible to specify (DWORD)-1 instead of the process pid to read physical memory instead of process virtual memory.

DWORD VMMDLL_MemReadScatter(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Inout_ PPMEM_SCATTER ppMEMs,
    _In_ DWORD cpMEMs,
    _In_ DWORD flags
);

DWORD VMMDLL_MemWriteScatter(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Inout_ PPMEM_SCATTER ppMEMs,
    _In_ DWORD cpMEMs
);

BOOL VMMDLL_MemReadPage(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ ULONG64 qwA,
    _Inout_bytecount_(4096) PBYTE pbPage
);

BOOL VMMDLL_MemRead(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ ULONG64 qwA,
    _Out_writes_(cb) PBYTE pb,
    _In_ DWORD cb
);

BOOL VMMDLL_MemReadEx(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ ULONG64 qwA,
    _Out_writes_(cb) PBYTE pb,
    _In_ DWORD cb,
    _Out_opt_ PDWORD pcbReadOpt,
    _In_ ULONG64 flags
);

BOOL VMMDLL_MemPrefetchPages(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_reads_(cPrefetchAddresses) PULONG64 pPrefetchAddresses,
    _In_ DWORD cPrefetchAddresses
);

BOOL VMMDLL_MemWrite(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ ULONG64 qwA,
    _In_reads_(cb) PBYTE pb,
    _In_ DWORD cb
);

BOOL VMMDLL_MemVirt2Phys(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ ULONG64 qwVA,
    _Out_ PULONG64 pqwPA
);

BOOL VMMDLL_MemCallback(
    _In_ VMM_HANDLE hVMM,
    _In_ VMMDLL_MEM_CALLBACK_TP tp,
    _In_opt_ PVOID ctxUser,
    _In_opt_ VMMDLL_MEM_CALLBACK_PFN pfnCB
);

BOOL VMMDLL_MemSearch(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Inout_ PVMMDLL_MEM_SEARCH_CONTEXT ctx,
    _Out_ PQWORD *ppva,
    _Out_ PDWORD pcva
);

BOOL VMMDLL_YaraSearch(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ PVMMDLL_YARA_CONFIG pYaraConfig,
    _In_ VMMYARA_SCAN_MEMORY_CALLBACK pfnScanMemoryCallback,
    _Out_opt_ PQWORD *ppva,
    _Out_opt_ PDWORD pcva
);
Memory Read Scatter API:
Read physical and virtual memory in an efficient way using the Scatter API using the functions listed below. In most instances it's possible to specify (DWORD)-1 instead of the process pid to read physical memory instead of process virtual memory.

Flow is as follows:

Call VMMDLL_Scatter_Initialize to initialize handle.
Populate memory ranges with multiple calls to VMMDLL_Scatter_Prepare and/or VMMDLL_Scatter_PrepareEx functions. The memory buffer given to VMMDLL_Scatter_PrepareEx will be populated with contents in step (3).
Retrieve the memory by calling VMMDLL_Scatter_ExecuteRead function.
If VMMDLL_Scatter_Prepare was used (i.e. not VMMDLL_Scatter_PrepareEx) then retrieve the memory read in (3).
Clear the handle for reuse by calling VMMDLL_Scatter_Clear alternatively close the handle to free resources with VMMDLL_Scatter_CloseHandle.
VMMDLL_SCATTER_HANDLE VMMDLL_Scatter_Initialize(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ DWORD flags
);

BOOL VMMDLL_Scatter_Prepare(
    _In_ VMMDLL_SCATTER_HANDLE hS,
    _In_ QWORD va,
    _In_ DWORD cb
);

BOOL VMMDLL_Scatter_PrepareEx(
    _In_ VMMDLL_SCATTER_HANDLE hS,
    _In_ QWORD va,
    _In_ DWORD cb,
    _Out_writes_opt_(cb) PBYTE pb,
    _Out_opt_ PDWORD pcbRead
);

BOOL VMMDLL_Scatter_PrepareWrite(
    _In_ VMMDLL_SCATTER_HANDLE hS,
    _In_ QWORD va, _Out_writes_(cb) PBYTE pb,
    _In_ DWORD cb
);

BOOL VMMDLL_Scatter_Execute(
    _In_ VMMDLL_SCATTER_HANDLE hS
);

BOOL VMMDLL_Scatter_ExecuteRead(
    _In_ VMMDLL_SCATTER_HANDLE hS
);

BOOL VMMDLL_Scatter_Read(
    _In_ VMMDLL_SCATTER_HANDLE hS,
    _In_ QWORD va,
    _In_ DWORD cb,
    _Out_writes_opt_(cb) PBYTE pb,
    _Out_opt_ PDWORD pcbRead
);

BOOL VMMDLL_Scatter_Clear(
    _In_ VMMDLL_SCATTER_HANDLE hS,
    _In_ DWORD dwPID,
    _In_ DWORD flags
);

VOID VMMDLL_Scatter_CloseHandle(
    _In_opt_ _Post_ptr_invalid_ VMMDLL_SCATTER_HANDLE hS
);
Process API:
Functionality related to processes running on the target system are exposed in via the functions below:

BOOL VMMDLL_PidGetFromName(
    _In_ VMM_HANDLE hVMM,
    _In_ LPSTR szProcName,
    _Out_ PDWORD pdwPID
);

BOOL VMMDLL_PidList(
    _In_ VMM_HANDLE hVMM,
    _Out_writes_opt_(*pcPIDs) PDWORD pPIDs,
    _Inout_ PSIZE_T pcPIDs
);

BOOL VMMDLL_ProcessGetInformation(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Inout_opt_ PVMMDLL_PROCESS_INFORMATION pProcessInformation,
    _In_ PSIZE_T pcbProcessInformation
);

BOOL VMMDLL_ProcessGetInformationAll(
    _In_ VMM_HANDLE hVMM,
    _Out_ PVMMDLL_PROCESS_INFORMATION *ppProcessInformationAll,
    _Out_ PDWORD pcProcessInformation
);

LPSTR VMMDLL_ProcessGetInformationString(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ DWORD fOptionString
);

ULONG64 VMMDLL_ProcessGetProcAddressU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPSTR  uszModuleName,
    _In_ LPSTR szFunctionName
);

ULONG64 VMMDLL_ProcessGetProcAddressW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPWSTR wszModuleName,
    _In_ LPSTR szFunctionName
);

ULONG64 VMMDLL_ProcessGetModuleBaseU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPSTR  uszModuleName
);

ULONG64 VMMDLL_ProcessGetModuleBaseW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPWSTR wszModuleName
);

BOOL VMMDLL_Map_GetPteU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID, _In_ BOOL fIdentifyModules,
    _Out_ PVMMDLL_MAP_PTE *ppPteMap
);

BOOL VMMDLL_Map_GetPteW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID, _In_ BOOL fIdentifyModules,
    _Out_ PVMMDLL_MAP_PTE *ppPteMap
);

BOOL VMMDLL_Map_GetVadU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ BOOL fIdentifyModules,
    _Out_ PVMMDLL_MAP_VAD *ppVadMap
);

BOOL VMMDLL_Map_GetVadW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ BOOL fIdentifyModules,
    _Out_ PVMMDLL_MAP_VAD *ppVadMap
);

BOOL VMMDLL_Map_GetVadEx(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ DWORD oPage,
    _In_ DWORD cPage,
    _Out_ PVMMDLL_MAP_VADEX *ppVadExMap
);

BOOL VMMDLL_Map_GetModuleU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Out_ PVMMDLL_MAP_MODULE *ppModuleMap,
    _In_ DWORD flags
);

BOOL VMMDLL_Map_GetModuleW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Out_ PVMMDLL_MAP_MODULE *ppModuleMap,
    _In_ DWORD flags
);

BOOL VMMDLL_Map_GetModuleFromNameU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_opt_ LPSTR  uszModuleName,
    _Out_ PVMMDLL_MAP_MODULEENTRY *ppModuleMapEntry,
    _In_ DWORD flags
);

BOOL VMMDLL_Map_GetModuleFromNameW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_opt_ LPWSTR wszModuleName,
    _Out_ PVMMDLL_MAP_MODULEENTRY *ppModuleMapEntry,
    _In_ DWORD flags
);

BOOL VMMDLL_Map_GetUnloadedModuleU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Out_ PVMMDLL_MAP_UNLOADEDMODULE *ppUnloadedModuleMap
);

BOOL VMMDLL_Map_GetUnloadedModuleW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Out_ PVMMDLL_MAP_UNLOADEDMODULE *ppUnloadedModuleMap
);

BOOL VMMDLL_Map_GetEATU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPSTR  uszModuleName,
    _Out_ PVMMDLL_MAP_EAT *ppEatMap
);

BOOL VMMDLL_Map_GetEATW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPWSTR wszModuleName,
    _Out_ PVMMDLL_MAP_EAT *ppEatMap
);

BOOL VMMDLL_Map_GetIATU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPSTR  uszModuleName,
    _Out_ PVMMDLL_MAP_IAT *ppIatMap
);

BOOL VMMDLL_Map_GetIATW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPWSTR wszModuleName,
    _Out_ PVMMDLL_MAP_IAT *ppIatMap
);

BOOL VMMDLL_Map_GetHeap(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Out_ PVMMDLL_MAP_HEAP *ppHeapMap
);

BOOL VMMDLL_Map_GetHeapAlloc(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ QWORD qwHeapNumOrAddress,
    _Out_ PVMMDLL_MAP_HEAPALLOC *ppHeapAllocMap
);

BOOL VMMDLL_Map_GetThread(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Out_ PVMMDLL_MAP_THREAD *ppThreadMap
);

_Success_(return) BOOL VMMDLL_Map_GetThread_CallstackU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ DWORD dwTID,
    _In_ DWORD flags,
    _Out_ PVMMDLL_MAP_THREAD_CALLSTACK *ppThreadCallstack
);

_Success_(return) BOOL VMMDLL_Map_GetThread_CallstackW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ DWORD dwTID,
    _In_ DWORD flags,
    _Out_ PVMMDLL_MAP_THREAD_CALLSTACK *ppThreadCallstack
);

BOOL VMMDLL_Map_GetHandleU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Out_ PVMMDLL_MAP_HANDLE *ppHandleMap
);

BOOL VMMDLL_Map_GetHandleW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _Out_ PVMMDLL_MAP_HANDLE *ppHandleMap
);

BOOL VMMDLL_ProcessGetDirectoriesU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPSTR  uszModule,
    _Out_writes_(16) PIMAGE_DATA_DIRECTORY pDataDirectories
);

BOOL VMMDLL_ProcessGetDirectoriesW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPWSTR wszModule,
    _Out_writes_(16) PIMAGE_DATA_DIRECTORY pDataDirectories
);

BOOL VMMDLL_ProcessGetSectionsU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPSTR  uszModule,
    _Out_writes_opt_(cSections) PIMAGE_SECTION_HEADER pSections,
    _In_ DWORD cSections,
    _Out_ PDWORD pcSections
);

BOOL VMMDLL_ProcessGetSectionsW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPWSTR wszModule,
    _Out_writes_opt_(cSections) PIMAGE_SECTION_HEADER pSections,
    _In_ DWORD cSections,
    _Out_ PDWORD pcSections
);

BOOL VMMDLL_WinGetThunkInfoIATU(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPSTR  uszModuleName,
    _In_ LPSTR szImportModuleName,
    _In_ LPSTR szImportFunctionName,
    _Out_ PVMMDLL_WIN_THUNKINFO_IAT pThunkInfoIAT
);

BOOL VMMDLL_WinGetThunkInfoIATW(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ LPWSTR wszModuleName,
    _In_ LPSTR szImportModuleName,
    _In_ LPSTR szImportFunctionName,
    _Out_ PVMMDLL_WIN_THUNKINFO_IAT pThunkInfoIAT
);
Windows Registry API:
The registry API supports enumerating registry hives and the reading/writing of their memory space. Reading and writing individual registry keys are not supported at the moment.

BOOL VMMDLL_WinReg_HiveList(
    _In_ VMM_HANDLE hVMM,
    _Out_writes_(cHives) PVMMDLL_REGISTRY_HIVE_INFORMATION pHives,
    _In_ DWORD cHives,
    _Out_ PDWORD pcHives
);

BOOL VMMDLL_WinReg_HiveReadEx(
    _In_ VMM_HANDLE hVMM,
    _In_ ULONG64 vaCMHive,
    _In_ DWORD ra,
    _Out_ PBYTE pb,
    _In_ DWORD cb,
    _Out_opt_ PDWORD pcbReadOpt,
    _In_ ULONG64 flags
);

BOOL VMMDLL_WinReg_HiveWrite(
    _In_ VMM_HANDLE hVMM,
    _In_ ULONG64 vaCMHive,
    _In_ DWORD ra,
    _In_ PBYTE pb,
    _In_ DWORD cb
);

BOOL VMMDLL_WinReg_EnumKeyExU(
    _In_ VMM_HANDLE hVMM,
    _In_ LPSTR uszFullPathKey,
    _In_ DWORD dwIndex,
    _Out_writes_opt_(*lpcchName) LPSTR lpName,
    _Inout_ LPDWORD lpcchName,
    _Out_opt_ PFILETIME lpftLastWriteTime
);

BOOL VMMDLL_WinReg_EnumKeyExW(
    _In_ VMM_HANDLE hVMM,
    _In_ LPWSTR wszFullPathKey,
    _In_ DWORD dwIndex,
    _Out_writes_opt_(*lpcchName) LPWSTR lpName,
    _Inout_ LPDWORD lpcchName,
    _Out_opt_ PFILETIME lpftLastWriteTime
);

BOOL VMMDLL_WinReg_EnumValueU(
    _In_ VMM_HANDLE hVMM,
    _In_ LPSTR uszFullPathKey,
    _In_ DWORD dwIndex,
    _Out_writes_opt_(*lpcchValueName) LPSTR lpValueName,
    _Inout_ LPDWORD lpcchValueName,
    _Out_opt_ LPDWORD lpType,
    _Out_writes_opt_(*lpcbData) LPBYTE lpData,
    _Inout_opt_ LPDWORD lpcbData
);


BOOL VMMDLL_WinReg_EnumValueW(
    _In_ VMM_HANDLE hVMM,
    _In_ LPWSTR wszFullPathKey,
    _In_ DWORD dwIndex,
    _Out_writes_opt_(*lpcchValueName) LPWSTR lpValueName,
    _Inout_ LPDWORD lpcchValueName,
    _Out_opt_ LPDWORD lpType,
    _Out_writes_opt_(*lpcbData) LPBYTE lpData,
    _Inout_opt_ LPDWORD lpcbData
);

BOOL VMMDLL_WinReg_QueryValueExU(
    _In_ VMM_HANDLE hVMM,
    _In_ LPSTR uszFullPathKeyValue,
    _Out_opt_ LPDWORD lpType,
    _Out_writes_opt_(*lpcbData) LPBYTE lpData,
    _When_(lpData == NULL, _Out_opt_) _When_(lpData != NULL, _Inout_opt_) LPDWORD lpcbData
);

BOOL VMMDLL_WinReg_QueryValueExW(
    _In_ VMM_HANDLE hVMM,
    _In_ LPWSTR wszFullPathKeyValue,
    _Out_opt_ LPDWORD lpType,
    _Out_writes_opt_(*lpcbData) LPBYTE lpData,
    _When_(lpData == NULL, _Out_opt_) _When_(lpData != NULL, _Inout_opt_) LPDWORD lpcbData
);
Windows specific symbol debugging API:
Limited functionality for debugging symbols retrieved from the Microsoft symbol server.

BOOL VMMDLL_PdbLoad(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ ULONG64 vaModuleBase,
    _Out_writes_(MAX_PATH) LPSTR szModuleName
);

BOOL VMMDLL_PdbSymbolName(
    _In_ VMM_HANDLE hVMM,
    _In_ LPSTR szModule,
    _In_ QWORD cbSymbolAddressOrOffset,
    _Out_writes_(MAX_PATH) LPSTR szSymbolName,
    _Out_opt_ PDWORD pdwSymbolDisplacement
);

BOOL VMMDLL_PdbSymbolAddress(
    _In_ VMM_HANDLE hVMM,
    _In_ LPSTR szModule,
    _In_ LPSTR szSymbolName,
    _Out_ PULONG64 pvaSymbolAddress
);

BOOL VMMDLL_PdbTypeSize(
    _In_ VMM_HANDLE hVMM,
    _In_ LPSTR szModule,
    _In_ LPSTR szTypeName,    
    _Out_ PDWORD pcbTypeSize
);

BOOL VMMDLL_PdbTypeChildOffset(
    _In_ VMM_HANDLE hVMM,
    _In_ LPSTR szModule,
    _In_ LPSTR szTypeName,
    _In_ LPWSTR wszTypeChildName,    
    _Out_ PDWORD pcbTypeChildOffset
);
Windows specific API:
BOOL VMMDLL_Map_GetPfn(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD pPfns[],
    _In_ DWORD cPfns,
    _Out_writes_bytes_opt_(*pcbPfnMap) PVMMDLL_MAP_PFN pPfnMap,
    _Inout_ PDWORD pcbPfnMap
);

BOOL VMMDLL_Map_GetPool(
    _In_ VMM_HANDLE hVMM,
    _Out_ PVMMDLL_MAP_POOL *ppPoolMap,
    _In_ DWORD flags
);

BOOL VMMDLL_Map_GetNetU(
    _In_ VMM_HANDLE hVMM,
    _Out_ PVMMDLL_MAP_NET *ppNetMap
);

BOOL VMMDLL_Map_GetNetW(
    _In_ VMM_HANDLE hVMM,
    _Out_ PVMMDLL_MAP_NET *ppNetMap
);

BOOL VMMDLL_Map_GetUsersU(
    _In_ VMM_HANDLE hVMM,
    _Out_ PVMMDLL_MAP_USER *ppUserMap
);

BOOL VMMDLL_Map_GetUsersW(
    _In_ VMM_HANDLE hVMM,
    _Out_ PVMMDLL_MAP_USER *ppUserMap
);

BOOL VMMDLL_Map_GetVMU(
    _In_ VMM_HANDLE hVMM,
    _Out_ PVMMDLL_MAP_VM *ppVmMap
);
BOOL VMMDLL_Map_GetVMW(
    _In_ VMM_HANDLE hVMM,
    _Out_ PVMMDLL_MAP_VM *ppVmMap
);

BOOL VMMDLL_Map_GetServicesU(
    _In_ VMM_HANDLE hVMM,
    _Out_ PVMMDLL_MAP_SERVICE *ppServiceMap
);

BOOL VMMDLL_Map_GetServicesW(
    _In_ VMM_HANDLE hVMM,
    _Out_ PVMMDLL_MAP_SERVICE *ppServiceMap
);
Virtual Machine (VM) API:
VMM_HANDLE VMMDLL_VmGetVmmHandle(
    _In_ VMM_HANDLE hVMM,
    _In_ VMMVM_HANDLE hVM
);

VMMDLL_SCATTER_HANDLE VMMDLL_VmScatterInitialize(
    _In_ VMM_HANDLE hVMM,
    _In_ VMMVM_HANDLE hVM
);

BOOL VMMDLL_VmMemRead(
    _In_ VMM_HANDLE hVMM,
    _In_ VMMVM_HANDLE hVM,
    _In_ ULONG64 qwGPA,
    _Out_writes_(cb) PBYTE pb,
    _In_ DWORD cb
);

DWORD VMMDLL_VmMemReadScatter(
    _In_ VMM_HANDLE hVMM,
    _In_ VMMVM_HANDLE hVM,
    _Inout_ PPMEM_SCATTER ppMEMsGPA,
    _In_ DWORD cpMEMsGPA,
    _In_ DWORD flags
);

BOOL VMMDLL_VmMemWrite(
    _In_ VMM_HANDLE hVMM,
    _In_ DWORD dwPID,
    _In_ ULONG64 qwGPA,
    _In_reads_(cb) PBYTE pb,
    _In_ DWORD cb
);

DWORD VMMDLL_VmMemWriteScatter(
    _In_ VMM_HANDLE hVMM,
    _In_ VMMVM_HANDLE hVM,
    _Inout_ PPMEM_SCATTER ppMEMsGPA,
    _In_ DWORD cpMEMsGPA
);

BOOL VMMDLL_VmMemTranslateGPA(
    _In_ VMM_HANDLE H,
    _In_ VMMVM_HANDLE hVM,
    _In_ ULONG64 qwGPA,
    _Out_opt_ PULONG64 pPA,
    _Out_opt_ PULONG64 pVA
);
Logging API:
VOID VMMDLL_Log(
    _In_ VMM_HANDLE hVMM,
    _In_opt_ VMMDLL_MODULE_ID MID,
    _In_ VMMDLL_LOGLEVEL dwLogLevel,
    _In_z_ _Printf_format_string_ LPSTR uszFormat,
    ...
);

VOID VMMDLL_LogEx(
    _In_ VMM_HANDLE hVMM,
    _In_opt_ VMMDLL_MODULE_ID MID,
    _In_ VMMDLL_LOGLEVEL dwLogLevel,
    _In_z_ _Printf_format_string_ LPSTR uszFormat,
    va_list arglist
);
Utility API:
BOOL VMMDLL_UtilFillHexAscii(
    _In_reads_opt_(cb) PBYTE pb,
    _In_ DWORD cb,
    _In_ DWORD cbInitialOffset,
    _Out_writes_opt_(*pcsz) LPSTR sz,
    _Inout_ PDWORD pcsz
);
Pages 69
Overview
Home
Command Line
MemProcFS on ARM64
MemProcFS on Linux
MemProcFS on macOS
MemProcFS Remoting
API
C/C++ API
C# API
Java API
Rust API
Python API
base
process
registry
search/yara
Jupyter notebook
Root File System
(root)
conf
forensic
csv
files
findevil
json
ntfs
prefetch
timeline
web
yara
misc
bitlocker
eventlog
phys2virt
procinfo
search
bin
yara
view
registry
sys
certificates
drivers
memory
objects
net
pool
proc
services
syscall
sysinfo
tasks
users
vm
Per-Process File System
(root)
console
files
handles
heaps
memmap
minidump
modules
phys2virt
py
procstruct
search
bin
yara
threads
token
virt2phys
vmemd
Development
Building
C Plugins
Rust Plugins
Python Plugins
FAQ
Timing & Refresh
DMA not working
DMA on AMD/
Thunderbolt
Clone this wiki locally
https://github.com/ufrisk/MemProcFS.wiki.git
Footer
© 2026 GitHub, Inc.
Footer navigation
Terms
Privacy
Security
Status
Community
Docs
Contact
Manage cookies
Do not share my personal information
