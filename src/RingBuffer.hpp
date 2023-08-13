#pragma once

#include <vector>
#include <cassert>

// RingBuffer.hpp
// Dynamic ring buffer. Meant for turntables.
// Perf - let's just remove indexes directly right now.
namespace aa
{
template <typename T>
struct RingBuffer
{
    using buf_t = std::vector<T>;
    using size_type = typename buf_t::size_type;

    // The goal of this type is simple:
    // We need to store our current position and allow us to 'shift' over to give a different view.
    // Also, we should support wraparound.
    size_type pos_{0};
    
    void shift(size_type n = 1) {
        const auto sz = size();
        if (sz == 0) return;
        pos_ = (pos_ + n) % sz;
    }

    auto& buf() { return buf_; }

    auto& get(size_type offset) {
        assert(size() != 0);
        return buf_[(pos_ + offset) % size()];
    }

    auto size() const noexcept { return buf_.size(); }

    std::vector<T> buf_;
};
}  // namespace aa
