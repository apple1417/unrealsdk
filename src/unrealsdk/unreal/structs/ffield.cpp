#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/structs/ffield.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unreal/structs/tfieldvariant.h"
#include "unrealsdk/unrealsdk.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(FFieldClass, UNREALSDK_FFIELDCLASS_FIELDS);
UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(FField, UNREALSDK_FFIELD_FIELDS);

[[nodiscard]] bool FFieldClass::inherits(const FFieldClass* base_class) const {
    auto cls = this;
    do {
        if (cls == base_class) {
            return true;
        }
    } while ((cls = cls->SuperField()) != nullptr);
    return false;
}
[[nodiscard]] bool FFieldClass::inherits(const UStruct* base_class) const {
    // HACK: assume the UStructs have the same name as their associated FFieldClass
    auto name = base_class->Name();

    auto cls = this;
    do {
        if (cls->Name() == name) {
            return true;
        }
    } while ((cls = cls->SuperField()) != nullptr);
    return false;
}

bool FField::is_instance(const FFieldClass* cls) const {
    return this->Class()->inherits(cls);
}
bool FField::is_instance(const UClass* cls) const {
    return this->Class()->inherits(cls);
}

#ifdef UNREALSDK_INTERNAL_PATH_NAME

namespace {

/**
 * @brief Recursive helper to generate full field path name.
 *
 * @tparam T The type of string to get the name in.
 * @param obj The object to get the path name of.
 * @param stream The stream to push the object name onto.
 */
void iter_path_name(const FField* field, std::wstringstream& stream) {
    const auto& owner = field->Owner();
    if (owner != nullptr) {
        if (owner.is_ffield()) {
            iter_path_name(owner.as_ffield(), stream);
        } else {
            stream << owner.as_uobject()->get_path_name();
        }

        // Going to way too much effort to replicate the separator logic here
        static constexpr auto variant_cls = [](const FFieldVariant& var) {
            return var.is_ffield() ? var.as_ffield()->Class()->Name()
                                   : var.as_uobject()->Class()->Name();
        };
        static const FName package_name = L"Package"_fn;

        bool use_colon_separator = false;
        if (variant_cls(owner) != package_name) {
            FFieldVariant grandparent;
            if (owner.is_ffield()) {
                grandparent = owner.as_ffield()->Owner();
            } else {
                grandparent = owner.as_uobject()->Outer();
            }
            if (grandparent != nullptr) {
                use_colon_separator = variant_cls(grandparent) == package_name;
            }
        }
        stream << (use_colon_separator ? L':' : L'.');
        stream << field->Name();
    }
}

}  // namespace

[[nodiscard]] std::wstring FField::get_path_name(void) const {
    std::wstringstream stream;
    iter_path_name(this, stream);
    return stream.str();
}

#else

[[nodiscard]] std::wstring FField::get_path_name(void) const {
    return unrealsdk::internal::ffield_path_name(this);
}

#endif

}  // namespace unrealsdk::unreal
