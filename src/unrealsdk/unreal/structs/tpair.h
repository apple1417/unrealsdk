#ifndef UNREALSDK_UNREAL_STRUCTS_TPAIR_H
#define UNREALSDK_UNREAL_STRUCTS_TPAIR_H

namespace unrealsdk::unreal {

UNREALSDK_UNREAL_STRUCT_PADDING_PUSH()

template <typename KeyType, typename ValueType>
struct TPair {
    KeyType key;
    ValueType value;
};

UNREALSDK_UNREAL_STRUCT_PADDING_POP()

}  // namespace unrealsdk::unreal

#endif /* UNREALSDK_UNREAL_STRUCTS_TPAIR_H */
