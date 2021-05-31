/*
 *
 * Copyright (c) 2015 Microsoft Corporation. All rights reserved.
 *
 * This code is licensed under the MIT License (MIT).
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#pragma once

#include <exception>
#include <stdexcept>
#include <string>

#define GSL_THROW_ON_CONTRACT_VIOLATION

//
// There are three configuration options for this GSL implementation's behavior
// when pre/post conditions on the GSL types are violated:
//
// 1. GSL_TERMINATE_ON_CONTRACT_VIOLATION: std::terminate will be called (default)
// 2. GSL_THROW_ON_CONTRACT_VIOLATION: a gsl::fail_fast exception will be thrown
// 3. GSL_UNENFORCED_ON_CONTRACT_VIOLATION: nothing happens
//
#if !(defined(GSL_THROW_ON_CONTRACT_VIOLATION) ^ defined(GSL_TERMINATE_ON_CONTRACT_VIOLATION) ^    \
      defined(GSL_UNENFORCED_ON_CONTRACT_VIOLATION))
#define GSL_TERMINATE_ON_CONTRACT_VIOLATION
#endif

#define GSL_STRINGIFY_DETAIL(x) #x
#define GSL_STRINGIFY(x) GSL_STRINGIFY_DETAIL(x)

#if defined(__clang__) || defined(__GNUC__)
#define GSL_LIKELY(x) __builtin_expect (!!(x), 1)
#define GSL_UNLIKELY(x) __builtin_expect (!!(x), 0)
#else
#define GSL_LIKELY(x) (x)
#define GSL_UNLIKELY(x) (x)
#endif

/*int __builtin_LINE();
const char* __builtin_FILE();*/

namespace od
{
inline namespace gsl
{
struct fail_fast:
    public std::runtime_error
{
    explicit fail_fast(
                char const* const message,
                const char* pretty_func,
                const int l = __builtin_LINE(),
				const char* f = __builtin_FILE()) : std::runtime_error(message) {} // HACK
    std::string stack_trace;
};
}
}

#if defined(GSL_THROW_ON_CONTRACT_VIOLATION)

// Expects with one or two parameters (to specify the error)
#define MacroImpl_Expects_1(cond)                    \
    if (GSL_UNLIKELY(!(cond)))                       \
        throw od::gsl::fail_fast(                    \
            "GSL: Precondition failure at " __FILE__ \
            ": " GSL_STRINGIFY(__LINE__), __PRETTY_FUNCTION__);

#define MacroImpl_Expects_2(cond, str)                      \
    if (GSL_UNLIKELY(!(cond))) {                            \
        throw od::gsl::fail_fast( (std::string(             \
            "GSL: Precondition failure at " __FILE__        \
            ": " GSL_STRINGIFY(__LINE__) ": ") + str).c_str(), __PRETTY_FUNCTION__); \
    }

#define MacroImpl_Expects_arg(arg1, arg2, arg3, ...) arg3
#define MacroImpl_Expects_choose(...) \
    MacroImpl_Expects_arg(__VA_ARGS__, MacroImpl_Expects_2, MacroImpl_Expects_1, )
#define Expects(...) MacroImpl_Expects_choose(__VA_ARGS__)(__VA_ARGS__)


// Ensures with one or two parameters (to specify the error)
#define MacroImpl_Ensures_1(cond)                     \
    if (GSL_UNLIKELY(!(cond)))                        \
        throw od::gsl::fail_fast(                     \
            "GSL: Postcondition failure at " __FILE__ \
            ": " GSL_STRINGIFY(__LINE__), __PRETTY_FUNCTION__);

#define MacroImpl_Ensures_2(cond, str)                       \
    if (GSL_UNLIKELY(!(cond))) {                            \
        throw od::gsl::fail_fast( (std::string(             \
            "GSL: Postcondition failure at " __FILE__       \
            ": " GSL_STRINGIFY(__LINE__) ": ") + str).c_str(), __PRETTY_FUNCTION__); \
    }

#define MacroImpl_Ensures_arg(arg1, arg2, arg3, ...) arg3
#define MacroImpl_Ensures_choose(...) \
    MacroImpl_Ensures_arg(__VA_ARGS__, MacroImpl_Ensures_2, MacroImpl_Ensures_1, )
#define Ensures(...) MacroImpl_Ensures_choose(__VA_ARGS__)(__VA_ARGS__)


// Assert with one or two parameters (to specify the error)
#define MacroImpl_Assert_1(cond)                  \
    if (GSL_UNLIKELY(!(cond))) {                  \
        throw od::gsl::fail_fast(                 \
            "GSL: Assertion failure at " __FILE__ \
            ": " GSL_STRINGIFY(__LINE__), __PRETTY_FUNCTION__);        \
    }

#define MacroImpl_Assert_2(cond, str)                       \
    if (GSL_UNLIKELY(!(cond))) {                            \
        throw od::gsl::fail_fast( (std::string(             \
            "GSL: Assertion failure at " __FILE__           \
            ": " GSL_STRINGIFY(__LINE__) ": ") + str).c_str(), __PRETTY_FUNCTION__); \
    }

#define MacroImpl_Assert_arg(arg1, arg2, arg3, ...) arg3
#define MacroImpl_Assert_choose(...) \
    MacroImpl_Assert_arg(__VA_ARGS__, MacroImpl_Assert_2, MacroImpl_Assert_1, )
#define Assert(...) MacroImpl_Ensures_choose(__VA_ARGS__)(__VA_ARGS__)

#elif defined(GSL_TERMINATE_ON_CONTRACT_VIOLATION)

#define Expects(cond)                                                                              \
    if (GSL_UNLIKELY(!(cond))) std::terminate();
#define Ensures(cond)                                                                              \
    if (GSL_UNLIKELY(!(cond))) std::terminate();
#define Assert(cond)                                                                              \
    if (GSL_UNLIKELY(!(cond))) std::terminate();

#elif defined(GSL_UNENFORCED_ON_CONTRACT_VIOLATION)

#define ExpectsPretty(cond)
#define Expects(cond)
#define Ensures(cond)
#define Assert(cond)

#endif

