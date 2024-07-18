/**
 * chat_window.h
 * Created by [Rohit Kumar] on [18-Jul-2024].
 * Copyright © [2024] [Rohit Kumar]. All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 * */

#include <ncurses.h>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>

class ChatWindow {
  public:
    void run();
    ChatWindow();
    ~ChatWindow();
  private:
    void onTypeWindow();
    void onDisplayWindow();

    std::mutex mtx_;
    std::condition_variable cv_;
    std::string shared_message_;
    bool message_ready_ = false;
    std::deque<std::string> message_buffer_;

    int type_window_height_;
    int type_window_width_;
    int display_window_height_;
    int display_window_width_;
    std::unique_ptr<WINDOW, decltype(&delwin)> type_window_ptr;
    std::unique_ptr<WINDOW, decltype(&delwin)> display_window_ptr;
};