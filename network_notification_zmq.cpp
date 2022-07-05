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

#include "network_notification.h"
#include "network_notification_zmq.h"

using namespace _unnamed::system::network;

zmq_publisher::zmq_publisher(const std::string& named_id, const std::string& address, const std::string& port) noexcept
    : listener(named_id), _address(address), _port(port) {
}

const std::string zmq_publisher::make_url(const std::string& address, const std::string& port) noexcept {
    return "tcp://" + address + ":" + port;
}

const std::string zmq_publisher::get_url() const noexcept {
    return make_url(this->_address, this->_port);
}

const std::string& zmq_publisher::get_address() const noexcept {
    return this->_address;
}

const std::string& zmq_publisher::get_port() const noexcept {
    return this->_port;
}

void zmq_publisher::bind(const t::context_m& context) {
    auto socket_ = std::make_unique<t::socket>(*context, zmq::socket_type::pub);

    socket_->set(zmq::sockopt::sndhwm, 1000);
    socket_->set(zmq::sockopt::tcp_keepalive, 1);
    socket_->set(zmq::sockopt::ipv6, 0);
    socket_->set(zmq::sockopt::linger, 0);

    socket_->bind(get_url());

    this->_socket = std::move(socket_);
}

void zmq_publisher::unbind(const std::string& address, const std::string& port) {
    this->unbind(make_url(address, port));
}

void zmq_publisher::unbind(const std::string& url) {
    if (this->_socket) {
        this->_socket->unbind(url);
    }
}

void zmq_publisher::close() noexcept {
    if (this->_socket) {
        this->_socket->close();
    }
}

bool zmq_publisher::connected() const noexcept {
    return this->_socket->connected();
}

void zmq_publisher::disconnect(std::string const& url) {
    this->_socket->disconnect(url);
}

void zmq_publisher::send(const t::const_buffer& topic, const t::const_buffer& data) {
    if (this->_socket) {
        this->_socket->send(topic, zmq::send_flags::sndmore);
        this->_socket->send(data, zmq::send_flags::none);
    }
}
