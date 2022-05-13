#include "key_input.h"
#include "../gfx/glfw.h"
#include <spdlog/spdlog.h>

namespace vx::ctrl {
    KeyInput *KeyInput::keyInput_ = nullptr;
    auto KeyInput::getInstance() -> KeyInput * {
        if (keyInput_ == nullptr) { keyInput_ = new KeyInput(); }
        return keyInput_;
    }

    void KeyInput::handleKeyPressEvent(int key, int scancode, int action, int mods) {
        // If the key is released, we don't want to double-activate the key. Alternatively, if we have no registered
        // callbacks, exit under that circumstance as well.
        if (action == GLFW_RELEASE || callbacks_.empty()) { return; }

        // If the key field is focused, we will add input to the string.
        for (auto &[buf, state] : callbacks_) {}
    }

    void KeyInput::registerImGuiKeyCallback(const std::string &name) {
        // Since we're using this with ImGui, we don't want to double-register, so this will silently
        // ignore repeated requests so that way we do not wipe the state
        if (doesKeyExist(name)) { return; }
        callbacks_.insert({name, KeyState()});
    }

    void KeyInput::removeImGuiKeyCallback(const std::string &name) {
        if (!doesKeyExist(name)) {
            spdlog::error("Failed to deregister callback {}", name);
            return;
        }
        callbacks_.erase(name);
    }

    void KeyInput::setLastKey(const std::string &name, u8 key) {
        if (!doesKeyExist(name)) {
            spdlog::error("Identifier not found {}", name);
            return;
        }
        callbacks_.at(name).lastKey = key;
    }

    void KeyInput::setFocused(const std::string &name, bool focused) {
        if (!doesKeyExist(name)) {
            spdlog::error("Identifier not found {}", name);
            return;
        }
        callbacks_.at(name).focused = focused;
    }

    auto KeyInput::getCallbackState(const std::string &name) -> std::optional<KeyInput::KeyState> {
        if (!doesKeyExist(name)) {
            spdlog::error("Identifier not found {}", name);
            return std::nullopt;
        }
        return callbacks_.at(name);
    }

    auto KeyInput::doesKeyExist(const std::string &name) const -> bool {
        return callbacks_.find(name) != callbacks_.end();
    }
}// namespace vx::ctrl
