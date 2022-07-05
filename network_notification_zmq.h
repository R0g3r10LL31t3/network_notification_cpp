/**
* MIT License
*
* Copyright(c) 2022 Rogério Lecarião Leite, Brazil.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this softwareand associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright noticeand this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#pragma once

#include <string>
#include <list>
#include <map>
#include <memory>

namespace _unnamed {

namespace system {

namespace network {

class listener_handler_notifier;
class listener_handler;
class listener;
class subject;

namespace t {

/**
 * Declaration to header isolation
 * #include <zmq.hpp> in *.cpp source files
 */
using socket = zmq::socket_t;
using context = zmq::context_t;
using const_buffer = zmq::const_buffer;

using socket_i = std::unique_ptr<const socket>;
using socket_m = std::unique_ptr<socket>;

using context_i = std::shared_ptr<const context>;
using context_m = std::shared_ptr<context>;
} //t

namespace exception {

#define EXCEPTION_SOURCE \
					(std::string(" Throw from: ") + (__func__) + "() at `" + (__FILE__) + "`:`" + std::to_string(__LINE__) + "`.")

#define EXCEPTION(msg) \
					std::exception(std::string(std::string(msg) + EXCEPTION_SOURCE).c_str())

/**
 * use
 * throw zmq_publisher_exception("reason exception.");
 */
#define zmq_publisher_exception(reason) \
					EXCEPTION("zmq_publisher_exception: " reason)
}

class zmq_publisher : public listener {
private:
    using listener::listener;

    std::string _address;
    std::string _port;
    t::socket_m _socket;

    const std::string get_url() const noexcept;

    static const std::string make_url(const std::string& address, const std::string& port) noexcept;
public:
    explicit zmq_publisher(const std::string& named_id, const std::string& address, const std::string& port) noexcept;

    ~zmq_publisher() = default;

    const std::string& get_address() const noexcept;

    const std::string& get_port() const noexcept;

    /**
     * @throws zmq::error_t();
     */
    void bind(const t::context_m& context);

    /**
     * @throws zmq::error_t();
     */
    void unbind(const std::string& address, const std::string& port);

    /**
     * @throws zmq::error_t();
     */
    void unbind(const std::string& url);

    void close() noexcept;

    bool connected() const noexcept;

    void disconnect(std::string const& url);

    void send(const t::const_buffer& topic, const t::const_buffer& data);
};
} //network
} //system
} //_unnamed
