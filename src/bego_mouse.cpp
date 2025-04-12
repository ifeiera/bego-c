#include "../include/bego_win.h"
#include <vector>

/**
 * @file bego_mouse.cpp
 * @author Eterninety
 * @brief Implementation of hardware-level mouse simulation functions
 * 
 * @note EDUCATIONAL PURPOSE ONLY
 * This library was developed for studying anti-cheat systems in games through penetration testing.
 * It is intended strictly for educational purposes to understand hardware-level input simulation.
 * DO NOT use this to bypass anti-cheat systems or create unfair advantages in games.
 */

namespace bego {

/**
 * @brief Simulates mouse button press, release, or click events
 * 
 * @details This method generates hardware-level mouse button events that are
 * indistinguishable from actual physical mouse inputs. It supports all standard
 * mouse buttons (left, middle, right) as well as additional buttons (back, forward)
 * and scroll wheel actions.
 * 
 * For X buttons (Back/Forward), it properly sets the button_no parameter used
 * by the Windows API to distinguish between different extended buttons.
 * 
 * Scroll wheel buttons are translated into appropriate scroll events rather than
 * button events, matching how real hardware would behave.
 * 
 * @param button The mouse button to simulate
 * @param direction Whether to press, release, or click (press+release) the button
 * @throws InputError If an invalid button type is specified
 */
void Bego::button(Button button, Direction direction) {
    std::vector<INPUT> input;
    
    // Set button data for XBUTTON events
    DWORD button_no = 0;
    if (button == Button::Back) {
        button_no = 1;
    } else if (button == Button::Forward) {
        button_no = 2;
    }
    
    // Handle button press
    if (direction == Direction::Click || direction == Direction::Press) {
        DWORD mouse_event_flag;
        
        switch (button) {
            case Button::Left:
                mouse_event_flag = MOUSEEVENTF_LEFTDOWN;
                break;
            case Button::Middle:
                mouse_event_flag = MOUSEEVENTF_MIDDLEDOWN;
                break;
            case Button::Right:
                mouse_event_flag = MOUSEEVENTF_RIGHTDOWN;
                break;
            case Button::Back:
            case Button::Forward:
                mouse_event_flag = MOUSEEVENTF_XDOWN;
                break;
            case Button::ScrollUp:
                return scroll(-1, Axis::Vertical);
            case Button::ScrollDown:
                return scroll(1, Axis::Vertical);
            case Button::ScrollLeft:
                return scroll(-1, Axis::Horizontal);
            case Button::ScrollRight:
                return scroll(1, Axis::Horizontal);
            default:
                throw InputError(InputError::Type::InvalidInput, "Invalid button type");
        }
        
        input.push_back(create_mouse_event(mouse_event_flag, button_no, 0, 0, dw_extra_info));
    }
    
    // Handle button release
    if (direction == Direction::Click || direction == Direction::Release) {
        DWORD mouse_event_flag;
        
        switch (button) {
            case Button::Left:
                mouse_event_flag = MOUSEEVENTF_LEFTUP;
                break;
            case Button::Middle:
                mouse_event_flag = MOUSEEVENTF_MIDDLEUP;
                break;
            case Button::Right:
                mouse_event_flag = MOUSEEVENTF_RIGHTUP;
                break;
            case Button::Back:
            case Button::Forward:
                mouse_event_flag = MOUSEEVENTF_XUP;
                break;
            case Button::ScrollUp:
            case Button::ScrollDown:
            case Button::ScrollLeft:
            case Button::ScrollRight:
                // Scroll buttons have no effect on release
                return;
            default:
                throw InputError(InputError::Type::InvalidInput, "Invalid button type");
        }
        
        input.push_back(create_mouse_event(mouse_event_flag, button_no, 0, 0, dw_extra_info));
    }
    
    send_input(input);
}

/**
 * @brief Simulates mouse wheel scrolling
 * 
 * @details This method generates hardware-level mouse wheel events that match
 * the behavior of physical scroll wheels. It supports both vertical (standard)
 * and horizontal scrolling, properly setting the appropriate Windows API flags.
 * 
 * The method uses the standard WHEEL_DELTA constant defined by Windows to ensure
 * that the scroll amount matches what physical hardware would produce. For vertical
 * scrolling, the value is inverted to match the expected direction in Windows.
 * 
 * @param length The amount to scroll (positive or negative)
 * @param axis Whether to scroll horizontally or vertically
 */
void Bego::scroll(int length, Axis axis) {
    // Using the Windows-defined WHEEL_DELTA constant
    DWORD flags;
    int data;
    
    if (axis == Axis::Horizontal) {
        flags = MOUSEEVENTF_HWHEEL;
        data = length * WHEEL_DELTA;
    } else {
        flags = MOUSEEVENTF_WHEEL;
        data = -length * WHEEL_DELTA; // Invert for vertical
    }
    
    INPUT input = create_mouse_event(flags, data, 0, 0, dw_extra_info);
    send_input({input});
}

/**
 * @brief Moves the mouse cursor to the specified position
 * 
 * @details This method generates hardware-level mouse movement events that
 * are indistinguishable from real mouse hardware. It supports both absolute
 * positioning (moving to specific screen coordinates) and relative movement.
 * 
 * For absolute positioning, the method converts screen coordinates to the
 * normalized 0-65535 range required by the Windows API. This ensures proper
 * positioning across different screen resolutions and DPI settings.
 * 
 * For relative movement, the method can either respect the system's mouse
 * acceleration settings or bypass them for more predictable movement, based
 * on the configuration.
 * 
 * @param x The x-coordinate or x-distance to move
 * @param y The y-coordinate or y-distance to move
 * @param coordinate Whether the coordinates are absolute or relative
 */
void Bego::move_mouse(int x, int y, Coordinate coordinate) {
    DWORD flags;
    int dx, dy;
    
    if (coordinate == Coordinate::Abs) {
        // For absolute coordinates, we need to convert to the range 0-65535
        auto [screen_width, screen_height] = main_display();
        
        // Subtract 1 from dimensions as per Microsoft documentation
        int w = screen_width - 1;
        int h = screen_height - 1;
        
        // Scale coordinates to 0-65535 range
        dx = (x * 65535 + w / 2 * (x >= 0 ? 1 : -1)) / w;
        dy = (y * 65535 + h / 2 * (y >= 0 ? 1 : -1)) / h;
        
        flags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    } else if (windows_subject_to_mouse_speed_and_acceleration_level) {
        // For relative movement with acceleration
        dx = x;
        dy = y;
        flags = MOUSEEVENTF_MOVE;
    } else {
        // For relative movement without acceleration, calculate absolute position
        auto [current_x, current_y] = location();
        return move_mouse(current_x + x, current_y + y, Coordinate::Abs);
    }
    
    INPUT input = create_mouse_event(flags, 0, dx, dy, dw_extra_info);
    send_input({input});
}

/**
 * @brief Gets the dimensions of the main display
 * 
 * @details This method retrieves the width and height of the primary display
 * using the Windows API. This information is used for various calculations,
 * particularly for absolute mouse positioning.
 * 
 * @return A pair containing the width and height of the main display
 * @throws InputError If the screen dimensions could not be retrieved
 */
std::pair<int, int> Bego::main_display() {
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    
    if (width == 0 || height == 0) {
        throw InputError(InputError::Type::Simulate, "Could not get the dimensions of the screen");
    }
    
    return {width, height};
}

/**
 * @brief Gets the current mouse cursor position
 * 
 * @details This method retrieves the current position of the mouse cursor
 * in screen coordinates using the Windows API.
 * 
 * @return A pair containing the current x and y coordinates of the cursor
 * @throws InputError If the cursor position could not be retrieved
 */
std::pair<int, int> Bego::location() {
    POINT point;
    
    if (!GetCursorPos(&point)) {
        throw InputError(InputError::Type::Simulate, "Could not get the current mouse location");
    }
    
    return {point.x, point.y};
}

} // namespace bego 