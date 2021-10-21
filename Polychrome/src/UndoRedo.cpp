#include "UndoRedo.h"


namespace Polychrome
{
	std::list<ICommand*> UndoRedo::UndoStack;
	std::list<ICommand*> UndoRedo::RedoStack;
	bool UndoRedo::CalledBegin = false;

	bool UndoRedo::Undo()
	{
		if (UndoStack.empty())
			return false;

		ICommand* command = UndoStack.back();
		command->Undo();
		RedoStack.push_back(command);
		UndoStack.pop_back();

		return true;
	}


	bool UndoRedo::Redo()
	{
		if (RedoStack.empty())
			return false;

		ICommand* command = RedoStack.back();
		command->Redo();
		UndoStack.push_back(command);
		RedoStack.pop_back();

		return true;
	}
	bool UndoRedo::CanUndo()
	{
		return !CalledBegin && !UndoStack.empty();
	}
	bool UndoRedo::CanRedo()
	{
		return !CalledBegin && !RedoStack.empty();
	}
}