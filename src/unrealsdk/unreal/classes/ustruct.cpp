#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/classes/ustruct.h"
#include "unrealsdk/config.h"
#include "unrealsdk/game/bl3/offsets.h"
#include "unrealsdk/game/bl4/offsets.h"
#include "unrealsdk/unreal/class_name.h"
#include "unrealsdk/unreal/classes/ufield.h"
#include "unrealsdk/unreal/classes/ufunction.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/find_class.h"
#include "unrealsdk/unreal/offset_list.h"
#include "unrealsdk/unreal/offsets.h"
#include "unrealsdk/unreal/wrappers/bound_function.h"
#include "unrealsdk/unreal/wrappers/gobjects.h"
#include "unrealsdk/utils.h"

namespace unrealsdk::unreal {

UNREALSDK_DEFINE_FIELDS_SOURCE_FILE(UStruct, UNREALSDK_USTRUCT_FIELDS);

#pragma region Field Iterator

UStruct::FieldIterator::FieldIterator(void) : this_struct(nullptr), field(nullptr) {}
UStruct::FieldIterator::FieldIterator(const UStruct* this_struct, UField* field)
    : this_struct(this_struct), field(field) {}

UStruct::FieldIterator::reference UStruct::FieldIterator::operator*() const {
    return this->field;
}

UStruct::FieldIterator& UStruct::FieldIterator::operator++() {
    if (this->field != nullptr) {
        this->field = this->field->Next();
    }
    while (this->field == nullptr && this->this_struct != nullptr) {
        this->this_struct = this->this_struct->SuperField();

        if (this->this_struct != nullptr) {
            this->field = this->this_struct->Children();
        }
    }

    return *this;
}
UStruct::FieldIterator UStruct::FieldIterator::operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

bool UStruct::FieldIterator::operator==(const UStruct::FieldIterator& rhs) const {
    return this->this_struct == rhs.this_struct && this->field == rhs.field;
};
bool UStruct::FieldIterator::operator!=(const UStruct::FieldIterator& rhs) const {
    return !(*this == rhs);
};

utils::IteratorProxy<UStruct::FieldIterator> UStruct::fields(void) const {
    FieldIterator begin{this, this->Children()};

    // If we start out pointing at null (because this struct has no direct children), increment once
    //  to find the actual first field
    if (*begin == nullptr) {
        begin++;
    }

    return {begin, {}};
}

#pragma endregion

#pragma region Property Iterator

// TODO: is this really what we want to be doing?
#if UNREALSDK_PROPERTIES_ARE_FFIELD
// Follow the ChildProperties->Next->Next linked list
// This is split by struct, so this is very similar to the above implementation

UStruct::PropertyIterator::PropertyIterator(void) : this_struct(nullptr), field(nullptr) {}
UStruct::PropertyIterator::PropertyIterator(const UStruct* this_struct, FField* field)
    : this_struct(this_struct), field(field) {}

UStruct::PropertyIterator::reference UStruct::PropertyIterator::operator*() const {
    // Trust that the increment keeps this valid
    return reinterpret_cast<UProperty*>(this->field);
}

UStruct::PropertyIterator& UStruct::PropertyIterator::operator++() {
    auto uprop_cls = find_class<UProperty>();

    if (this->field != nullptr) {
        do {
            this->field = this->field->Next();
            // If we got a field that's not a property, try again
        } while (this->field != nullptr && !this->field->Class()->inherits(uprop_cls));
    }

    while (this->field == nullptr && this->this_struct != nullptr) {
        // Ran out of fields, try the next struct
        this->this_struct = this->this_struct->SuperField();
        if (this->this_struct == nullptr) {
            break;
        }

        this->field = this->this_struct->ChildProperties();

        // If we got a field that's not a property, try again
        while (this->field != nullptr && !this->field->Class()->inherits(uprop_cls)) {
            this->field = this->field->Next();
        }
    }

    return *this;
}
UStruct::PropertyIterator UStruct::PropertyIterator::operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

bool UStruct::PropertyIterator::operator==(const UStruct::PropertyIterator& rhs) const {
    return this->this_struct == rhs.this_struct && this->field == rhs.field;
};
bool UStruct::PropertyIterator::operator!=(const UStruct::PropertyIterator& rhs) const {
    return !(*this == rhs);
};

utils::IteratorProxy<UStruct::PropertyIterator> UStruct::properties(void) const {
    PropertyIterator begin{this, this->ChildProperties()};

    // Like in the previous one, a null first property is still wrong, and needs to be incremented
    // once, but now so is a non-property type
    UProperty* first = *begin;
    if (first == nullptr || !first->Class()->inherits(find_class<UProperty>())) {
        begin++;
    }

    return {begin, {}};
}

#else
// Follow the PropertyLink->PropertyLinkNext->PropertyLinkNext linked list
// This includes base classes so has a much simpler implementation

UStruct::PropertyIterator::PropertyIterator(void) : prop(nullptr) {}
UStruct::PropertyIterator::PropertyIterator(UProperty* prop) : prop(prop) {}

UStruct::PropertyIterator::reference UStruct::PropertyIterator::operator*() const {
    return prop;
}

UStruct::PropertyIterator& UStruct::PropertyIterator::operator++() {
    prop = prop->PropertyLinkNext();
    return *this;
}
UStruct::PropertyIterator UStruct::PropertyIterator::operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

bool UStruct::PropertyIterator::operator==(const UStruct::PropertyIterator& rhs) const {
    return this->prop == rhs.prop;
};
bool UStruct::PropertyIterator::operator!=(const UStruct::PropertyIterator& rhs) const {
    return !(*this == rhs);
};

utils::IteratorProxy<UStruct::PropertyIterator> UStruct::properties(void) const {
    return {{this->PropertyLink()}, {}};
}

#endif

#pragma endregion

#pragma region SuperField Iterator

UStruct::SuperFieldIterator::SuperFieldIterator(void) : this_struct(nullptr) {}
UStruct::SuperFieldIterator::SuperFieldIterator(const UStruct* this_struct)
    : this_struct(this_struct) {}

UStruct::SuperFieldIterator::reference UStruct::SuperFieldIterator::operator*() const {
    return this->this_struct;
}

UStruct::SuperFieldIterator& UStruct::SuperFieldIterator::operator++() {
    this->this_struct = this->this_struct->SuperField();
    return *this;
}
UStruct::SuperFieldIterator UStruct::SuperFieldIterator::operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

bool UStruct::SuperFieldIterator::operator==(const UStruct::SuperFieldIterator& rhs) const {
    return this->this_struct == rhs.this_struct;
};
bool UStruct::SuperFieldIterator::operator!=(const UStruct::SuperFieldIterator& rhs) const {
    return !(*this == rhs);
};

utils::IteratorProxy<UStruct::SuperFieldIterator> UStruct::superfields(void) const {
    return {{this}, {}};
}

#pragma endregion

size_t UStruct::get_struct_size(void) const {
#if UNREALSDK_USTRUCT_HAS_ALIGNMENT
    auto min_alignment = this->MinAlignment();
    return (this->PropertySize() + min_alignment - 1) & ~(min_alignment - 1);
#else
    return this->PropertySize();
#endif
}

#if UNREALSDK_PROPERTIES_ARE_FFIELD
[[nodiscard]] TFieldVariant<UProperty, UField> UStruct::find(const FName& name) const {
    for (auto prop : this->properties()) {
        if (prop->Name() == name) {
            return {prop};
        }
    }
    for (auto field : this->fields()) {
        if (field->Name() == name) {
            return {field};
        }
    }

    throw std::invalid_argument("Couldn't find field " + (std::string)name);
}
#else
TFieldVariantStub<UField> UStruct::find(const FName& name) const {
    for (auto field : this->fields()) {
        if (field->Name() == name) {
            return {field};
        }
    }

    throw std::invalid_argument("Couldn't find field " + (std::string)name);
}
#endif

UProperty* UStruct::find_prop(const FName& name) const {
    for (auto prop : this->properties()) {
        if (prop->Name() == name) {
            return prop;
        }
    }

    throw std::invalid_argument("Couldn't find property " + (std::string)name);
}

UFunction* UStruct::find_func_and_validate(const FName& name) const {
    auto result = this->find(name);
    if (!result.is_field()) {
        throw std::invalid_argument("expected function but got property");
    }
    return validate_type<UFunction>(result.as_field());
}

bool UStruct::inherits(const UStruct* base_struct) const {
    auto superfields = this->superfields();
    return std::ranges::find(superfields, base_struct) != superfields.end();
}

}  // namespace unrealsdk::unreal
