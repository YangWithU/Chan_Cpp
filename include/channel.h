#pragma once

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>

#include "IChannel.h"
#include "OChannel.h"

namespace go {

  template<typename T, std::size_t Buffer_size = 1>
  class Chan : public IChan<T, Buffer_size>, public OChan<T, Buffer_size> {
   public:
    Chan() {
      Chan::IChan::m_buffer_ = Chan::OChan::m_buffer_ = std::make_shared<ChannelBuffer<T, Buffer_size>>();
    }
    ~Chan() = default;

    // insert to channel
    friend auto operator<<(Chan<T, Buffer_size> &chan, const T &obj) -> OChan<T, Buffer_size> & {
      return static_cast<OChan<T, Buffer_size> &>(chan) << obj;
    }
    friend auto operator>>(const T &obj, Chan<T, Buffer_size> &chan) -> OChan<T, Buffer_size> & {
      return static_cast<OChan<T, Buffer_size> &>(chan) << obj;
    }

    // stream
    // cout << ch
    friend auto operator<<(std::ostream &os, Chan<T, Buffer_size> &ch) -> std::ostream & {
      os << static_cast<OChan<T, Buffer_size>>(ch);
      return os;
    }

    // cin >> ch
    friend auto operator>>(std::istream &is, Chan<T, Buffer_size> &ch) -> std::istream & {
      is >> static_cast<IChan<T, Buffer_size>>(ch);
      return is;
    }

  };// end of Channel definition


}// namespace go
