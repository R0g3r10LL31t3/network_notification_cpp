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


#include "network_publisher.h"

using namespace _unnamed::system::network;

const std::string& publisher_handler_sender::get_publisher_handler_topic_id() const {
    return get_publisher_handler()->get_topic_id();
}

void publisher_handler_sender::set_publisher_handler(t::publisher_handler_mptr publisher_handler) {
    this->_publisher_handler = publisher_handler;
}

t::publisher_handler_iptr publisher_handler_sender::get_publisher_handler() const {
    return this->_publisher_handler;
}

void publisher_handler_sender::set_publisher(t::publisher_mptr publisher) {
    this->_publisher = publisher;
}

t::publisher_iptr publisher_handler_sender::get_publisher() const {
    return this->_publisher;
}

void publisher_handler_sender::set_publisher_controller(t::publisher_controller_mptr publisher_controller) {
    this->_publisher_controller = publisher_controller;
}

t::publisher_controller_iptr publisher_handler_sender::get_publisher_controller() const {
    return this->_publisher_controller;
}

publisher_handler::publisher_handler(const std::string& topic_id) {
    this->_topic_id = topic_id;
}

const std::string& publisher_handler::get_topic_id() const {
    return this->_topic_id;
}

bool publisher_handler::notify(const t::publisher_handler_sender_m& publisher_handler_sender) {
    return publisher_handler_sender->do_notify();
}

publisher::publisher(const std::string& named_id)
    : _named_id(named_id) {
}

const std::string& publisher::get_named_id() const {
    return this->_named_id;
}

const t::publisher_handlers_table_m& publisher::get_publisher_handlers_table() const {
    return this->_publisher_handlers_table;
}

void publisher::add_publisher_handler(const t::publisher_handler_m& publisher_handler) {
    this->_publisher_handlers_table.insert({ publisher_handler->get_topic_id(), publisher_handler });
}

void publisher::rem_publisher_handler(const t::publisher_handler_m& publisher_handler) {
    this->_publisher_handlers_table.erase(publisher_handler->get_topic_id());
}

bool publisher::notify(const t::publisher_handler_sender_m& publisher_handler_sender) {
    bool notified_ = false;

    auto publisher_handler_it_ = this->_publisher_handlers_table.find(publisher_handler_sender->get_publisher_handler_topic_id());
    if (publisher_handler_it_ != this->_publisher_handlers_table.end()) {
        const t::publisher_handler_m publisher_handler_ = (*publisher_handler_it_).second;
        publisher_handler_sender->set_publisher(this);
        notified_ = publisher_handler_->notify(publisher_handler_sender);
        publisher_handler_sender->set_publisher(nullptr);
    }

    return notified_;
}

publisher_controller::publisher_controller(const std::string& named_id)
    : _named_id(named_id) {
}

const std::string& publisher_controller::get_named_id() const {
    return this->_named_id;
}

void publisher_controller::add_publisher(const t::publisher_m& publisher) {
    this->_publishers.insert({ publisher->get_named_id(), publisher });
}

void publisher_controller::rem_publisher(const t::publisher_m& publisher) {
    this->_publishers.erase(publisher->get_named_id());
}

const t::publisher_table_m& publisher_controller::get_publishers() const {
    return this->_publishers;
}

void publisher_controller::clear_publishers_faileds() {
    this->_publishers_faileds.clear();
}

void publisher_controller::add_publisher_failed(const t::publisher_m& publisher_failed) {
    this->_publishers_faileds.push_back(publisher_failed);
}

const t::publisher_list_i& publisher_controller::get_publishers_faileds() const {
    return this->_publishers_faileds;
}

void publisher_controller::notify_all(const t::publisher_handler_sender_m& publisher_handler_sender) {

    this->clear_publishers_faileds();

    for (const auto& publisher_address_pair_ : this->_publishers) {
        const t::publisher_m& publisher_ = publisher_address_pair_.second;
        publisher_handler_sender->set_publisher_controller(this);
        if (!publisher_->notify(publisher_handler_sender)) {
            this->add_publisher_failed(publisher_);
        }
        publisher_handler_sender->set_publisher_controller(nullptr);
    }
}
