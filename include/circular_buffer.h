#pragma once

#include <memory>
namespace go {

  template<typename T, std::size_t Buffer_size>
  class Circular_buffer {
   private:
    void increment(T*& p) {
      if (p + 1 == &m_buffer_[0] + Buffer_size) {
        p = &m_buffer_[0];
      } else {
        ++p;
      }
    }

   public:
    Circular_buffer() : m_buffer_(std::make_unique<T[]>(Buffer_size)),
                        m_front_(&m_buffer_[0]),
                        m_end_(&m_buffer_[0]),
                        m_size_(0) {
    }

    ~Circular_buffer() = default;

    [[nodiscard]] bool empty() const {
      return m_size_ <= 0;
    }

    [[nodiscard]] bool full() const {
      return m_size_ == Buffer_size;
    }

    auto front() const -> const T& {
      if (empty()) {
        throw std::out_of_range("buffer empty");
      }
      return *m_front_;
    }

    auto front() -> T& {
      if (empty()) {
        throw std::out_of_range("buffer empty");
      }
      return *m_front_;
    }

    auto back() -> const T& {
      if (empty()) {
        throw std::out_of_range("buffer empty");
      }
      return *m_end_;
    }

    void pop_back() {
      if (empty()) {
        throw std::out_of_range("buffer empty");
      }
      --m_size_;
      increment(m_front_);
    }

    [[nodiscard]] auto size() const -> std::size_t {
      return m_size_;
    }

    void push_back(const T& elem) {
      if (full()) {
        throw std::out_of_range("container is full");
      }
      *m_end_ = elem;
      increment(m_end_);
      ++m_size_;
    }

    void push_back(T&& elem) {
      if (full()) {
        throw std::out_of_range("container is full");
      }
      *m_end_ = std::move(elem);
      increment(m_end_);
      ++m_size_;
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
      push_back(std::forward<T>(args...));
    }

    auto pop_front() -> T {
      if (empty()) {
        throw std::out_of_range("container is empty");
      }
      --m_size_;
      T temp = *m_front_;
      increment(m_front_);
      return temp;
    }

   private:
    std::unique_ptr<T[]> m_buffer_;
    T* m_front_;
    T* m_end_;
    size_t m_size_;
  };

}// namespace go