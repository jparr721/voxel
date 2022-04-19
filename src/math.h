#pragma once

#include <Eigen/Dense>
#include <cinttypes>
#include <cstddef>
#include <cstdint>

namespace vx {
    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;
    using usize = std::size_t;
    using f32 = float;
    using f64 = double;

    using vec2i = Eigen::Vector2i;
    using vec2f = Eigen::Vector2f;
    using vec3f = Eigen::Vector3f;
    using mat4f = Eigen::Matrix4f;
    using rmat4f = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;

    template<typename T>
    using vec = Eigen::Matrix<T, -1, 1>;

    template<typename T>
    using mat = Eigen::Matrix<T, -1, -1>;

    template<unsigned dim, typename T>
    auto identity() -> Eigen::Matrix<T, dim, dim, Eigen::RowMajor> {
        return Eigen::Matrix<T, dim, dim, Eigen::RowMajor>::Identity();
    }
}// namespace vx
