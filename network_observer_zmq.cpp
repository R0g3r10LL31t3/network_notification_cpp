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

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif /* #if defined(WIN32) || defined(WIN64) */

#include <string>
#include <iostream>

#include <zmq.hpp>

#include "network_observer.h"
#include "network_observer_zmq.h"

using namespace _unnamed::system::network;

zmq_subscriber_handler_notifier::zmq_subscriber_handler_notifier()
    : observer_handler_notifier() {
}

void zmq_subscriber_handler_notifier::set_subscriber(t::zmq_subscriber_mptr subscriber) {
    observer_handler_notifier::set_observer(reinterpret_cast<t::observer_mptr>(subscriber));
};

t::zmq_subscriber_mptr zmq_subscriber_handler_notifier::get_subscriber() const {
    return reinterpret_cast<t::zmq_subscriber_mptr>(observer_handler_notifier::get_observer());
}

zmq_subscriber::zmq_subscriber(const std::string& named_id, const std::string& address, const std::string& port) noexcept
    : observer(named_id), _address(address), _port(port) {
}

const std::string zmq_subscriber::make_url(const std::string& address, const std::string& port) noexcept {
    return "tcp://" + address + ":" + port;
}

const std::string zmq_subscriber::get_url() const noexcept {
    return make_url(this->_address, this->_port);
}

const std::string& zmq_subscriber::get_address() const noexcept {
    return this->_address;
}

const std::string& zmq_subscriber::get_port() const noexcept {
    return this->_port;
}

void zmq_subscriber::bind(const t::context_m& context) {
    auto socket_ = std::make_unique<t::socket>(*context, zmq::socket_type::sub);

    socket_->set(zmq::sockopt::sndhwm, 1000);
    socket_->set(zmq::sockopt::tcp_keepalive, 1);
    socket_->set(zmq::sockopt::ipv6, 0);
    socket_->set(zmq::sockopt::linger, 0);

    //socket_->set(zmq::sockopt::subscribe, topic);

    socket_->connect(get_url());

    this->_socket = std::move(socket_);
}

void zmq_subscriber::unbind(const std::string& address, const std::string& port) {
    this->unbind(make_url(address, port));
}

void zmq_subscriber::unbind(const std::string& url) {
    if (this->_socket) {
        this->_socket->unbind(url);
    }
}

void zmq_subscriber::close() noexcept {
    if (this->_socket) {
        this->_socket->close();
    }
}

bool zmq_subscriber::connected() const noexcept {
    return this->_socket->connected();
}

void zmq_subscriber::disconnect(std::string const& url) {
    this->_socket->disconnect(url);
}

void zmq_subscriber::receive(t::mutable_buffer& topic, t::mutable_buffer& data) {
    if (this->_socket) {
        this->_socket->recv(topic, zmq::recv_flags::dontwait);
        this->_socket->recv(data, zmq::recv_flags::dontwait);
    }
}
