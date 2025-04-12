#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <string>
#include <iomanip>
#include <Windows.h>
#include "../include/bego_win.h"

// Global flags to track program state
std::atomic<bool> g_running = true;
std::atomic<bool> g_x2_pressed = false;
std::atomic<int> g_key_count = 0;

// Print a header with nice formatting
void printHeader() {
    std::cout << "\n===========================================================" << std::endl;
    std::cout << "             BEGO-C AUTO-PRESS EXAMPLE" << std::endl;
    std::cout << "===========================================================" << std::endl;
    std::cout << "This example demonstrates hardware-level auto-pressing using Bego-C." << std::endl;
    std::cout << "Instructions:" << std::endl;
    std::cout << "  1. Press and hold Mouse Forward (X2) button to trigger auto-press" << std::endl;
    std::cout << "  2. The program will simulate 'k' key presses at hardware level" << std::endl;
    std::cout << "  3. Press [ESC] key at any time to exit" << std::endl;
    std::cout << "===========================================================" << std::endl;
}

// Thread function to poll button state
void pollButtonState() {
    std::cout << "Button polling thread started..." << std::endl;
    
    try {
        int lastStatusUpdate = 0;
        
        while (g_running) {
            // Check if X2 button is pressed (Mouse Forward button)
            bool isPressed = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;
            
            // If state changed to pressed, print message
            if (isPressed && !g_x2_pressed) {
                std::cout << "[" << std::setw(8) << GetTickCount() << "] X2 Button PRESSED - Starting auto-press mode" << std::endl;
            }
            // If state changed to released, print message
            else if (!isPressed && g_x2_pressed) {
                std::cout << "[" << std::setw(8) << GetTickCount() << "] X2 Button RELEASED - Stopping auto-press mode" << std::endl;
            }
            
            // Update global flag with current state
            g_x2_pressed = isPressed;
            
            // Print status update every second
            int currentSec = GetTickCount() / 1000;
            if (isPressed && (currentSec > lastStatusUpdate)) {
                lastStatusUpdate = currentSec;
                std::cout << "[" << std::setw(8) << GetTickCount() << "] Status: Auto-pressing active. Total presses: " 
                          << g_key_count << std::endl;
            }
            
            // Check for ESC key to exit
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                std::cout << "[" << std::setw(8) << GetTickCount() << "] ESC key detected - Terminating program" << std::endl;
                g_running = false;
                break;
            }
            
            // Wait a bit to reduce CPU usage (10ms is fast enough for button checking)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR in polling thread: " << e.what() << std::endl;
        g_running = false;
    }
    
    std::cout << "Button polling thread terminated." << std::endl;
}

int main() {
    try {
        // Set DPI awareness for accurate mouse positioning
        bego::set_dpi_awareness();
        
        // Set up settings for hardware-level simulation
        bego::Settings settings;
        settings.release_keys_when_dropped = true; // Auto-release keys when program exits
        
        // Create the Bego instance
        bego::Bego bego(settings);
        
        // Print header with instructions
        printHeader();
        
        // Start polling thread
        std::thread pollThread(pollButtonState);
        
        // Main loop - handles key press simulation
        while (g_running) {
            try {
                // Check if X2 button is pressed to activate auto-pressing
                if (g_x2_pressed) {
                    // Press 'k' key at hardware level
                    bego.key(bego::Key::K, bego::Direction::Click);
                    g_key_count++;
                    
                    // Slower key repeat rate for more stability (100ms = 10 keys per second)
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                } else {
                    // When not pressing keys, sleep to reduce CPU usage
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
            catch (const bego::InputError& e) {
                // Handle Bego-specific errors
                std::cerr << "BEGO ERROR: " << e.what() << std::endl;
                if (e.get_type() == bego::InputError::Type::Simulate) {
                    std::cerr << "Simulation error - possible privilege issue. Terminating." << std::endl;
                    g_running = false;
                    break;
                }
            }
            catch (const std::exception& e) {
                // Handle other errors
                std::cerr << "ERROR in main loop: " << e.what() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1)); // Pause briefly before continuing
            }
        }
        
        // Wait for poll thread to finish
        std::cout << "Waiting for threads to terminate..." << std::endl;
        if (pollThread.joinable()) {
            pollThread.join();
        }
        
        // Print summary
        std::cout << "\n===========================================================" << std::endl;
        std::cout << "Program terminated." << std::endl;
        std::cout << "Total simulated key presses: " << g_key_count << std::endl;
        std::cout << "===========================================================" << std::endl;
        
    } catch (const bego::InputError& e) {
        std::cerr << "FATAL BEGO ERROR: " << e.what() << std::endl;
        std::cerr << "Error type: " << static_cast<int>(e.get_type()) << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "UNKNOWN FATAL ERROR" << std::endl;
        return 1;
    }
    
    return 0;
} 