// Improved Injection Bridge using Ring-1 inspired techniques
// This demonstrates how to integrate the new improvements into your existing injection system

#include <windows.h>
#include <iostream>
#include "Memproc/vmmdll.h"
#include "hook_manager.h"
#include "code_cave_finder.h"
#include "relocation_tracker.h"
#include "pe_mapper.h"

// Example: Improved injection flow with Ring-1 principles
class ImprovedInjector {
private:
    VMM_HANDLE hVMM;
    DWORD pid;
    HookManager::HookRegistry hookRegistry;
    
public:
    ImprovedInjector(VMM_HANDLE vm, DWORD processId) : hVMM(vm), pid(processId) {}
    
    bool InjectWithRing1Improvements(const char* dllPath) {
        std::cout << "\n[Injector] Starting Ring-1 inspired injection..." << std::endl;
        std::cout << "===============================================" << std::endl;
        
        // Step 1: Load and map the PE file (improved relocation tracking)
        PEFile payload;
        if (!payload.load(dllPath)) {
            std::cerr << "[Injector] Failed to load DLL: " << dllPath << std::endl;
            return false;
        }
        
        std::cout << "[Injector] Loaded DLL: " << dllPath << std::endl;
        std::cout << "  Image Size: 0x" << std::hex << payload.imageSize() << std::dec << std::endl;
        std::cout << "  Preferred Base: 0x" << std::hex << payload.preferredBase() << std::dec << std::endl;
        
        // Step 2: Find suitable code caves (Ring-1 multi-module approach)
        CodeCave::CaveFinder caveFinder(hVMM, pid);
        
        // Ring-1 searches ntdll, kernel32, kernelbase in priority order
        std::vector<const char*> modulesToSearch{"ntdll.dll", "kernel32.dll", "kernelbase.dll"};
        
        if (!caveFinder.FindExecutableCaves(modulesToSearch, 0x200)) {
            std::cerr << "[Injector] No suitable code caves found" << std::endl;
            return false;
        }
        
        caveFinder.PrintAllCaves();
        
        // Step 3: Allocate memory for DLL (in this example, we still use basic allocation)
        // Note: Ring-1 uses PML4 cloning, which requires hypervisor access - not possible in Host approach
        ULONG64 allocBase = 0; // You would allocate via your existing method here
        
        // For demonstration, let's say we allocated at a base address
        allocBase = 0x180000000; // Example address
        
        // Step 4: Map image with improved relocation tracking
        auto mappedImage = payload.mapImage();
        
        RelocationTracker::RelocationMap relocMap;
        if (!relocMap.ParseRelocations(mappedImage, payload.nt)) {
            std::cerr << "[Injector] Failed to parse relocations" << std::endl;
            return false;
        }
        
        if (!relocMap.ApplyRelocations(mappedImage, payload.preferredBase(), allocBase)) {
            std::cerr << "[Injector] Failed to apply relocations" << std::endl;
            return false;
        }
        
        // Verify relocations (Ring-1 debugging practice)
        if (!relocMap.VerifyRelocations(mappedImage, allocBase)) {
            std::cerr << "[Injector] Relocation verification failed!" << std::endl;
            relocMap.DumpRelocations(); // Print detailed info for debugging
        }
        
        // Step 5: Write mapped DLL to guest memory
        std::cout << "[Injector] Writing DLL to guest memory at 0x" << std::hex << allocBase << std::dec << std::endl;
        
        if (!VMMDLL_MemWrite(hVMM, pid, allocBase, mappedImage.data(), mappedImage.size())) {
            std::cerr << "[Injector] Failed to write DLL to guest memory" << std::endl;
            return false;
        }
        
        // Step 6: Find code cave for hook shellcode
        auto* shellcodeCave = caveFinder.GetBestCave(0x200); // Need ~512 bytes for full-context hook
        if (!shellcodeCave) {
            std::cerr << "[Injector] No cave found for shellcode" << std::endl;
            return false;
        }
        
        // Verify cave is still clean
        if (!caveFinder.VerifyCave(*shellcodeCave)) {
            std::cerr << "[Injector] Selected cave has been modified, unsafe to use" << std::endl;
            return false;
        }
        
        // Step 7: Install hooks using improved hook manager (Ring-1 approach)
        ULONG64 targetFunc = VMMDLL_ProcessGetProcAddressU(hVMM, pid, "kernel32.dll", "Sleep");
        if (!targetFunc) {
            std::cerr << "[Injector] Failed to find target function" << std::endl;
            return false;
        }
        
        ULONG64 dllEntryPoint = allocBase + payload.entryPointRVA();
        
        // Register hook (Ring-1 separates registration from installation)
        auto* hook = hookRegistry.RegisterHook(
            "Sleep_Hook",
            targetFunc,
            dllEntryPoint,
            HookManager::HookType::INLINE_JMP,
            hVMM,
            pid
        );
        
        if (!hook) {
            std::cerr << "[Injector] Failed to register hook" << std::endl;
            return false;
        }
        
        // Install the hook
        if (!hookRegistry.InstallHook("Sleep_Hook")) {
            std::cerr << "[Injector] Failed to install hook" << std::endl;
            return false;
        }
        
        // Step 8: Verify everything (Ring-1 health check)
        std::cout << "\n[Injector] Performing post-injection verification..." << std::endl;
        
        if (!hookRegistry.VerifyAllHooks()) {
            std::cerr << "[Injector] Hook verification failed!" << std::endl;
            return false;
        }
        
        // Print statistics
        hookRegistry.PrintStats();
        
        std::cout << "\n[Injector] Injection completed successfully!" << std::endl;
        std::cout << "  DLL Base: 0x" << std::hex << allocBase << std::dec << std::endl;
        std::cout << "  Entry Point: 0x" << std::hex << dllEntryPoint << std::dec << std::endl;
        std::cout << "  Hooks Installed: " << hookRegistry.GetHook("Sleep_Hook") != nullptr << std::endl;
        
        return true;
    }
    
    // Ring-1 style cleanup
    void Cleanup() {
        std::cout << "\n[Injector] Cleaning up..." << std::endl;
        // HookRegistry destructor automatically restores all hooks
    }
};

// Usage example
int ExampleUsage(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: injector_bridge_improved.exe <target_process> <payload.dll>" << std::endl;
        return -1;
    }
    
    const char* targetProcess = argv[1];
    const char* payloadPath = argv[2];
    
    std::cout << "[+] Initializing MemProcFS via LiveCloudKd..." << std::endl;
    
    LPCSTR vmmArgv[] = { "", "-device", "hvmm://id=0,m=0" };
    VMM_HANDLE hVMM = VMMDLL_Initialize(3, vmmArgv);
    if (!hVMM) {
        std::cerr << "[-] Failed to initialize VMM" << std::endl;
        return -1;
    }
    
    std::cout << "[+] Successfully hooked into Hyper-V Guest Memory!" << std::endl;
    
    // Find target process
    DWORD pid = 0;
    std::cout << "[+] Searching for " << targetProcess << "..." << std::endl;
    
    for (int attempt = 0; attempt < 120; attempt++) {
        if (VMMDLL_PidGetFromName(hVMM, targetProcess, &pid)) break;
        Sleep(1000);
    }
    
    if (pid == 0) {
        std::cerr << "[-] Process not found" << std::endl;
        VMMDLL_Close(hVMM);
        return -1;
    }
    
    std::cout << "[+] Found " << targetProcess << " PID: " << pid << std::endl;
    
    // Inject with improved techniques
    ImprovedInjector injector(hVMM, pid);
    bool success = injector.InjectWithRing1Improvements(payloadPath);
    
    if (success) {
        std::cout << "\n[+] Press Enter to cleanup and exit..." << std::endl;
        std::cin.get();
    }
    
    injector.Cleanup();
    VMMDLL_Close(hVMM);
    
    return success ? 0 : -1;
}


