#include "unrealsdk/pch.h"
#include "unrealsdk/game/bl4/offsets.h"
#include "unrealsdk/game/bl4/bl4.h"
#include "unrealsdk/unreal/offset_list.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2 && !defined(UNREALSDK_IMPORTING)

using namespace unrealsdk::unreal::offsets;

namespace unrealsdk::game {
namespace bl4 {
namespace {

constexpr auto OFFSETS = OFFSET_LIST_FROM_NAMESPACE();

}

}  // namespace bl4

[[nodiscard]] const unreal::offsets::OffsetList& BL4Hook::get_offsets(void) const {
    return bl4::OFFSETS;
}

}  // namespace unrealsdk::game
#endif
