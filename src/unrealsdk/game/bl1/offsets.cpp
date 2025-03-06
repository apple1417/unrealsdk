#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl1/offsets.h"
#include "unrealsdk/game/bl1/bl1.h"
#include "unrealsdk/unreal/classes/ufield.h"
#include "unrealsdk/unreal/offset_list.h"

#if defined(UE3) && defined(ARCH_X86) && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::unreal::offsets;

namespace unrealsdk::game {
namespace bl1 {
namespace {

constexpr auto OFFSETS = OFFSET_LIST_FROM_NAMESPACE();

}
}  // namespace bl1

[[nodiscard]] const unreal::offsets::OffsetList& BL1Hook::get_offsets(void) const {
    return bl1::OFFSETS;
}

unreal::TArray<unreal::TPair<unreal::FName, uint64_t>> BL1Hook::uenum_get_names(
    const unreal::UEnum* uenum) const {
    return reinterpret_cast<const bl1::UEnum*>(uenum)->get_names();
}

}  // namespace unrealsdk::game
#endif
