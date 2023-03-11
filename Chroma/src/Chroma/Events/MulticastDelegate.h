#pragma once
#include "Delegate.h"
#include <vector>
#include <functional>

namespace Chroma
{

	template <typename Ret, typename... Args>
	class MulticastDelegate<Ret(Args...)> : private DelegateBase<Ret(Args...)> {
	public:
	    MulticastDelegate() = default;
	    ~MulticastDelegate() {
	        clear();
	    }

	    void clear() {
	        for (auto& element : invocation_list)
	        {
	            delete element;
	        }
	        invocation_list.clear(); 
	    }

	    bool empty() const { return invocation_list.size() < 1; }

	    bool operator==(void* ptr) const {
	        return (ptr == nullptr) && this->empty();
	    }

	    bool operator !=(void* ptr) const {
	        return (ptr != nullptr) || (!this->empty());
	    }

	    size_t size() const { return invocation_list.size(); }

	    MulticastDelegate& operator=(const MulticastDelegate&) = delete;
	    MulticastDelegate(const MulticastDelegate&) = delete;

	    bool operator==(const MulticastDelegate& other) const {
	        if (invocation_list.size() != other.invocation_list.size()) {
	            return false;
	        }
	        auto otherIt = other.invocation_list.begin();
	        for (auto it = invocation_list.begin(); it != invocation_list.end(); it++) {
	            if (**it != **otherIt) {
	                return false;
	            }
	        }
	        return true;
	    }

	    bool operator !=(const MulticastDelegate& other) const {
	        return !(*this==other);
	    }

	    bool operator ==(const Delegate<Ret(Args...)>& other) const {
	        if (empty() && other.empty()) {
	            return true;
	        }
	        if (other.empty() || (size() != 1)) {
	            return false;
	        }
	        return (other.invocation == **invocation_list.begin());
	    }

	    bool operator != (const Delegate<Ret(Args...)>& other) const {
	        return !(*this == other);
	    }

	    MulticastDelegate& operator +=(const MulticastDelegate& other) {
	        for (auto& item : other.invocation_list) {
	            this->invocation_list.push_back(new typename DelegateBase<Ret(Args...)>::InvocationElement(item->object, item->stub));
	        }
	        return *this;
	    }

	    template <typename Lambda>
	    MulticastDelegate& operator +=(const Lambda& lambda) {
	        Delegate<Ret(Args...)> d = Delegate<Ret(Args...)>::template create<Lambda>(lambda);
	        return *this += d;
	    }

	    MulticastDelegate& operator +=(const Delegate<Ret(Args...)>& other) {
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

	    void operator()(Args... args) const {
	        for (auto& item : invocation_list) {
	            (*(item->stub))(item->object, args...);
	        }
	    }

	    template<typename Handler>
	    void operator()(Args... args, Handler handler) const {
	        size_t index = 0;
	        for (auto& item : invocation_list) {
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

	private:
	    std::vector<typename DelegateBase<Ret(Args...)>::InvocationElement*> invocation_list;

	};	
}
