#pragma once

#include <memory>

#include "ChannelBuffer.h"
#include "OChannel.h"

namespace go {
  template<typename T, std::size_t Buffer_size>
  class OChan;

  template<typename T, std::size_t Buffer_size = 0>
  class IChan_Iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

   public:
    explicit IChan_Iterator(std::shared_ptr<ChannelBuffer<value_type, Buffer_size>> buffer, bool isEnd = false) : m_buffer_(buffer) {
      if (!isEnd) {
        ++(*this);
      }
    }

    auto operator*() -> value_type & {
      return m_value_;
    }

    auto operator++() -> IChan_Iterator & {
      m_value_ = m_buffer_->getNextValue();
      return *this;
    }

    auto operator++(int) -> IChan_Iterator {
      auto temp = *this;
      ++(*this);
      return temp;
    }

    auto operator==(const IChan_Iterator<value_type, Buffer_size> &other) const -> bool {
      return m_buffer_->isClosed();
    }

    auto operator!=(const IChan_Iterator<value_type, Buffer_size> &other) const -> bool {
      return !operator==(other);
    }

   private:
    std::shared_ptr<ChannelBuffer<value_type, Buffer_size>> m_buffer_;
    value_type m_value_;
  };

  template<typename T, std::size_t Buffer_size = 0>
  class IChan {
    friend class Case;

   protected:
    explicit IChan(std::shared_ptr<ChannelBuffer<T, Buffer_size>> buf) : m_buffer_(buf){};

   public:
    IChan() = default;
    IChan(const IChan<T, Buffer_size> &ch) = default;
    IChan(IChan &&other) noexcept { m_buffer_ = std::move(other.m_buffer_); }

    // extract
    // ch >> res
    friend auto operator>>(IChan<T, Buffer_size> &ch, T &obj) -> IChan<T, Buffer_size> & {
      obj = ch.m_buffer_->getNextValue();
      return ch;
    }
    // res << ch
    friend auto operator<<(T &obj, IChan<T, Buffer_size> &ch) -> IChan<T, Buffer_size> & {
      obj = ch.m_buffer_->getNextValue();
      return ch;
    }

    // cin >> ch
    friend auto operator>>(std::istream &is, IChan<T, Buffer_size> &ch) -> std::istream & {
      T temp;
      is >> temp;
      ch << temp;
      return is;
    }

    template<typename std::size_t OBuffer_size>
    friend auto operator<<(OChan<T, OBuffer_size> &oc, IChan<T, Buffer_size> &ic) -> IChan<T, Buffer_size> & {
      T temp;
      temp << ic;
      temp >> oc;
      return ic;
    }
    template<typename std::size_t OBuffer_size>
    friend auto operator>>(IChan<T, Buffer_size> &ic, OChan<T, OBuffer_size> &oc) -> IChan<T, Buffer_size> & {
      T temp;
      temp << ic;
      temp >> oc;
      return ic;
    }

    auto begin() -> IChan_Iterator<T, Buffer_size> {
      return IChan_Iterator{m_buffer_};
    }

    auto end() -> IChan_Iterator<T, Buffer_size> {
      return IChan_Iterator{m_buffer_, true};
    }

   protected:
    std::shared_ptr<ChannelBuffer<T, Buffer_size>> m_buffer_;
  };
}// namespace go