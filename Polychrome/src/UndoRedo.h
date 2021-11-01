#pragma once

#include <Chroma/Math/Vec2.h>
#include <Chroma/Core/Log.h>
#include <stack>
#include <unordered_map>
#include <imgui.h>
#include <functional>

namespace Polychrome
{

	static const unsigned int STACK_LIMIT = 1000;

	struct ICommand
	{
		virtual inline const void SetOld(void* old_value) = 0;
		virtual inline const void SetNew(void* new_value) = 0;
		virtual inline void* GetOld() = 0;
		virtual inline void* GetNew() = 0;

		virtual inline const bool Undo() = 0;
		virtual inline const bool Redo() = 0;

		virtual inline const std::string GetName() = 0;

		virtual inline void* GetPtr() = 0;
	};

	template <class T>
	struct Command : ICommand
	{
		Command(T* ptr)
			: Ptr(ptr) {}

		~Command() { if (UserData != nullptr) ::operator delete(UserData); }

		inline const void SetOld(void* old_value) override { Old = *(T*)old_value; } 
		inline const void SetNew(void* new_value) override { New = *(T*)new_value; }
		inline void* GetOld() override { return (void*)&Old; }
		inline void* GetNew() override { return (void*)&New; }

		inline const bool Undo() override 
		{ 
			if (Ptr == nullptr) return false;
			Setter(Ptr, Old, UserData);
			return true; 
		}

		inline const bool Redo() override 
		{ 
			if (Ptr == nullptr) return false;
			Setter(Ptr, New, UserData);
			return true;
		}

		inline const std::string GetName() override { return Name; }

		inline const void SetUserData(void* user_data) { UserData = user_data; }

		inline void* GetPtr() override { return (void*)Ptr; }


		std::string Name;
		std::function<void(T*, T, void*)> Setter = [&](T* ptr, T value, void* user_data) { *ptr = value; };

	private:
		T Old;
		T New;
		T* Ptr;
		void* UserData = nullptr;
		

	};

	class UndoRedo
	{
	public:

		template <class T>
		static bool BeginCommand(T* ptr, T val, const std::string& name, const std::function<void(T*, T, void*)>& setter_func, void* user_data = nullptr, bool copy_user_data = false, size_t size = 256)
		{
			if (ptr == nullptr)
				return false;
			if (CalledBegin)
			{
				CHROMA_CORE_ERROR("Called BeginCommand twice before EndCommand!");
				return false;
			}

			if (UndoStack.size() == STACK_LIMIT)
			{
				ICommand* cmd = UndoStack.front();
				UndoStack.pop_front();
				delete cmd;
			}

			auto command = new Command<T>(ptr);
			command->SetOld((void*)&val);
			command->Name = name;
			command->Setter = setter_func;
			if (user_data != nullptr && copy_user_data)
			{
				void* userCopy = ::operator new(size);
				std::memcpy(userCopy, user_data, size);
				command->SetUserData(userCopy);
			}
			else if (user_data != nullptr)
			{
				command->SetUserData(user_data);
			}
			
			UndoStack.push_back(command);
			CalledBegin = true;
		}

		template <class T>
		static bool BeginCommand(T* ptr, T val, const std::string& name)
		{
			if (ptr == nullptr)
				return false;
			if (CalledBegin)
			{
				CHROMA_CORE_ERROR("Called BeginCommand twice before EndCommand!");
				return false;
			}

			if (UndoStack.size() == STACK_LIMIT)
			{
				ICommand* cmd = UndoStack.front();
				UndoStack.pop_front();
				delete cmd;
			}

			auto command = new Command<T>(ptr);
			command->SetOld((void*)&val);
			command->Name = name;
			UndoStack.push_back(command);
			CalledBegin = true;
		}

		template <class T>
		static bool BeginCommand(T* ptr, const std::string& name)
		{
			return BeginCommand(ptr, *ptr, name);
		}

		template <class T>
		static bool EndCommand(T* ptr, T val)
		{
			if (!CalledBegin)
			{
				CHROMA_CORE_ERROR("Called EndCommand before BeginCommand!");
				return false;
			}

			if (UndoStack.size() <= 0)
				return false;

			if ((T*)UndoStack.back()->GetPtr() != ptr)
				return false;


			UndoStack.back()->SetNew((void*)&val);

			if (val == *(T*)UndoStack.back()->GetOld())
			{
				std::erase_if(UndoStack, [](ICommand* cmd) { return *(T*)cmd->GetOld() == *(T*)cmd->GetNew(); });
				CalledBegin = false;
				return false;
			}

			for (ICommand* redoCommand : RedoStack)
			{
				delete redoCommand;
			}

			RedoStack.clear();
			
			CalledBegin = false;
			return true;
		}

		template <class T>
		static bool EndCommand(T* ptr)
		{
			return EndCommand(ptr, *ptr);
		}

		static bool Undo();
		static bool Redo();

		static bool CanUndo();
		static bool CanRedo();

		template <class T>
		static bool ImGuiRegister(T* ptr, T previous_value, const std::string& name, const std::function<void(T*, T, void*)>& setter_func, void* user_data = nullptr, bool copy_user_data = false, size_t size = 256)
		{
			if (ImGui::IsItemActivated())
				return UndoRedo::BeginCommand<T>(ptr, previous_value, name, setter_func, user_data, copy_user_data, size);

			if (ImGui::IsItemDeactivated())
				return UndoRedo::EndCommand<T>(ptr, *ptr);
			return false;
		}

		template <class T>
		static bool ImGuiRegister(T* ptr, T previous_value, const std::string& name)
		{
			if (ImGui::IsItemActivated())
				return UndoRedo::BeginCommand<T>(ptr, previous_value, name);

			if (ImGui::IsItemDeactivated())
				return UndoRedo::EndCommand<T>(ptr, *ptr);
			return false;
		}

	//private:
		static std::list<ICommand*> UndoStack;
		static std::list<ICommand*> RedoStack;

	private:
		static bool CalledBegin;
	};

}

