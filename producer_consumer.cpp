#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <numeric>
#include <thread>
#include <vector>

std::deque<int> dq;
std::mutex lock;
std::condition_variable cv;
std::atomic<bool> done;

void produce(const std::vector<int>& vec) {
  for (int v : vec) {
    std::unique_lock<std::mutex> locker(lock);
    dq.push_front(v);
    locker.unlock();
    std::cout << std::this_thread::get_id() << " produced " << v << std::endl;
    cv.notify_all();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  done.store(true);
  cv.notify_all();
}

void consume() {
  while (!(dq.empty() && done.load())) {
    std::unique_lock<std::mutex> locker(lock);
    cv.wait(locker, []() { return !dq.empty() || done.load(); });
    if (!dq.empty()) {
      int num = dq.back();
      dq.pop_back();
      std::cout << std::this_thread::get_id() << " consumed " << num
                << std::endl;
    }
    locker.unlock();
  }
}

int main() {
  std::vector<int> resource(100);
  std::vector<std::thread> threads(10);

  //! Equivalent to resouce = {0, 1, ..., 99}
  std::iota(resource.begin(), resource.end(), 0);

  threads[0] = std::thread(produce, std::ref(resource));
  for (size_t i = 1; i < threads.size(); i++) threads[i] = std::thread(consume);

  for (auto& thread : threads) {
    std::cout << thread.get_id() << " joining" << std::endl;
    if (thread.joinable()) thread.join();
  }

  return 0;
}