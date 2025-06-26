#pragma once

#define OB_NO_COPY(CLASS)                     \
    CLASS() = default;                        \
    CLASS(const CLASS &) = delete;            \
    CLASS &operator=(const CLASS &) = delete; \
    CLASS(CLASS &&) = default;                \
    CLASS &operator=(CLASS &&) = default
