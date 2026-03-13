// Quick Integration Guide - Add Ring-1 improvements to your existing bridge.cpp
// This shows minimal changes needed to integrate the new improvements

#include "hook_manager.h"         // NEW
#include "code_cave_finder.h"     // NEW
#include "relocation_tracker.h"   // NEW
// ... your existing includes ...

int main(int argc, char* argv[]) {
    // ... your existing VMM initialization ...
    
    VMM_HANDLE hVMM = VMMDLL_Initialize(3, vmmArgv);
    DWORD pid = /* your existing process finding code */;
    
    // ==========================================
    //  Initialize hook registry (Ring-1)
    // ==========================================
    HookManager::HookRegistry hookRegistry;
    
    // ==========================================
    //  Find code caves (Ring-1 approach)
    // ==========================================
    CodeCave::CaveFinder caveFinder(hVMM, pid);
    std::vector<const char*> modules = {"ntdll.dll", "kernel32.dll", "kernelbase.dll"};
    caveFinder.FindExecutableCaves(modules, 0x200);
    
    auto* shellcodeCave = caveFinder.GetBestCave(0x200);
    if (!shellcodeCave) {
        std::cerr << "[-] No suitable code cave found!" << std::endl;
        return -1;
    }
    
    // ==========================================
    // EXISTING: Load your DLL
    // ==========================================
    PEFile payload;
    payload.load("your_payload.dll");
    auto mappedImage = payload.mapImage();
    
    // ==========================================
    //  Improved relocation handling
    // ==========================================
    RelocationTracker::RelocationMap relocMap;
    relocMap.ParseRelocations(mappedImage, payload.nt);
    
    ULONG64 allocBase = 0x180000000; // Your existing allocation
    relocMap.ApplyRelocations(mappedImage, payload.preferredBase(), allocBase);
    
    //  Verify relocations
    if (!relocMap.VerifyRelocations(mappedImage, allocBase)) {
        relocMap.DumpRelocations(); // Debug output
    }
    
    // ==========================================
    // EXISTING: Write DLL to guest
    // ==========================================
    VMMDLL_MemWrite(hVMM, pid, allocBase, mappedImage.data(), mappedImage.size());
    
    // ==========================================
    //  Register and install hooks (Ring-1)
    // ==========================================
    ULONG64 targetFunc = VMMDLL_ProcessGetProcAddressU(hVMM, pid, "kernel32.dll", "Sleep");
    ULONG64 detourFunc = allocBase + payload.entryPointRVA();
    
    // Register hook (separates registration from installation)
    hookRegistry.RegisterHook(
        "Sleep_Hook",
        targetFunc,
        detourFunc,
        HookManager::HookType::INLINE_JMP,
        hVMM,
        pid
    );
    
    // Install the hook
    if (!hookRegistry.InstallHook("Sleep_Hook")) {
        std::cerr << "[-] Hook installation failed" << std::endl;
        return -1;
    }
    
    //  Verify hook was installed correctly
    hookRegistry.VerifyAllHooks();
    
    //  Print statistics
    hookRegistry.PrintStats();
    
    std::cout << "[+] Injection complete! Press Enter to cleanup..." << std::endl;
    std::cin.get();
    
    //  Cleanup (automatically restores hooks via destructor)
    // hookRegistry destructor handles this
    
    VMMDLL_Close(hVMM);
    return 0;
}


