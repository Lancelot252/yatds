#ifndef DSAAC_FATAL_HPP
#define DSAAC_FATAL_HPP

#include <stdexcept>
#include <string>

namespace dsaac {

[[noreturn]] inline void fatal_error(const std::string& message)
{
    throw std::runtime_error(message);
}

} // namespace dsaac

#endif
