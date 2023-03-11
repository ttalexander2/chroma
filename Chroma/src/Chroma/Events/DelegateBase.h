#pragma once

namespace Chroma
{
	template <typename T>
	class DelegateBase;

	template <typename Ret, typename ...Args>
	class DelegateBase<Ret(Args...)> 
	{
	protected:
		using StubType = Ret(*)(void* this_ptr, Args...);

		struct InvocationElement {
			InvocationElement() = default;
			InvocationElement(void* this_ptr, StubType aStub) : object(this_ptr), stub(aStub) {}

			void clone(InvocationElement& target) const
			{
				target.stub = stub;
				target.object = object;
			}

			bool operator==(const InvocationElement& other)
			{
				return other.stub == stub && other.object == object;
			}

			bool operator !=(const InvocationElement& other)
			{
				return other.stub != stub || other.object != object;
			}

			void* object = nullptr;
			StubType stub = nullptr;
		};
	};
}
