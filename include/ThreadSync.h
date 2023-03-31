#pragma once

namespace Xceed {

    namespace ThreadSync {
        template<class T>
        static T* safeAcquire(std::deque<T>* unsafe_buffer, std::mutex& m, std::condition_variable& trigger) {
            std::unique_lock<std::mutex> lock{m};
            while (unsafe_buffer->empty()) {
                trigger.wait(lock, [&] {
                    return !unsafe_buffer->empty();
                });
            }
            T* acquired = new T(unsafe_buffer->front());
            unsafe_buffer->pop_front();

            return acquired;
        }
    };

}
