<img src="https://res.cloudinary.com/dzljlz2nn/image/upload/e_blur:35/jxpbtfgxehqvyrlilcrf" alt="Banner" width="1024">

# Bego-C: True Hardware-Level Input Simulation
[![Discord](https://img.shields.io/badge/Discord-fff?style=flat&logoColor=white&colorA=18181B&colorB=d55ced&logo=discord)](https://discord.com/users/896087803656560681)
[![CMake](https://img.shields.io/badge/Built%20with%20CMake-fff?style=flat&logoColor=white&colorA=18181B&colorB=d55ced&logo=cmake)](https://cmake.org)
[![License](https://img.shields.io/badge/License%20MIT-fff?style=flat&colorB=d55ced)](LICENSE)

> 💡 The name "Bego" comes from Indonesian language, meaning "stupid" or "silly." It was chosen because the creator humorously admits: "I made this input simulation because I'm terrible at playing games, IFYKYK"

Bego-C is a C++ library that provides genuine hardware-level keyboard and mouse input simulation through low-level Windows APIs. Unlike higher-level solutions, Bego-C directly injects input events into the Windows message queue at the driver level.

## ⚠️ Important Disclaimer: Research & Educational Purpose Only

**Bego-C was created as a research project to study anti-cheat systems in games.** This library is intended **STRICTLY FOR EDUCATIONAL PURPOSES**. Do not use it to bypass anti-cheat systems, violate terms of service, or create unfair advantages in multiplayer environments.

## 🔍 Technical Proof of Hardware-Level Simulation

Bego-C's core technical achievement is generating input events that are functionally identical to those produced by physical hardware. Here's how we achieve this at the code level:

### 1. Direct SendInput Implementation

At the core of Bego-C is the `send_input` function in `src/input_helpers.cpp` that directly interfaces with the Windows API:

```cpp
void send_input(const std::vector<INPUT>& input) {
    // Get the size of INPUT structure
    const int input_size = sizeof(INPUT);
    // Get the number of input events
    const UINT input_len = static_cast<UINT>(input.size());
    
    // Direct call to Windows API - this is the exact same pathway used by device drivers
    UINT result = SendInput(input_len, const_cast<LPINPUT>(input.data()), input_size);
    
    if (result != input_len) {
        DWORD error_code = GetLastError();
        throw InputError(InputError::Type::Simulate, 
            "Not all input events were sent. They may have been blocked by UIPI. Error code: " + 
            std::to_string(error_code));
    }
}
```

This function sends input data through the exact same pathway as hardware device drivers, bypassing any application-level abstractions.

### 2. Hardware-Accurate Keyboard Event Generation

For keyboard events, we carefully replicate the low-level details used by hardware, including proper scan code translation and extended key handling in `src/bego_keyboard.cpp`:

```cpp
void Bego::raw(uint16_t scan, Direction direction) {
    std::vector<INPUT> input;
    
    // Translate hardware scan code to virtual key - critical for proper key mapping
    WORD vk = translate_key(scan, MAPVK_VSC_TO_VK_EX);
    
    // Set KEYEVENTF_SCANCODE flag to indicate we're using hardware scan codes
    DWORD keyflags = KEYEVENTF_SCANCODE;
    
    // Handle extended keys exactly as hardware does
    if (is_extended_key(static_cast<VIRTUAL_KEY>(vk))) {
        keyflags |= KEYEVENTF_EXTENDEDKEY;
    }
    
    // Generate key down event with proper flags and scan code
    if (direction == Direction::Click || direction == Direction::Press) {
        input.push_back(create_keybd_event(keyflags, vk, scan, dw_extra_info));
    }
    
    // Generate key up event with proper flags and scan code
    if (direction == Direction::Click || direction == Direction::Release) {
        input.push_back(create_keybd_event(keyflags | KEYEVENTF_KEYUP, vk, scan, dw_extra_info));
    }
    
    // Send the input through the hardware-level pathway
    send_input(input);
}
```

The use of raw scan codes and proper handling of extended key flags ensures the keyboard events are byte-for-byte identical to real hardware.

### 3. Precise Mouse Coordinate System

Our mouse input emulation in `src/bego_mouse.cpp` uses the exact same coordinate transformation as hardware devices:

```cpp
void Bego::move_mouse(int x, int y, Coordinate coordinate) {
    DWORD flags;
    int dx, dy;
    
    if (coordinate == Coordinate::Abs) {
        // For absolute coordinates, we convert to normalized 0-65535 range
        // This is EXACTLY what hardware mouse drivers do
        auto [screen_width, screen_height] = main_display();
        
        // Subtract 1 from dimensions as per Microsoft's driver specs
        int w = screen_width - 1;
        int h = screen_height - 1;
        
        // This exact calculation is what hardware devices use
        dx = (x * 65535 + w / 2 * (x >= 0 ? 1 : -1)) / w;
        dy = (y * 65535 + h / 2 * (y >= 0 ? 1 : -1)) / h;
        
        flags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    } else if (windows_subject_to_mouse_speed_and_acceleration_level) {
        // For relative movement with acceleration
        dx = x;
        dy = y;
        flags = MOUSEEVENTF_MOVE;
    } else {
        // For precise relative movement without acceleration
        auto [current_x, current_y] = location();
        return move_mouse(current_x + x, current_y + y, Coordinate::Abs);
    }
    
    INPUT input = create_mouse_event(flags, 0, dx, dy, dw_extra_info);
    send_input({input});
}
```

The coordinate normalization and transformation algorithms match exactly what hardware drivers implement.

### 4. System-Level Event Timing

By setting the `.time = 0` field in INPUT structures, we ensure the OS timestamps the events just like hardware:

```cpp
INPUT create_keybd_event(DWORD flags, WORD vk, WORD scan, size_t dw_extra_info) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    
    input.ki.wVk = vk;
    input.ki.wScan = scan;
    input.ki.dwFlags = flags;
    input.ki.time = 0;  // System timestamps it identical to hardware
    input.ki.dwExtraInfo = static_cast<ULONG_PTR>(dw_extra_info);
    
    return input;
}
```

When `time = 0`, Windows assigns the current system time to the event - the same behavior as with hardware-generated events.

## 🛡️ Anti-Cheat Considerations

- **Kernel vs. User Mode**: Modern anti-cheat systems (Vanguard, BattlEye) operate at kernel level and can potentially detect user-mode input simulation. Keyboard input is generally harder to detect than mouse movement.

- **Inhuman Patterns**: Even with perfect hardware-level simulation, inputs that show inhuman precision, timing, or consistency will trigger suspicion regardless of how they were generated.

### Detection Challenges for Anti-Cheat Systems

From a technical perspective, Bego-C's keyboard input simulation is particularly challenging for anti-cheat systems to detect through purely automated means. This is because:

1. Keyboard events are structurally identical to those generated by physical hardware
2. The Windows message queue processes these events through the same exact pathway
3. The SendInput API is a legitimate Windows function used by many applications

However, this does not mean such inputs are completely undetectable:

- **👁️ Manual Review**: Player reports and suspicious activity flags can trigger manual reviews by game moderators
- **📊 Behavioral Analysis**: Patterns that appear inhuman (perfect timing, identical repeat actions) can be flagged regardless of how technically perfect the input simulation is
- **🎮 Contextual Detection**: Certain keypresses that don't match expected human behavior in specific game contexts might raise suspicions
- **📈 Statistical Analysis**: Extended play sessions may reveal statistical anomalies in input patterns that differ from genuine human input

This research helps understand the technical boundaries between simulation and detection, which is valuable for both offensive security testing and defensive anti-cheat development.

## ✨ Key Technical Features

- **⌨️ Hardware Scan Code Support**: Direct use of hardware scan codes bypasses virtual key abstractions
- **🔄 Extended Key Flag Handling**: Proper KEYEVENTF_EXTENDEDKEY flags for navigation/special keys
- **🖱️ Hardware-Accurate Coordinate System**: Uses same normalized 0-65535 coordinates as physical devices
- **🔍 Event Field-Level Precision**: Every field in INPUT structures matches hardware-generated values
- **⏱️ System Timestamp Integration**: Events receive exact same system timestamps as hardware input

## ⚙️ Configuration Settings

Bego-C provides several configuration options through its `Settings` class to fine-tune the input simulation behavior:

```cpp
// Default constructor sets these values:
Settings::Settings() 
    : windows_dw_extra_info(EVENT_MARKER),             // 0x12345678 by default
      release_keys_when_dropped(true),                 // Auto-release keys
      windows_subject_to_mouse_speed_and_acceleration_level(false) {
}
```

### Configuration Parameters

- **windows_dw_extra_info** (default: `EVENT_MARKER` / 0x12345678)
  - Sets the `dwExtraInfo` field in all INPUT structures sent to the system
  - This marker helps identify events generated by the library 
  - Can be changed if needed, but default value works in most cases
  - Example: `settings.windows_dw_extra_info = 0x87654321;`

- **release_keys_when_dropped** (default: `true`)
  - When set to true, any keys or mouse buttons still being "held down" will be automatically released when the Bego object is destroyed
  - Prevents "stuck keys" if your application crashes or exits unexpectedly
  - Set to false only if you need to maintain key states between instances
  - Example: `settings.release_keys_when_dropped = false;`

- **windows_subject_to_mouse_speed_and_acceleration_level** (default: `false`)
  - When false, mouse movement bypasses Windows acceleration/precision settings
  - Provides more precise and predictable mouse movement
  - Set to true only if you want to respect user's acceleration settings
  - Example: `settings.windows_subject_to_mouse_speed_and_acceleration_level = true;`

## 📝 Usage Examples

### Basic Setup

```cpp
#include <bego.h>
#include <bego_win.h>

int main() {
    // Enable DPI awareness for accurate mouse positioning
    bego::set_dpi_awareness();
    
    // Configure settings
    bego::Settings settings;
    settings.windows_subject_to_mouse_speed_and_acceleration_level = false; // Bypass Windows acceleration
    settings.release_keys_when_dropped = true;                              // Auto-release keys when destroyed
    
    // Initialize the simulator
    bego::Bego bego(settings);
    
    // Use the library...
    
    return 0;
}
```

### Hardware-Level Keyboard Input

```cpp
// Regular text typing
bego.text("This text is typed with hardware-level events");

// Individual key presses
bego.key(bego::Key::Return, bego::Direction::Click);  // Press and release Enter
bego.key(bego::Key::Space, bego::Direction::Click);   // Press and release Space

// Press and hold keys (useful for gaming or keyboard shortcuts)
bego.key(bego::Key::W, bego::Direction::Press);       // Press and hold W
// ... do something while W is held down ...
bego.key(bego::Key::W, bego::Direction::Release);     // Release W

// Keyboard shortcuts
bego.key(bego::Key::Control, bego::Direction::Press);
bego.key(bego::Key::C, bego::Direction::Click);       // Ctrl+C (Copy)
bego.key(bego::Key::Control, bego::Direction::Release);

// Raw hardware scan code input (lowest level)
uint16_t a_scan = 0x1E;                               // Scan code for 'A'
bego.raw(a_scan, bego::Direction::Click);             // Press using raw scan code
```

### Hardware-Level Mouse Input

```cpp
// Get display dimensions
auto [width, height] = bego.main_display();

// Absolute mouse positioning (coordinates are screen positions)
bego.move_mouse(width/2, height/2, bego::Coordinate::Abs);  // Move to screen center

// Relative mouse movement
bego.move_mouse(100, 0, bego::Coordinate::Rel);             // Move 100 pixels right
bego.move_mouse(0, -50, bego::Coordinate::Rel);             // Move 50 pixels up

// Mouse buttons
bego.button(bego::Button::Left, bego::Direction::Click);    // Left click
bego.button(bego::Button::Right, bego::Direction::Click);   // Right click

// Double-click
bego.button(bego::Button::Left, bego::Direction::Click);
std::this_thread::sleep_for(std::chrono::milliseconds(50));
bego.button(bego::Button::Left, bego::Direction::Click);

// Press and drag
bego.button(bego::Button::Left, bego::Direction::Press);    // Press and hold
bego.move_mouse(100, 100, bego::Coordinate::Rel);           // Drag while holding
bego.button(bego::Button::Left, bego::Direction::Release);  // Release

// Scrolling
bego.scroll(10, bego::Axis::Vertical);                      // Scroll down
bego.scroll(-10, bego::Axis::Vertical);                     // Scroll up
bego.scroll(5, bego::Axis::Horizontal);                     // Scroll right
```

### Practical Example: Auto-Clicker

```cpp
#include <bego.h>
#include <bego_win.h>
#include <thread>
#include <atomic>
#include <iostream>

std::atomic<bool> running = true;

int main() {
    // Handle Ctrl+C to exit gracefully
    signal(SIGINT, [](int) { running = false; });
    
    bego::Bego bego(bego::Settings());
    std::cout << "Auto-clicking every 200ms. Press Ctrl+C to exit." << std::endl;
    
    // Get current position for clicking
    auto [x, y] = bego.location();
    
    // Auto-click loop
    while (running) {
        bego.button(bego::Button::Left, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    std::cout << "Auto-clicker stopped." << std::endl;
    return 0;
}
```

### Advanced Example: Gaming Input Simulation

```cpp
// WASD movement with sprinting (Shift)
bego.key(bego::Key::W, bego::Direction::Press);           // Start moving forward
bego.key(bego::Key::Shift, bego::Direction::Press);       // Start sprinting
std::this_thread::sleep_for(std::chrono::seconds(2));     // Sprint for 2 seconds
bego.key(bego::Key::Shift, bego::Direction::Release);     // Stop sprinting
std::this_thread::sleep_for(std::chrono::seconds(1));     // Walk for 1 second
bego.key(bego::Key::W, bego::Direction::Release);         // Stop moving

// Aim and shoot
bego.move_mouse(960, 540, bego::Coordinate::Abs);         // Center aim
bego.button(bego::Button::Left, bego::Direction::Click);  // Fire weapon
```

## 🔧 Requirements

- C++17 compiler
- Windows operating system

## 🔨 Building

The project uses CMake for building:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## 📄 License

This library is open source and available under the [MIT License](LICENSE).

## 👏 Technical Background and Acknowledgements

Bego-C is directly inspired by and serves as a C++ port of the Rust Enigo library. The project was created to study how anti-cheat systems in games detect and prevent automated inputs, while implementing similar functionality using modern C++ features. This educational research tool maintains the same hardware-level simulation capabilities of the original Enigo library, while exploring the technical boundaries of input simulation and detection systems.

The library leverages undocumented aspects of the Windows input subsystem to achieve the highest level of hardware simulation fidelity possible without actual device driver development, making it a valuable resource for understanding both the offensive and defensive sides of input automation security. 