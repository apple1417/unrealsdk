#ifndef UNREALSDK_GAME_BL4_OFFSETS_H
#define UNREALSDK_GAME_BL4_OFFSETS_H

#include "unrealsdk/game/bl3/offsets.h"

#if UNREALSDK_FLAVOUR == UNREALSDK_FLAVOUR_OAK2

namespace unrealsdk::game::bl4 {

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
#endif
// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init,
//             readability-identifier-naming,
//             readability-magic-numbers)

using namespace unrealsdk::game::bl3;

struct FNameEntry {
    static const constexpr auto META_SIZE_BIT_OFFSET = 6;

    union {
        uint8_t Flags;
        uint16_t Metadata;
    };
    unreal::FNameEntry::name_union Name;
};

// NOLINTEND(cppcoreguidelines-pro-type-member-init,
//           readability-identifier-naming,
//           readability-magic-numbers)
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

}  // namespace unrealsdk::game::bl4

#endif

#endif /* UNREALSDK_GAME_BL4_OFFSETS_H */
