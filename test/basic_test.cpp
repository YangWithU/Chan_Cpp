#include <gtest/gtest.h>

#include "chan_utitlity.h"

void chan_fibnacci(go::Chan<int> channel, go::Chan<int> quit) {
  using namespace go;
  int x = 0, y = 1;
  for (bool fin = true; fin;) {
    Select{
            Case{
                    channel << x,
                    [&]() {
                      int t = x;
                      x = y;
                      y += t;
                    }},
            Case{
                    quit, [&](auto v) {
                      std::cout << "End signal received. Now quitting" << std::endl;
                      fin = false;
                    }}};
  }
}

constexpr int64_t cfib(int64_t n) {
  if (n == 0) return 0;
  if (n == 1) return 1;
  return cfib(n - 1) + cfib(n - 2);
}

TEST(ChannelTest, TestFib) {
  go::Chan<int> channel;
  go::Chan<int> quit;

  std::thread t([&]() {
    for (int i = 0; i < 10; i++) {
      auto ans = -1;
      ans << channel;
      EXPECT_EQ(ans, cfib(i));
      std::cout << ans << '\n';
    }
    quit << 0;// send end signal
  });
  t.detach();
  std::thread(chan_fibnacci, channel, quit).detach();
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

template<typename T>
void test2_send(go::Chan<T> &ch, T val, int time) {
  ch << val;
}

TEST(ChannelTest, TestSelectDefault) {
  go::Chan<int> ichan;
  go::Chan<std::string> schan;

  auto t1 = std::thread(test2_send<int>, std::ref(ichan), 114514, 1);
  auto t2 = std::thread(test2_send<std::string>, std::ref(schan), "hello", 2);

  for (bool fin = true; fin;) {
    go::Select{
            go::Case{ichan,
                     [&](int x) {
                       std::cout << "ichan received: " << x << '\n';
                       EXPECT_EQ(x, 114514);
                       fin = false;
                     }},

            go::Case{
                    schan, [](const std::string &x) {
                      std::cout << "schan received: " << x << '\n';
                    }},

            go::Default{[]() {
              std::cout << "waiting for case to receive..." << '\n';
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }}};
  }

  t1.join();
  t2.join();
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(ChannelTest, EarlyDefault) {
  go::Chan<int> ch;
  std::thread([&]() {
    ch << 114514;
  }).detach();

  // code below wouldn't compile:
  // during type instantiation in ctor,
  // after param forward,
  // no such ctor in Select
  // golang::Select{
  //         golang::Default{
  //                 [](int x) {
  //                   std::cout << "chan received: " << x << '\n';
  //                 }},
  //
  //         golang::Case{
  //                 ch, [&](int x) {
  //                   std::cout << "chan received: " << x << '\n';
  //                 }}};

  std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(ChannelTest, BasicRangeTest) {
  go::Chan<int, 10> ch;
  std::thread([&]() {
    for (int i = 0; i < 10; i++) {
      ch << i;
    }
  }).detach();

  std::this_thread::sleep_for(std::chrono::seconds(10));

  int i = 0;
  for (const auto &x: ch) {
    EXPECT_EQ(x, i++);
  }
  go::Close(ch);
}

// has error: notify_one() would randomly choose a cv
// means some cv will wait forever
TEST(ChannelTest, CloseTest) {
  go::Chan<int> ch;
  go::Chan<int> out;
  std::vector<std::thread> threads;
  threads.reserve(10);
  for (int i = 0; i < 10; i++) {
    threads.emplace_back([&]() { out << ch; });
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));

  ch << 1 << 2 << 3 << 4 << 5;
  1 >> 2 >> 3 >> ch;
  go::Close(ch);
  int cnt = 0;
  for (auto &x: out) {
    cnt++;
  }

  for (int i = 0; i < 10; ++i) {
    threads[i].join();
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));

  EXPECT_EQ(cnt, 8);
}

TEST(ChannelTest, BufferedChan) {
  go::Chan<int, 5> ch;
  std::thread([&]() {
    ch << 1 << 2 << 3 << 4 << 5;
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    go::Close(ch);
  }).detach();

  int i = 1;
  for (auto &x: ch) {
    EXPECT_EQ(x, i++);
  }
}

TEST(ChannelTest, SendReceiveCase) {
  go::Chan<int> ch;
  go::Select{
          go::Case{
                  ch,
                  [](int x) {
                    std::cout << "ch received: " << x << '\n';
                  }},
          go::Case{
                  114 >> ch, [&]() {
                    std::cout << "ch sent." << '\n';
                  }},
  };
  std::cout << "done\n";
}


TEST(ChannelTest, CircularBuffer) {
  go::Circular_buffer<int, 3> cb;
  try {
    cb.pop_front();
  } catch (const std::exception &e) {
    std::cout << e.what() << '\n';
  }
  for (int i = 0; i < 3; i++) {
    cb.emplace_back(i + 1);
  }
  ASSERT_EQ(cb.front(), 1);
  ASSERT_EQ(cb.back(), 2);
  try {
    cb.emplace_back(4);
  } catch (const std::exception &e) {
    std::cout << e.what() << '\n';
  }
}