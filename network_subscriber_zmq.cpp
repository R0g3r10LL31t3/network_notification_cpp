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

#include "network_subscriber.h"
#include "network_subscriber_zmq.h"

using namespace _unnamed::system::network;

typedef struct curve_client_tag {
    static const std::string client_private_key;
    static const std::string client_public_key;
    static const std::string server_public_key;
} curve_client_t;

/**
 * TODO: Remove this when you have a configuration environment
 * Theses keys are available at <http ://api.zeromq.org/master:zmq-curve>
 * Warning: never put private keys and sensitive data in the final application code.
 */
//7BB864B489AFA3671FBE69101F94B38972F24816DFB01B51656B3FEC8DFD0888 or z85 bellow
const std::string curve_client_tag::client_private_key = "JTKVSB%%)wK0E.X)V>+}o?pNmC{O&4W4b!Ni{Lh6";
//BB88471D65E2659B30C55A5321CEBB5AAB2B70A398645C26DCA2B2FCB43FC518 or z85 bellow
const std::string curve_client_tag::client_public_key = "D:)Q[IlAW!ahhC2ac:9*A}h:p?([4%wOTJ%JR%cs";
//54FCBA24E93249969316FB617C872BB0C1D1FF14800427C594CBFACF1BC2D652 or z85 bellow
const std::string curve_client_tag::server_public_key = "rq:rM>}U?@Lns47E1%kR.o@n%FcmmsL/@{H8]yf7";


zmq_subscriber::address_and_port_tag::address_and_port_tag(const std::string& address, const std::string& port) noexcept {
    this->address = address;
    this->port = port;
}

std::string zmq_subscriber::address_and_port_tag::to_string() const noexcept {
    return this->address + ":" + this->port;
}

zmq_subscriber_handler_receiver::zmq_subscriber_handler_receiver()
    : subscriber_handler_receiver() {
}

void zmq_subscriber_handler_receiver::set_subscriber(t::zmq_subscriber_mptr subscriber) {
    subscriber_handler_receiver::set_subscriber(reinterpret_cast<t::subscriber_mptr>(subscriber));
};

t::zmq_subscriber_mptr zmq_subscriber_handler_receiver::get_subscriber() const {
    return reinterpret_cast<t::zmq_subscriber_mptr>(subscriber_handler_receiver::get_subscriber());
}

zmq_subscriber::zmq_subscriber(const std::string& named_id, const t::context_m& context) noexcept
    : subscriber(named_id), _context(context) {
}

const std::string zmq_subscriber::make_url(const std::string& address, const std::string& port) noexcept {
    return "tcp://" + address + ":" + port;
}

const std::string zmq_subscriber::to_string() const noexcept {
    std::stringstream builder;
    for (auto _address_and_port : this->_addresses_and_ports) {
        builder << "[" << _address_and_port.address << ":" << _address_and_port.port << "]";
    }
    return builder.str();
}

void zmq_subscriber::connect(const std::string& address, const std::string& port) {
    auto socket_ = std::make_unique<t::socket>(*_context, zmq::socket_type::sub);

    socket_->set(zmq::sockopt::sndhwm, 1000);
    //socket_->set(zmq::sockopt::tcp_keepalive, 1);
    //socket_->set(zmq::sockopt::heartbeat_ivl, 1);
    socket_->set(zmq::sockopt::ipv6, 0);
    socket_->set(zmq::sockopt::linger, 0);

    socket_->set(zmq::sockopt::curve_serverkey, curve_client_t::server_public_key);
    socket_->set(zmq::sockopt::curve_publickey, curve_client_t::client_public_key);
    socket_->set(zmq::sockopt::curve_secretkey, curve_client_t::client_private_key);

    //socket_->set(zmq::sockopt::conflate, 1);
    //socket_->set(zmq::sockopt::subscribe, topic);

    socket_->connect(make_url(address, port));

    this->_addresses_and_ports.push_back(address_and_port_t{ address, port });
    this->close();
    this->_socket = std::move(socket_);
}

void zmq_subscriber::connect(const std::list<address_and_port_t>& addresses_and_ports) {
    auto socket_ = std::make_unique<t::socket>(*_context, zmq::socket_type::sub);

    socket_->set(zmq::sockopt::sndhwm, 1000);
    //socket_->set(zmq::sockopt::tcp_keepalive, 1);
    //socket_->set(zmq::sockopt::heartbeat_ivl, 1);
    socket_->set(zmq::sockopt::ipv6, 0);
    socket_->set(zmq::sockopt::linger, 0);

    socket_->set(zmq::sockopt::curve_serverkey, curve_client_t::server_public_key);
    socket_->set(zmq::sockopt::curve_publickey, curve_client_t::client_public_key);
    socket_->set(zmq::sockopt::curve_secretkey, curve_client_t::client_private_key);

    //socket_->set(zmq::sockopt::conflate, 1);
    //socket_->set(zmq::sockopt::subscribe, topic);

    for (auto address_and_port : addresses_and_ports) {
        socket_->connect(make_url(address_and_port.address, address_and_port.port));
    }

    this->_addresses_and_ports = addresses_and_ports;
    //auto destroy old socket, see zmq::socket_t and see zmq::socket_base_t
    this->close();
    this->_socket = std::move(socket_);
}
void zmq_subscriber::unbind(const std::string& address, const std::string& port) {
    if (this->_socket) {
        this->unbind(make_url(address, port));
    }
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
    return this->_socket? this->_socket->connected(): false;
}

void zmq_subscriber::disconnect(std::string const& url) {
    if (this->_socket) {
        this->_socket->disconnect(url);
    }
}

bool zmq_subscriber::receive(t::mutable_buffer& topic, t::mutable_buffer& data) {
    bool has_value_topic_ = false;
    bool has_value_data_ = false;

    if (this->_socket) {
        has_value_topic_ = this->_socket->recv(topic, zmq::recv_flags::dontwait).has_value();
        has_value_topic_ = this->_socket->recv(data, zmq::recv_flags::dontwait).has_value();
    }
    return has_value_topic_ && has_value_data_;
}

bool zmq_subscriber::receive(t::message& topic, t::message& data) {
    bool has_value_topic_ = false;
    bool has_value_data_ = false;
    if (this->_socket) {
        has_value_topic_ = this->_socket->recv(topic, zmq::recv_flags::dontwait).has_value();
        has_value_data_ = this->_socket->recv(data, zmq::recv_flags::dontwait).has_value();
    }
    return has_value_topic_ && has_value_data_;
}
