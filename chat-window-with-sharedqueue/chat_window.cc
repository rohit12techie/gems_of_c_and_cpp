#include "chat_window.h"

ChatWindow::ChatWindow() : type_window_ptr(nullptr, &delwin), display_window_ptr(nullptr, &delwin){
    initscr();
    refresh(); 
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int height, width;
    getmaxyx(stdscr, height, width);

    display_window_height_ = height - 2;
    display_window_width_ = (width / 2) - 2;
    type_window_height_ = height - 2;
    type_window_width_ = width - display_window_width_ - 5;

    display_window_ptr.reset(newwin(display_window_height_, display_window_width_, 1, 1));
    type_window_ptr.reset(newwin(type_window_height_, type_window_width_, 1, display_window_width_ + 3));
}

ChatWindow::~ChatWindow() {
    endwin();
}

void ChatWindow::run() {
    scrollok(display_window_ptr.get(), TRUE);
    scrollok(type_window_ptr.get(), TRUE);

    // Start threads for input and display windows
    std::thread type_thread(&ChatWindow::onTypeWindow, this);
    std::thread display_thread(&ChatWindow::onDisplayWindow, this);

    type_thread.join();
    display_thread.join();
}

void ChatWindow::onDisplayWindow() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx_);
	    cv_.wait(lock, [&]{ return message_ready_; });
        
        shared_queue_.subscribe([this](const std::string& message) {
            message_buffer_.push_back(message);
        });

        if (message_buffer_.size() > (unsigned)display_window_height_ - 4) {
            message_buffer_.pop_front();
        }

        werase(display_window_ptr.get());
        box(display_window_ptr.get(), 0, 0);
        mvwprintw(display_window_ptr.get(), 1, 1, "Program-controlled Window (Left)");

        int line = 3;
        for (const auto& msg : message_buffer_) {
            mvwprintw(display_window_ptr.get(), line++, 1, msg.c_str());
        }
        wrefresh(display_window_ptr.get());
        message_ready_ = false;
    }
}

void ChatWindow::onTypeWindow() {
    while (true) {
        // Clear the window
        werase(type_window_ptr.get());
        box(type_window_ptr.get(), 0, 0);
        mvwprintw(type_window_ptr.get(), 1, 1, "User Input Window (Right)");
        mvwprintw(type_window_ptr.get(), 3, 1, "Type here:");
        wrefresh(type_window_ptr.get());

        // Get user input
        std::string userInput;
        int ch;
        while ((ch = wgetch(type_window_ptr.get())) != '\n') {
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
            werase(type_window_ptr.get());
            box(type_window_ptr.get(), 0, 0);
            mvwprintw(type_window_ptr.get(), 1, 1, "User Input Window (Right)");
            mvwprintw(type_window_ptr.get(), 3, 1, "Type here:");
            mvwprintw(type_window_ptr.get(), 3, 11, userInput.c_str()); // Display user input
            wrefresh(type_window_ptr.get());
        }

        //Send user input to the shared message
        std::unique_lock<std::mutex> lock(mtx_);
        //shared_message_ = userInput;
        shared_queue_.publish(userInput);
        message_ready_ = true;
        cv_.notify_one();
    }
}
