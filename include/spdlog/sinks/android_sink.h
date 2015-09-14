/*************************************************************************/
/* spdlog - an extremely fast and easy to use c++11 logging library.     */
/* Copyright (c) 2014 Gabi Melman.                                       */
/* Copyright (c) 2015 Ruslan Baratov.                                    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#pragma once

#if defined(__ANDROID__)

#include <mutex>
#include "base_sink.h"
#include "../details/null_mutex.h"

#include <android/log.h>

namespace spdlog
{
namespace sinks
{

class switch_converter {
public:
    android_LogPriority convert_to_android(spdlog::level::level_enum level)
    {
        switch(level)
        {
            case spdlog::level::trace: return ANDROID_LOG_VERBOSE;
            case spdlog::level::debug: return ANDROID_LOG_DEBUG;
            case spdlog::level::info: return ANDROID_LOG_INFO;
            case spdlog::level::notice: return ANDROID_LOG_INFO;
            case spdlog::level::warn: return ANDROID_LOG_WARN;
            case spdlog::level::err: return ANDROID_LOG_ERROR;
            case spdlog::level::critical: return ANDROID_LOG_FATAL;
            case spdlog::level::alert: return ANDROID_LOG_FATAL;
            case spdlog::level::emerg: return ANDROID_LOG_FATAL;
        }
    }
};

class array_converter {
public:
    array_converter()
    {
        priorities_[static_cast<int>(spdlog::level::trace)] = ANDROID_LOG_VERBOSE;
        priorities_[static_cast<int>(spdlog::level::debug)] = ANDROID_LOG_DEBUG;
        priorities_[static_cast<int>(spdlog::level::info)] = ANDROID_LOG_INFO;
        priorities_[static_cast<int>(spdlog::level::notice)] = ANDROID_LOG_INFO;
        priorities_[static_cast<int>(spdlog::level::warn)] = ANDROID_LOG_WARN;
        priorities_[static_cast<int>(spdlog::level::err)] = ANDROID_LOG_ERROR;
        priorities_[static_cast<int>(spdlog::level::critical)] = ANDROID_LOG_FATAL;
        priorities_[static_cast<int>(spdlog::level::alert)] = ANDROID_LOG_FATAL;
        priorities_[static_cast<int>(spdlog::level::emerg)] = ANDROID_LOG_FATAL;
    }

    android_LogPriority convert_to_android(spdlog::level::level_enum level)
    {
        return priorities_[level];
    }

private:
    android_LogPriority priorities_[9];
};

/*
* Android sink (logging using __android_log_write)
*/
template<class Mutex, class Converter>
class base_android_sink : public base_sink < Mutex >
{
public:
    explicit base_android_sink(std::string tag="spdlog"): _tag(tag)
    {
    }

    void flush() override
    {
    }

protected:
    void _sink_it(const details::log_msg& msg) override
    {
        if (msg.level >= spdlog::level::off) {
          return;
        }
        if (msg.level < spdlog::level::trace) {
          return;
        }
        const android_LogPriority priority = converter_.convert_to_android(msg.level);
        const int expected_size = msg.formatted.size();
        const int size = __android_log_write(
            priority, _tag.c_str(), msg.formatted.c_str()
        );
        if (size > expected_size)
        {
            // Will write a little bit more than original message
        }
        else
        {
            throw spdlog_ex("Send to Android logcat failed");
        }
    }

private:
    Converter converter_;

    std::string _tag;
};

typedef base_android_sink<details::null_mutex, switch_converter> android_switch_sink;
typedef base_android_sink<details::null_mutex, array_converter> android_array_sink;

}
}

#endif
