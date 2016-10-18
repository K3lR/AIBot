#pragma once

#include <vector>

class BehaviorTree
{
public:
	// Represents each node of the tree
	class ITask
	{
	public:
		virtual bool run() = 0;
	};

	/************************** COMPOSITES ***************************/
	// Composite patter, contains list of other Tasks such as 
	class Composite : public ITask
	{
		std::vector<ITask*> mChildren;

	public:
		std::vector<ITask*> getChildren() { return mChildren; }
		void addTask(const ITask* task) { mChildren.emplace_back(task); }
	};

	// Runs each child till one fails
	class Sequence : public Composite
	{
	protected:
		virtual ~Sequence() {}
	public:
		bool run() override
		{
			for (ITask* child : getChildren())
			{
				if (!child->run())
				{
					return false;
				}
			}
			return true;
		}
	};

	// Runs each child while none has succeeded
	class Selector : public Composite
	{
	protected:
		virtual ~Selector() {}
	public:
		bool run() override
		{
			for (ITask* child : getChildren())
			{
				if (child->run())
				{
					return true;
				}
			}
			return false;
		}
	};

	/************************** DECORATORS ***************************/
	// Modifies child's result
	class Decorator : public ITask
	{
		ITask* mChild;

	public:
		ITask* getChild() { return mChild; }
		virtual bool run() { mChild->run(); }
	};

	class UntilFail : public Decorator
	{
		//***TODO 
	};

	/************************ B-TREE ROOT **************************/

	class Root : public ITask
	{
		ITask* mChild;
		friend class BehaviorTree;
		void setRootChild(ITask* rootChild) { mChild = rootChild; }
		virtual bool run() override { return mChild->run(); }

	};

private:
	Root* mRoot;

public:
	BehaviorTree() : mRoot{ new Root{} }
	{}
	
	void setRootChild(ITask* rootChild) { mRoot->setRootChild(rootChild); }
	bool run() { mRoot->run(); }
};


class Action : public BehaviorTree::ITask
{
	//***TODO
};