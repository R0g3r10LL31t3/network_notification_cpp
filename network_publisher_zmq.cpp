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

#include "network_publisher.h"
#include "network_publisher_zmq.h"

using namespace _unnamed::system::network;

zmq_publisher_handler_sender::zmq_publisher_handler_sender()
    : publisher_handler_sender() {
}

void zmq_publisher_handler_sender::set_publisher(t::zmq_publisher_mptr publisher) {
    publisher_handler_sender::set_publisher(reinterpret_cast<t::publisher_mptr>(publisher));
};

t::zmq_publisher_mptr zmq_publisher_handler_sender::get_publisher() const {
    return reinterpret_cast<t::zmq_publisher_mptr>(publisher_handler_sender::get_publisher());
}

zmq_publisher::zmq_publisher(const std::string& named_id, const t::context_m& context) noexcept
    : publisher(named_id), _context(context) {
}

const std::string zmq_publisher::make_url(const std::string& address, const std::string& port) noexcept {
    return "tcp://" + address + ":" + port;
}

const std::string zmq_publisher::get_addresses_and_ports() const noexcept {
    std::stringstream builder;
    for (auto _address_and_port : this->_addresses_and_ports) {
        builder << "[" << _address_and_port.address << ":" << _address_and_port.port << "]";
    }
    return builder.str();
}

void zmq_publisher::bind(const std::string& address, const std::string& port) {
    auto socket_ = std::make_unique<t::socket>(*_context, zmq::socket_type::xpub);

    socket_->set(zmq::sockopt::sndhwm, 1000);
    //socket_->set(zmq::sockopt::tcp_keepalive, 1);
    //socket_->set(zmq::sockopt::heartbeat_ivl, 1);
    socket_->set(zmq::sockopt::ipv6, 0);
    socket_->set(zmq::sockopt::linger, 0);

    socket_->set(zmq::sockopt::curve_secretkey, "JTKVSB%%)wK0E.X)V>+}o?pNmC{O&4W4b!Ni{Lh6");//8E0BDD697628B91D8F245587EE95C5B04D48963F79259877B49CD9063AEAD3B7
    socket_->set(zmq::sockopt::curve_server, 1);

    socket_->bind(make_url(address, port));

    this->_addresses_and_ports.push_back(address_and_port_t{ address, port });
    this->_socket = std::move(socket_);
}

void zmq_publisher::bind(const std::list<address_and_port_t>& addresses_and_ports) {
    auto socket_ = std::make_unique<t::socket>(*_context, zmq::socket_type::xpub);

    socket_->set(zmq::sockopt::sndhwm, 1000);
    //socket_->set(zmq::sockopt::tcp_keepalive, 1);
    //socket_->set(zmq::sockopt::heartbeat_ivl, 1);
    socket_->set(zmq::sockopt::xpub_verbose, 1);
    socket_->set(zmq::sockopt::ipv6, 0);
    socket_->set(zmq::sockopt::linger, 0);

    socket_->set(zmq::sockopt::curve_secretkey, "JTKVSB%%)wK0E.X)V>+}o?pNmC{O&4W4b!Ni{Lh6");//8E0BDD697628B91D8F245587EE95C5B04D48963F79259877B49CD9063AEAD3B7
    socket_->set(zmq::sockopt::curve_server, 1);

    for (auto address_and_port : addresses_and_ports) {
        socket_->bind(make_url(address_and_port.address, address_and_port.port));
    }

    this->_addresses_and_ports = addresses_and_ports;
    //auto destroy old connections, see zmq::socket_t and see zmq::socket_base_t
    this->_socket = std::move(socket_);
}

void zmq_publisher::unbind(const std::string& address, const std::string& port) {
    if (this->_socket) {
        this->unbind(make_url(address, port));
    }
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
    return this->_socket? this->_socket->connected(): false;
}

void zmq_publisher::disconnect(std::string const& url) {
    if (this->_socket) {
        this->_socket->disconnect(url);
    }
}

bool zmq_publisher::send(t::const_buffer& topic, t::const_buffer& data) {
    bool has_value_topic_ = false;
    bool has_value_data_ = false;
    if (this->_socket) {
        has_value_topic_ = this->_socket->send(topic, zmq::send_flags::sndmore).has_value();
        has_value_data_ = this->_socket->send(data, zmq::send_flags::none).has_value();
    }
    return has_value_topic_ && has_value_data_;
}

bool zmq_publisher::send(t::message& topic, t::message& data) {
    bool has_value_topic_ = false;
    bool has_value_data_ = false;
    if (this->_socket) {
        has_value_topic_ = this->_socket->send(topic, zmq::send_flags::sndmore).has_value();
        has_value_data_ = this->_socket->send(data, zmq::send_flags::none).has_value();
    }
    return has_value_topic_ && has_value_data_;
}
