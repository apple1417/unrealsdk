#include "pch.h"
#include <stdint.h>

#include "unreal/classes/uclass.h"
#include "unreal/classes/uproperty.h"
#include "unreal/structs/fname.h"
#include "game/game_hook.h"
#include "env.h"

namespace unrealsdk::unreal {

#ifdef UE3

size_t UProperty::class_size(void) {
    static size_t size = 0;
    if (size != 0) {
        return size;
    }

    size = env::get_numeric<size_t>(env::UPROPERTY_SIZE);
    if (size != 0) {
        return size;
    }

    // Rather than bother with a findobject call, we can recover UProperty from any arbitrary object
    // UObject always has properties on it, we don't need to worry about what class we get
    auto obj = *game::gobjects().begin();
    auto prop = obj->Class->PropertyLink;

    UStruct* cls = prop->Class;
    while (cls->Name != L"Property"_fn) {
        cls = cls->SuperField;
        if (cls == nullptr) {
            break;
        }
    }

    // If we couldn't find the class, default to our actual size
    if (cls == nullptr) {
        size = sizeof(UProperty);
        LOG(WARNING, "Couldn't find UProperty class size, defaulting to: %#x", size);
    } else {
        size = cls->get_struct_size();
        LOG(INFO, "UProperty class size: %#x", size);
    }
    return size;
}

#endif

}  // namespace unrealsdk::unreal