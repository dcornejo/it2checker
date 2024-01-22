#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <ctime>
#include <fcntl.h>
#include <vector>

// Save terminal state
struct termios saved_tio;

void getResponse(std::vector<char> &buffer) {

    clock_t start_time = clock();
    clock_t end_time = (3 * CLOCKS_PER_SEC) + start_time;
    clock_t now_time = 0;

    while (now_time < end_time) {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            buffer.push_back(ch);
        }
        now_time = clock();
    }
}

void configureTerminal() {
    struct termios tio;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &saved_tio);

    // Set IO to non-blocking
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    // New terminal settings: Non-canonical mode
    // Add ECHO off flag as well not to echo the input characters
    tio = saved_tio;
    tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &tio);
}

void restoreTerminal() {
    // Restore terminal state
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_tio);
}

int main() {
    std::vector<char> buffer;

    std::cout << "here we go..." << std::endl;

    // setup the terminal
    configureTerminal();
    // iTerm2 extended device attributes (get iTerm2 version)
    std::cout << "\x1b[>q";
    // read in response
    getResponse(buffer);
    // restore the terminal
    restoreTerminal();

    std::cout << ">" << std::string(buffer.begin(), buffer.end()) << "<" << std::endl;
}
