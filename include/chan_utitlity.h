#pragma once
#include <bits/ranges_algo.h>

#include <functional>
#include <random>
#include <ranges>

#include "IChannel.h"
#include "OChannel.h"
#include "channel.h"
#include "circular_buffer.h"

namespace go {

  class Case {
    // returns false means task(param) executed && param != nil
    std::function<bool()> task;

   public:
    Case() {
      task = []() { return true; };
    }
    Case(const Case &) = default;

    template<typename T, std::size_t Buffer_size, typename func>
    Case(IChan<T, Buffer_size> ch, func fn) {
      task = [=]() -> bool {
        std::optional<std::unique_ptr<T>> exist = ch.m_buffer_->tryGetNextValue();
        if (exist) {
          std::unique_ptr<T> ptr = std::move(*exist);
          fn(*ptr);
        }
        return exist == std::nullopt;
      };
    }

    template<typename T, std::size_t Buffer_size, typename func>
    Case(go::Chan<T, Buffer_size> ch, func fn) : Case(IChan<T, Buffer_size>(ch), std::forward<func>(fn)) {}

    template<typename T, std::size_t Buffer_size, typename func>
    Case(OChan<T, Buffer_size> ch, func fn) {
      task = [=]() -> bool {
        fn();
        return true;
      };
    }

    bool operator()() const { return task(); }
  };

  class Default {
    std::function<void()> task;

   public:
    template<typename func>
    explicit Default(func fn) {
      task = fn;
    }
    void operator()() const { task(); }
  };

  class Select {
   public:
    void exec(Case &&cs) {
      cases.emplace_back(std::move(cs));
      randExec();// Case as select param, ignore return value
    }

    void exec(Default &&def) {
      if (!randExec()) {
        def();
      }
    }

    template<typename... Args>
    explicit Select(Args &&...params) {
      cases.reserve(sizeof...(params));
      exec(std::forward<Args>(params)...);
    }

    template<typename... Args>
    void exec(Case &&cs, Args &&...params) {
      cases.emplace_back(std::move(cs));
      exec(std::forward<Args>(params)...);
    }

    // we only accept at most one Default inside Select
    // also, we only accept Default to place at the end of Select
    template<typename... Args>// when enable_if_t<true>, exec() = delete
    std::enable_if_t<sizeof...(Args) != 0> exec(Default &&df, Args &&...params) = delete;

    bool randExec() {
      const auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
      std::mt19937 mt(seed);
      std::ranges::shuffle(cases, mt);

      // in all cases, if one case successfully pass param into lambda in Case,
      // !c() == true, expr returns true
      return std::ranges::any_of(cases, [&](auto &c) { return !c(); });
    }

   private:
    std::vector<Case> cases;
  };

  template<typename T, std::size_t Buffer_size>
  void Close(OChan<T, Buffer_size> ch) {
    ch.close();
  }

  template<typename T, std::size_t Buffer_size = 1>
  auto make_chan() -> Chan<T, Buffer_size> {
    return Chan<T, Buffer_size>();
  }

}// namespace go
