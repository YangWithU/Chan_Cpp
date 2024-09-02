#pragma once

#include <memory>

#include "ChannelBuffer.h"
#include "IChannel.h"


namespace go {
  template<class T, std::size_t Buffer_size>
  class IChan;

  template<typename T, std::size_t Buffer_size = 0>
  class OChan {
   protected:
    explicit OChan(std::shared_ptr<ChannelBuffer<T, Buffer_size>> buf) : m_buffer_(std::move(buf)){};

   public:
    OChan() = default;
    OChan(const OChan<T, Buffer_size> &ch) = default;
    OChan(OChan &&other) noexcept { m_buffer_ = std::move(other.m_buffer_); }

    friend auto operator<<(OChan<T, Buffer_size> &ch, const T &obj) -> OChan<T, Buffer_size> & {
      ch.m_buffer_->insertValue(obj);
      return ch;
    }

    friend auto operator>>(const T &obj, OChan<T, Buffer_size> &ch) -> OChan<T, Buffer_size> & {
      ch.m_buffer_->insertValue(obj);
      return ch;
    }
    template<std::size_t IBuffer_size>
    friend auto operator<<(OChan<T, Buffer_size> &oc, const IChan<T, IBuffer_size> &ic) -> OChan<T, Buffer_size> & {
      T temp;
      temp << ic;
      temp >> oc;
      return oc;
    }
    template<std::size_t IBuffer_size>
    friend auto operator>>(const IChan<T, IBuffer_size> &ic, OChan<T, Buffer_size> &oc) -> OChan<T, Buffer_size> & {
      T temp;
      temp << ic;
      temp >> oc;
      return oc;
    }

    friend auto operator<<(std::ostream &os, const OChan<T, Buffer_size> &ch) -> std::ostream & {
      os << ch.m_buffer_->getNextValue();
      return os;
    }

    void close() {
      m_buffer_->close();
    }

   protected:
    std::shared_ptr<ChannelBuffer<T, Buffer_size>> m_buffer_;
  };
}// namespace go
