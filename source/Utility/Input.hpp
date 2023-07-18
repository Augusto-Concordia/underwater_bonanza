#include "GLFW/glfw3.h"

struct Input {
    enum class KeyState {
        None,
        Pressed,
        Holding,
        Released
    };

    inline static double cursor_x = 0.0, cursor_y = 0.0, cursor_delta_x = 0.0, cursor_delta_y = 0.0;
    inline static std::unordered_map<int, KeyState> current_key_state;

    static void KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods) {
        switch (_action) {
            case GLFW_PRESS:
                current_key_state[_key] = KeyState::Pressed;
                break;
            case GLFW_REPEAT:
                current_key_state[_key] = KeyState::Holding;
                break;
            case GLFW_RELEASE:
                current_key_state[_key] = KeyState::Released;
                break;
            default:
                break;
        }
    }

    static void PreEventsPoll(GLFWwindow* _window) {
        for (auto& key_state : current_key_state) {
            if (key_state.second == KeyState::Released) key_state.second = KeyState::None;
        }
    }

    static void PostEventsPoll(GLFWwindow* _window) {
        double now_cursor_x, now_cursor_y;

        //get the current cursor position, regardless if it has moved or not
        glfwGetCursorPos(_window, &now_cursor_x, &now_cursor_y);

        cursor_delta_x = cursor_x - now_cursor_x;
        cursor_delta_y = cursor_y - now_cursor_y;

        cursor_x = now_cursor_x;
        cursor_y = now_cursor_y;
    }

    static bool IsKeyPressed(GLFWwindow* _window, const int _desiredKey) {
        return current_key_state.contains(_desiredKey) && current_key_state[_desiredKey] == KeyState::Pressed || current_key_state[_desiredKey] == KeyState::Holding;
    }

    static bool IsKeyReleased(GLFWwindow* _window, const int _desiredKey) {
        return current_key_state.contains(_desiredKey) && current_key_state[_desiredKey] == KeyState::Released;
    }

    static bool IsMouseButtonPressed(GLFWwindow* _window, const int _desiredButton) {
        return glfwGetMouseButton(_window, _desiredButton) == GLFW_PRESS;
    }
};