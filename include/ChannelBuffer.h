#pragma once

#include <memory>
#include <queue>

#include "circular_buffer.h"

template<typename T, std::size_t Buffer_size = 0>
class ChannelBuffer {
 public:
  ChannelBuffer() = default;
  ~ChannelBuffer() = default;

  // used by operator<<, forced to extract elem from Chan
  auto getNextValue() -> T {
    std::unique_lock<std::mutex> ulk(buffer_lock_);
    if (buffer_.empty()) {
      if (isClosed_) {
        return {};
      }
      // only wakes when !buffer.empty
      inputWait_.wait(ulk, [&]() { return !buffer_.empty() || isClosed_; });
      if (buffer_.empty() && isClosed_) {
        return {};
      }
    }
    T temp;
    std::swap(temp, buffer_.front());
    buffer_.pop_back();
    outputWait_.notify_one();
    return temp;
  }

  auto insertValue(T value) -> void {
    if (!isClosed_) {
      {
        std::unique_lock<std::mutex> lock(buffer_lock_);
        if (buffer_.full()) {
          outputWait_.wait(lock, [&]() { return !buffer_.full() || isClosed_; });
          if (isClosed_) {
            return;
          }
        }
        buffer_.push_back(value);
      }
      inputWait_.notify_one();
    }
  }

  // used in Case, called by Select, calls lambda in Case
  auto tryGetNextValue() -> std::optional<std::unique_ptr<T>> {
    if (isClosed_) {
      return std::optional<std::unique_ptr<T>>();
    }

    std::unique_lock<std::mutex> lock(buffer_lock_);
    if (buffer_.empty()) {
      return std::nullopt;
    }
    std::unique_ptr<T> temp = std::make_unique<T>(buffer_.front());
    buffer_.pop_back();
    outputWait_.notify_one();
    return temp;// NRVO
  }

  void close() {
    isClosed_ = true;
    inputWait_.notify_one();
    outputWait_.notify_all();
  }

  bool isClosed() {
    return isClosed_;
  }

 private:
  go::Circular_buffer<T, Buffer_size> buffer_;
  std::mutex buffer_lock_;
  std::condition_variable inputWait_; // block output
  std::condition_variable outputWait_;// block insertion
  std::atomic_bool isClosed_{false};
};
