#include "unrealsdk/pch.h"
#include "unrealsdk/unreal/structs/fpropertychangeevent.h"

namespace unrealsdk::unreal {

class ZProperty;

FEditPropertyChain::FEditPropertyChain(const std::vector<ZProperty*>& chain)
    : size(static_cast<uint32_t>(chain.size())) {
    this->nodes.resize(chain.size());

    size_t idx = 0;
    for (auto prop : chain) {
        // We just set the size
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
        this->nodes[idx].value = prop;

        if (idx > 0) {
            this->nodes[idx].prev = &this->nodes[idx - 1];
        }
        if (idx < (chain.size() - 1)) {
            this->nodes[idx].next = &this->nodes[idx + 1];
        }
        // NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)

        idx++;
    }

    if (!chain.empty()) {
        this->head = &this->nodes.front();
        this->tail = &this->nodes.back();
    }
}

}  // namespace unrealsdk::unreal
