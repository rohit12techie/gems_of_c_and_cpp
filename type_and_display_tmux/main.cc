#include <ncurses.h>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>

std::mutex mtx;
std::condition_variable cv;
std::string sharedMessage;
bool messageReady = false;
std::deque<std::string> messageBuffer;

void inputWindow(WINDOW* win2) {
    while (true) {
        // Clear the window
        werase(win2);
        box(win2, 0, 0);
        mvwprintw(win2, 1, 1, "User Input Window (Right)");
        mvwprintw(win2, 3, 1, "Type here:");
        wrefresh(win2);

        // Get user input
        std::string userInput;
        int ch;
        while ((ch = wgetch(win2)) != '\n') {
            if (ch == KEY_BACKSPACE || ch == 127) {
                // Handle backspace
                if (!userInput.empty()) {
                    userInput.pop_back();
                }
            } else {
                // Append character to input
                userInput += static_cast<char>(ch);
            }

            // Update right window with user input
            werase(win2);
            box(win2, 0, 0);
            mvwprintw(win2, 1, 1, "User Input Window (Right)");
            mvwprintw(win2, 3, 1, "Type here:");
            mvwprintw(win2, 3, 11, userInput.c_str()); // Display user input
            wrefresh(win2);
        }

        // Send user input to the shared message
        std::unique_lock<std::mutex> lock(mtx);
        sharedMessage = userInput;
        messageReady = true;
        cv.notify_one();
    }
}

void displayWindow(WINDOW* win1, int win1_height) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{ return messageReady; });

        // Add the shared message to the buffer
        messageBuffer.push_back(sharedMessage);
        if (messageBuffer.size() > (unsigned)win1_height - 4) { // Keep the buffer within window height
            messageBuffer.pop_front();
        }

        // Clear and update the left window with buffered messages
        werase(win1);
        box(win1, 0, 0);
        mvwprintw(win1, 1, 1, "Program-controlled Window (Left)");

        int line = 3;
        for (const auto& msg : messageBuffer) {
            mvwprintw(win1, line++, 1, msg.c_str());
        }
        wrefresh(win1);

        messageReady = false;
    }
}

int main() {
    // Initialize ncurses
    initscr();
    cbreak(); // Line buffering disabled
    noecho(); // Don't echo key presses
    keypad(stdscr, TRUE); // Enable keypad for non-char keys
    curs_set(0); // Hide cursor

    // Get window dimensions
    int height, width;
    getmaxyx(stdscr, height, width);

    // Calculate window sizes
    int win1_height = height - 2; // Leave space for border
    int win1_width = width / 2 - 2; // Half of screen width
    int win2_height = height - 2; // Leave space for border
    int win2_width = width - win1_width - 5; // Remaining width

    // Create windows
    WINDOW* win1 = newwin(win1_height, win1_width, 1, 1);
    WINDOW* win2 = newwin(win2_height, win2_width, 1, win1_width + 3);

    // Enable scrolling for both windows
    scrollok(win1, TRUE);
    scrollok(win2, TRUE);

    // Start threads for input and display windows
    std::thread inputThread(inputWindow, win2);
    std::thread displayThread(displayWindow, win1, win1_height);

    // Wait for threads to finish (they won't in this infinite loop example)
    inputThread.join();
    displayThread.join();

    // End ncurses
    endwin();
    return 0;
}
