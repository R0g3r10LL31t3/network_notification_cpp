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
				 * Mutable and imutable smart shared pointers to objects
				 * Mutable objects can be modifiyed
				 * Imutable objects can not be modifiyed, but const methods can be invoked
				 */
				using listener_handler_notifier_i = std::shared_ptr<const listener_handler_notifier>;
				using listener_handler_notifier_m = std::shared_ptr<listener_handler_notifier>;

				using listener_handler_i = std::shared_ptr<const listener_handler>;
				using listener_handler_m = std::shared_ptr<listener_handler>;

				using listener_i = std::shared_ptr<const listener>;
				using listener_m = std::shared_ptr<listener>;

				using subject_i = std::shared_ptr<const subject>;
				using subject_m = std::shared_ptr<subject>;

				/**
				 * Constant Mutable and imutable pointers to objects
				 * Mutable objects can be modifiyed
				 * Imutable objects can not be modifiyed, but const methods can be invoked
				 */
				using listener_handler_iptr = listener_handler* const;
				using listener_handler_mptr = listener_handler*;

				using listener_iptr = listener* const;
				using listener_mptr = listener*;

				using subject_iptr = subject* const;				
				using subject_mptr = subject*;

				/**
				 * Data Structure
				 */
				using listener_handlers_table_i = std::map<std::string, t::listener_handler_i>;
				using listener_handlers_table_m = std::map<std::string, const t::listener_handler_m>;
				
				using listener_table_i = std::map<std::string, t::listener_i>;
				using listener_table_m = std::map<std::string, const t::listener_m>;

				using listener_list_i = std::list<t::listener_i>;
				using listener_list_m = std::list<const t::listener_m>;
			} //t

			class listener_handler_notifier {
			protected:
				t::listener_handler_mptr _listener_handler { nullptr };
				t::listener_mptr _listener { nullptr };
				t::subject_mptr _subject { nullptr };
			public:
				listener_handler_notifier() = default;
				
				~listener_handler_notifier() = default;

				virtual const std::string& get_listener_handler_name() const = 0;

				void set_listener_handler(t::listener_handler_mptr listener_handler);

				t::listener_handler_iptr get_listener_handler() const;
				
				void set_listener(t::listener_mptr listener);

				t::listener_iptr get_listener() const;

				void set_subject(t::subject_mptr subject);

				t::subject_iptr get_subject() const;

				virtual bool do_notify() = 0;
			};

			class listener_handler {
			public:
				listener_handler() = default;

				~listener_handler() = default;

				virtual const std::string& get_listener_handler_name() const = 0;

				template <typename lambda_function>
				bool notify(const lambda_function& function) const {
					return function(this);
				}
				
				bool notify(const t::listener_handler_notifier_m& listener_method_notifier);
			};

			class listener {
			private:
				std::string _named_id;
				t::listener_handlers_table_m _listener_handlers_table;
			public:
				explicit listener(const std::string& named_id);

				~listener() = default;

				const std::string& get_named_id() const;

				const t::listener_handlers_table_m& get_listener_handlers_table() const;

				void add_listener_handler(const t::listener_handler_m& listener_handler);

				void rem_listener_handler(const t::listener_handler_m& listener_handler);

				template <typename lambda_function>
				bool notify(const std::string& listener_handler_name, const lambda_function& function) const {
					bool notified_ = false;

					auto listener_handler_it_ = this->_listener_handlers_table.find(listener_handler_name);
					if (listener_handler_it_ != this->_listener_handlers_table.end()) {
						const t::listener_handler_m& listener_handler_ = (*listener_handler_it_).second;
						auto newFunction_ = [this, &function](const listener_handler* listener_handler) {
							return function(this, listener_handler);
						};
						notified_ = listener_handler_->notify(newFunction_);
					}

					return notified_;
				}

				bool notify(const t::listener_handler_notifier_m& listener_handler_notifier);
			};

			class subject {
			private:
				std::string _address;
				std::string _port;
				t::listener_table_m _listeners;
				t::listener_list_i _listeners_faileds;
			protected:
				void clear_listeners_faileds();
				void add_listener_failed(const t::listener_m& listener_failed);
			public:
				explicit subject(const std::string& address, const std::string& port);

				~subject() = default;

				const std::string& get_address() const;

				const std::string& get_port() const;

				const t::listener_list_i& get_listeners_faileds() const;
				
				virtual void subscriber(const t::listener_m& listener);

				virtual void unsubscriber(const t::listener_m& listener);
				
				const t::listener_table_m& get_listeners() const;

				template <typename lambda_function>
				void notify_all(const std::string& listener_handler_name, const lambda_function& function) {
					
					this->clear_listeners_faileds();

					for (const auto& listener_pair_ : this->_listeners) {
						const t::listener_m& listener_ = listener_pair_.second;
						auto newFunction_ = [this, &function](const listener* listener, const listener_handler* listener_handler) {
							return function(this, listener, listener_handler);
						};
						if (!listener_->notify(listener_handler_name, newFunction_)) {
							add_listener_failed(listener_);
						}
					}
				}

				void notify_all(const t::listener_handler_notifier_m& listener_handler_notifier);
			};

		} //network
	} //system
} //_unnamed
