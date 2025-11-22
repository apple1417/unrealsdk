#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl1e/offsets.h"
#include "unrealsdk/game/bl1e/bl1e.h"
#include "unrealsdk/unreal/classes/ufield.h"
#include "unrealsdk/unreal/offset_list.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_WILLOW64 && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::unreal::offsets;

namespace unrealsdk::game {
namespace bl1e {
namespace {

constexpr auto OFFSETS = OFFSET_LIST_FROM_NAMESPACE();

}
}  // namespace bl1e

[[nodiscard]] const OffsetList& BL1EHook::get_offsets(void) const {
    return bl1e::OFFSETS;
}

}  // namespace unrealsdk::game
#endif
