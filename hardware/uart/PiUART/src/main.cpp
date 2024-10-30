#include "kernel.h"
#include <circle/startup.h>

int main() {

    CKernel Kernel = CKernel(CMemorySystem::Get());

    // initialise kernel and start cores 1-3
    if (!Kernel.Initialize()) {
        CMultiCoreSupport::HaltAll();
        return EXIT_HALT;
    }

    // start core 0
    Kernel.Run(CORE_MAIN);

    // we should never get here
    CMultiCoreSupport::HaltAll();
    return EXIT_HALT;

}
