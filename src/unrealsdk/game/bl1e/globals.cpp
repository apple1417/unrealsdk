#include "unrealsdk/pch.h"

#include "unrealsdk/game/bl1e/bl1e.h"

#include "unrealsdk/memory.h"
#include "unrealsdk/unreal/wrappers/gnames.h"
#include "unrealsdk/unreal/wrappers/gobjects.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW64 && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::memory;
using namespace unrealsdk::unreal;

namespace unrealsdk::game {

namespace {

GObjects gobjects_wrapper{};

constexpr Pattern<18> SIG_GOBJECTS{
    "8B0D ????????"        // MOV  this,dword ptr [DAT_1425d95d8]
    "48 8B 15 {????????}"  // MOV  RDX,qword ptr [BL1E_GObjects]
    "48 833CDA 00"         // CMP  qword ptr [RDX + RBX*0x8],0x0
};

}  // namespace

void BL1EHook::find_gobjects(void) {
    auto gobjects_ptr = read_offset<GObjects::internal_type>(SIG_GOBJECTS.sigscan_nullable());
    LOG(MISC, "GObjects: {:p}", reinterpret_cast<void*>(gobjects_ptr));
    gobjects_wrapper = GObjects{gobjects_ptr};
}

const GObjects& BL1EHook::gobjects(void) const {
    return gobjects_wrapper;
};

namespace {

GNames gnames_wrapper{};

constexpr Pattern<25> SIG_GNAMES{
    "E8 ????????"        // CALL  FUN_1401afda0
    "488B05 {????????}"  // MOV   RAX,qword ptr [GNames_NameHash]
    "48 89 3C D8"        // MOV   qword ptr [RAX + RBX*0x8],RDI
    "49 8B CC"           // MOV   this,R12
    "FF15 ????????"      // CALL  qword ptr [->KERNEL32.DLL::LeaveCriticalSection]
};

}  // namespace

void BL1EHook::find_gnames(void) {
    auto gnames_ptr = read_offset<GNames::internal_type>(SIG_GNAMES.sigscan_nullable());
    LOG(MISC, "GNames: {:p}", reinterpret_cast<void*>(gnames_ptr));
    gnames_wrapper = GNames{gnames_ptr};
}

const GNames& BL1EHook::gnames(void) const {
    return gnames_wrapper;
}

}  // namespace unrealsdk::game

#endif
