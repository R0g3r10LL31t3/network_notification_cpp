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

class _unnamed::system::network::observer_handler_notifier;
class _unnamed::system::network::observer_handler;
class _unnamed::system::network::observer;
class _unnamed::system::network::observer_controller;

class zmq_subscriber_handler_notifier;
class zmq_subscriber;

namespace t {

/**
 * Declaration to header isolation
 * #include <zmq.hpp> in *.cpp source files
 */
using socket = zmq::socket_t;
using context = zmq::context_t;
using mutable_buffer = zmq::mutable_buffer;

/**
 * Mutable and imutable smart shared pointers to objects
 * Mutable objects can be modifiyed
 * Imutable objects can not be modifiyed, but const methods can be invoked
 */
using socket_i = std::unique_ptr<const socket>;
using socket_m = std::unique_ptr<socket>;

using context_i = std::shared_ptr<const context>;
using context_m = std::shared_ptr<context>;

using zmq_subscriber_handler_notifier_i = std::shared_ptr<const zmq_subscriber_handler_notifier>;
using zmq_subscriber_handler_notifier_m = std::shared_ptr<const zmq_subscriber_handler_notifier>;

using zmq_subscriber_i = std::shared_ptr<const zmq_subscriber>;
using zmq_subscriber_m = std::shared_ptr<const zmq_subscriber>;

/**
 * Constant mutable and imutable pointers to objects
 * Mutable objects can be modifiyed
 * Imutable objects can not be modifiyed, but const methods can be invoked
 */
using zmq_subscriber_iptr = zmq_subscriber* const;
using zmq_subscriber_mptr = zmq_subscriber*;
} //t

namespace exception {

#define EXCEPTION_SOURCE \
					(std::string(" Throw from: ") + (__func__) + "() at `" + (__FILE__) + "`:`" + std::to_string(__LINE__) + "`.")

#define EXCEPTION(msg) \
					std::exception(std::string(std::string(msg) + EXCEPTION_SOURCE).c_str())

/**
 * use
 * throw zmq_subscriber_exception("reason exception.");
 */
#define zmq_subscriber_exception(reason) \
					EXCEPTION("zmq_subscriber_exception: " reason)
}

class zmq_subscriber_handler_notifier :
    public observer_handler_notifier {
private:
    void set_observer(t::observer_mptr observer) = delete;

    t::observer_mptr get_observer() const = delete;
public:
    zmq_subscriber_handler_notifier();

    ~zmq_subscriber_handler_notifier() = default;

    void set_subscriber(t::zmq_subscriber_mptr subscriber);

    t::zmq_subscriber_mptr get_subscriber() const;
};

class zmq_subscriber: public observer {
private:
    using observer::observer;

    std::string _address;
    std::string _port;
    t::socket_m _socket;

    const std::string get_url() const noexcept;

    static const std::string make_url(const std::string& address, const std::string& port) noexcept;
public:
    explicit zmq_subscriber(const std::string& named_id, const std::string& address, const std::string& port) noexcept;

    ~zmq_subscriber() = default;

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

    void receive(t::mutable_buffer& topic, t::mutable_buffer& data);
};
} //network
} //system
} //_unnamed
