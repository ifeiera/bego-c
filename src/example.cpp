#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include "../include/bego_win.h"

// Helper function to print section headers
void printSection(const std::string& title) {
    std::cout << "\n--------------------------------------------" << std::endl;
    std::cout << title << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
}

// Helper function to wait with countdown
void waitWithCountdown(int seconds, const std::string& message) {
    std::cout << message << " in ";
    for (int i = seconds; i > 0; i--) {
        std::cout << i << "... ";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Now!" << std::endl;
}

int main() {
    try {
        // Enable DPI awareness for accurate mouse positioning
        bego::set_dpi_awareness();
        
        // Create the settings
        bego::Settings settings;
        
        // Configure settings for hardware-level simulation
        settings.windows_subject_to_mouse_speed_and_acceleration_level = false; // Bypass Windows acceleration
        settings.release_keys_when_dropped = true;                              // Auto-release keys on exit
        settings.windows_dw_extra_info = bego::EVENT_MARKER;                    // Set custom event marker
        
        // Create the Bego instance
        bego::Bego bego(settings);
        
        printSection("BEGO-C HARDWARE-LEVEL INPUT SIMULATION DEMO");
        std::cout << "This program demonstrates various hardware-level input simulation capabilities." << std::endl;
        std::cout << "Please open a text editor before proceeding (e.g., Notepad)." << std::endl;
        
        waitWithCountdown(5, "Starting demo");
        
        // Get display and cursor information
        printSection("SYSTEM INFORMATION");
        auto [width, height] = bego.main_display();
        auto [cursor_x, cursor_y] = bego.location();
        
        std::cout << "Screen dimensions: " << width << "x" << height << std::endl;
        std::cout << "Current cursor position: (" << cursor_x << ", " << cursor_y << ")" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Mouse positioning demo
        printSection("MOUSE POSITION DEMO");
        std::cout << "Moving mouse to different screen positions..." << std::endl;
        
        // Move to center
        std::cout << "Moving to center" << std::endl;
        bego.move_mouse(width/2, height/2, bego::Coordinate::Abs);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Move to top-left
        std::cout << "Moving to top-left" << std::endl;
        bego.move_mouse(100, 100, bego::Coordinate::Abs);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Move to top-right
        std::cout << "Moving to top-right" << std::endl;
        bego.move_mouse(width-100, 100, bego::Coordinate::Abs);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Move to bottom-left
        std::cout << "Moving to bottom-left" << std::endl;
        bego.move_mouse(100, height-100, bego::Coordinate::Abs);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Move to bottom-right
        std::cout << "Moving to bottom-right" << std::endl;
        bego.move_mouse(width-100, height-100, bego::Coordinate::Abs);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Back to center
        std::cout << "Back to center" << std::endl;
        bego.move_mouse(width/2, height/2, bego::Coordinate::Abs);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Relative movement demo
        std::cout << "Demonstrating relative movement..." << std::endl;
        for (int i = 0; i < 4; i++) {
            bego.move_mouse(50, 0, bego::Coordinate::Rel);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            bego.move_mouse(0, 50, bego::Coordinate::Rel);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            bego.move_mouse(-50, 0, bego::Coordinate::Rel);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            bego.move_mouse(0, -50, bego::Coordinate::Rel);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Mouse button demo
        printSection("MOUSE BUTTON DEMO");
        std::cout << "Performing mouse clicks..." << std::endl;
        
        // Left click
        std::cout << "Left click" << std::endl;
        bego.button(bego::Button::Left, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Right click
        std::cout << "Right click" << std::endl;
        bego.button(bego::Button::Right, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Left click to dismiss context menu
        bego.move_mouse(width/2, height/2 + 50, bego::Coordinate::Abs);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bego.button(bego::Button::Left, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Double click demo
        std::cout << "Double click" << std::endl;
        bego.button(bego::Button::Left, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Short delay between clicks
        bego.button(bego::Button::Left, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Press and release demo
        std::cout << "Press and hold, then release" << std::endl;
        bego.button(bego::Button::Left, bego::Direction::Press);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        bego.move_mouse(width/2 + 100, height/2 + 100, bego::Coordinate::Abs);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        bego.button(bego::Button::Left, bego::Direction::Release);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Scroll demo
        printSection("MOUSE SCROLL DEMO");
        std::cout << "Scrolling in different directions..." << std::endl;
        
        // Vertical scroll down
        std::cout << "Scroll down" << std::endl;
        for (int i = 0; i < 5; i++) {
            bego.scroll(3, bego::Axis::Vertical);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Vertical scroll up
        std::cout << "Scroll up" << std::endl;
        for (int i = 0; i < 5; i++) {
            bego.scroll(-3, bego::Axis::Vertical);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Horizontal scroll (if supported by application)
        std::cout << "Horizontal scroll" << std::endl;
        for (int i = 0; i < 3; i++) {
            bego.scroll(3, bego::Axis::Horizontal);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        for (int i = 0; i < 3; i++) {
            bego.scroll(-3, bego::Axis::Horizontal);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Text typing demo
        printSection("KEYBOARD TEXT DEMO");
        std::cout << "Typing text..." << std::endl;
        
        // Make sure we're in a text field - click first
        bego.button(bego::Button::Left, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        
        // Type normal text
        std::cout << "Typing regular text" << std::endl;
        bego.text("This is hardware-level text input from Bego-C.");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        bego.key(bego::Key::Return, bego::Direction::Click);
        
        // Type with special characters
        std::cout << "Typing with special characters" << std::endl;
        bego.text("Special chars: !@#$%^&*()_+-=[]{}\\|;:'\",.<>/?");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        bego.key(bego::Key::Return, bego::Direction::Click);
        
        // Type with line breaks and tabs
        std::cout << "Typing with line breaks and tabs" << std::endl;
        bego.text("Line 1\nLine 2\n\tIndented line\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Individual key press demo
        printSection("KEYBOARD KEY DEMO");
        std::cout << "Demonstrating individual key presses..." << std::endl;
        
        // Function keys
        std::cout << "Function keys" << std::endl;
        bego.key(bego::Key::F1, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        
        // Dismiss help dialog if appeared
        bego.key(bego::Key::Escape, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        
        // Navigation keys
        std::cout << "Navigation keys" << std::endl;
        for (int i = 0; i < 4; i++) {
            bego.key(bego::Key::Right, bego::Direction::Click);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        for (int i = 0; i < 2; i++) {
            bego.key(bego::Key::Down, bego::Direction::Click);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        bego.key(bego::Key::Home, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        bego.key(bego::Key::End, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        
        // Modifier key combinations
        printSection("MODIFIER KEY COMBINATIONS");
        std::cout << "Testing modifier keys..." << std::endl;
        
        // Select all (Ctrl+A)
        std::cout << "Ctrl+A (Select All)" << std::endl;
        bego.key(bego::Key::Control, bego::Direction::Press);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bego.key(bego::Key::A, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bego.key(bego::Key::Control, bego::Direction::Release);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Cut (Ctrl+X)
        std::cout << "Ctrl+X (Cut)" << std::endl;
        bego.key(bego::Key::Control, bego::Direction::Press);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bego.key(bego::Key::X, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bego.key(bego::Key::Control, bego::Direction::Release);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Type new content
        bego.text("Text was cut and will be pasted back.");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        bego.key(bego::Key::Return, bego::Direction::Click);
        
        // Paste (Ctrl+V)
        std::cout << "Ctrl+V (Paste)" << std::endl;
        bego.key(bego::Key::Control, bego::Direction::Press);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bego.key(bego::Key::V, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bego.key(bego::Key::Control, bego::Direction::Release);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Raw scan code demo
        printSection("RAW SCAN CODE DEMO");
        std::cout << "Using raw hardware scan codes..." << std::endl;
        
        // Press Enter using raw scan code (0x1C)
        std::cout << "Pressing Enter using raw scan code 0x1C" << std::endl;
        bego.raw(0x1C, bego::Direction::Click);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        
        // Type 'ABC' using raw scan codes
        std::cout << "Typing 'ABC' using raw scan codes" << std::endl;
        bego.raw(0x1E, bego::Direction::Click); // A
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bego.raw(0x30, bego::Direction::Click); // B
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bego.raw(0x2E, bego::Direction::Click); // C
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        printSection("DEMO COMPLETED");
        std::cout << "All hardware-level input simulation tests completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 