﻿#pragma once

#include "EventBase.h"
#include "Delegate.h"
#include <vector>
#include <functional>

namespace Chroma
{
	
	template <typename Owner, typename Ret, typename... Args>
	class Event<Owner, Ret(Args...)> : private EventBase<Ret, Args...> {
	    friend Owner;
	public:
	    Event() = default;
	    ~Event() {
	        clear();
	    }

	    bool empty() const { return this->invocation_list.size() < 1; }

	    bool operator==(void* ptr) const {
	        return (ptr == nullptr) && this->empty();
	    }

	    bool operator !=(void* ptr) const {
	        return (ptr != nullptr) || (!this->empty());
	    }

	    size_t size() const { return this->invocation_list.size(); }

	    Event& operator=(const Event&) = delete;
	    Event(const Event&) = delete;

	    bool operator==(const Event& other) const {
	        if (this->invocation_list.size() != other.invocation_list.size()) {
	            return false;
	        }
	        auto otherIt = other.invocation_list.begin();
	        for (auto it = this->invocation_list.begin(); it != this->invocation_list.end(); it++) {
	            if (**it != **otherIt) {
	                return false;
	            }
	        }
	        return true;
	    }

	    bool operator !=(const Event& other) const {
	        return !(*this==other);
	    }

	    bool operator ==(const Delegate<Ret(Args...)>& other) const {
	        if (empty() && other.empty()) {
	            return true;
	        }
	        if (other.empty() || (size() != 1)) {
	            return false;
	        }
	        return (other.invocation == **this->invocation_list.begin());
	    }

	    bool operator != (const Delegate<Ret(Args...)>& other) const {
	        return !(*this == other);
	    }

	    Event& operator +=(const Event& other) {
	        for (auto& item : other.invocation_list) {
	            this->invocation_list.push_back(new typename DelegateBase<Ret(Args...)>::InvocationElement(item->object, item->stub));
	        }
	        return *this;
	    }

	    template <typename Lambda>
	    Event& operator +=(const Lambda& lambda) {
	        Delegate<Ret(Args...)> d = Delegate<Ret(Args...)>::template create<Lambda>(lambda);
	        return *this += d;
	    }

	    Event& operator +=(const Delegate<Ret(Args...)>& other) {
	        if (other.is_null()) {
	            return *this;
	        }
	        this->invocation_list.push_back(new typename DelegateBase<Ret(Args...)>::InvocationElement(other.invocation.object, other.invocation.stub));
	        return *this;
	    }

	    template <auto Function, typename T>
	    void add(T* instance) {
	        Delegate<Ret(Args...)> d;
	        d.template bind<T, Function>(instance);
	        return *this += d;
	    }

	    template <auto Function, typename T>
	    void add(T const* instance) {
	        Delegate<Ret(Args...)> d;
	        d.template bind<T, Function>(instance);
	        return *this += d;
	    }

	    template <auto Function>
	    void add() {
	        Delegate<Ret(Args...)> d;
	        d.template bind<Function>();
	        return *this += d;
	    }

	    template <typename Lambda>
	    void add(const Lambda& instance) {
	        Delegate<Ret(Args...)> d;
	        d.template bind<Lambda>(instance);
	        return *this += d;
	    }


	private:

	    void clear() {
	        for (auto& element : this->invocation_list)
	        {
	            delete element;
	        }
	        this->invocation_list.clear(); 
	    }

	    void operator()(Args... args) const {
	        for (auto& item : this->invocation_list) {
	            (*(item->stub))(item->object, args...);
	        }
	    }

	    template<typename Handler>
	    void operator()(Args... args, Handler handler) const {
	        size_t index = 0;
	        for (auto& item : this->invocation_list) {
	            Ret value = (*(item->stub))(item->object, args...);
	            handler(index, &value);
	            index++;
	        }
	    }

	    void operator()(Args... args, Delegate<void(size_t, Ret*)> handler) const {
	        operator()<decltype(handler)>(args..., handler);
	    }

	    void operator()(Args... args, std::function<void(size_t, Ret*)> handler) const {
	        operator()<decltype(handler)>(args..., handler);
	    }

	    void broadcast(Args... args) const {
	        operator()(args...);
	    }

	    template<typename Handler>
	    void broadcast(Args... args, Handler handler) const {
	        operator()(args..., handler);
	    }

	    void broadcast(Args... args, Delegate<void(size_t, Ret*)> handler) const {
	        operator()<decltype(handler)>(args..., handler);
	    }

	    void broadcast(Args... args, std::function<void(size_t, Ret*)> handler) const {
	        operator()<decltype(handler)>(args..., handler);
	    }

	};
}
