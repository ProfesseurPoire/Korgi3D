#pragma once

#include <vector>
#include <optional>
#include <memory>
#include <functional>
#include <queue>
#include <iterator>

template<class T>
using Vector = std::vector<T>;

template<class T>
using Optional = std::optional<T>;

template<class T>
using UniquePtr = std::unique_ptr<T>;

template<class T>
using Reference = std::reference_wrapper<T>;

namespace corgi
{

template<class T>
class Tree;

template<class T>
class Node;

template<class ... Args>
class Event
{
public:
	
	template<class U>
	void operator+=(U callback){callbacks_.emplace_back(callback);}
	void operator()(Args&& ... args)
	{
		std::for_each(callbacks_.begin(), callbacks_.end(), [&](auto& cb)
		{
			cb(std::forward<Args>(args)...);
		});
	}
	std::vector<std::function<void(Args ...)>> callbacks_;
};

// TODO :   I couldn't put this inside Iterator because compiler complained
//          that it couldn't find the type when I tried to initialized
//          it inside the Tree class so 🤦‍♀️ 🤷‍♂️
enum class IteratorMode :  char
{
	DepthFirst,
	BreadthFirst
};

template<class T>
class TreeIterator
{
public:

	TreeIterator()=default;

	explicit TreeIterator(Tree<T>& tree, IteratorMode mode = IteratorMode::DepthFirst):
		mode_(mode)
	{
		for(auto& n : tree.children().data())
		{
		 	queue_.push_back(*n);
		}
		operator++();
	}

	explicit TreeIterator(Node<T>& node, IteratorMode mode = IteratorMode::DepthFirst):
		mode_(mode)
	{
		for(auto& n : node.children().data())
		{
		 	queue_.push_back(*n);
		}
		operator++();
	}

	[[nodiscard]] Node<T>& operator*() noexcept{return *current_node_;}

	void increment_breadth_first() noexcept
	{
		current_node_ = &queue_.front().get();
		queue_.pop_front();
		auto& children = current_node_->children();
		std::copy(children.begin(), children.end(), std::back_inserter(queue_));
	}

	void increment_depth_first()  noexcept
	{
		current_node_ = &queue_.front().get();
		queue_.pop_front();
		auto& children= current_node_->children();
		std::copy(children.rbegin(), children.rend(), std::front_inserter(queue_));
	}

	TreeIterator& operator++() noexcept
	{
		if(queue_.empty())
		{
			current_node_=nullptr;
			return *this;
		}
		switch(mode_)
		{
			case IteratorMode::BreadthFirst:
				increment_breadth_first();
			break;

			case IteratorMode::DepthFirst:
				increment_depth_first();
			break;
		}
		return *this;
	}

	[[nodiscard]] bool operator!=(const TreeIterator& iterator)const noexcept
	{
		return current_node_ != iterator.current_node_;
	}

private:

	std::deque<Reference<Node<T>>> queue_;
	IteratorMode mode_      {IteratorMode::DepthFirst};
	bool recursive_         {true};
	Node<T>* current_node_  {nullptr};
};

template<class T>
class Children;

// Mainly exist as for the emplace_back function that wraps the annoying
// make_unique<Node> stuff
template<class T>
class Children
{
public:

	friend class Node<T>;

// Lifecycle

	Children(Tree<T>& tree) :
		tree_(tree){}

	Children(Node<T>& parent) : 
		parent_(&parent),
		tree_(parent.tree_) {}

// Functions

// So apparently I kinda have to make a ConstIterator class because
// otherwise the thing complains when I try to copy a const Node<T>&
class ConstIterator
{
public:

	using iterator_category = std::output_iterator_tag;
	using value_type        = Node<T>&;
	using difference_type   = int;
	using pointer           = Node<T>*;
	using reference         = Node<T>&;

	ConstIterator(const Children& children, int num, bool is_reversed):
		children_(&children),
		num_(num),
		is_reversed_(is_reversed)
	{}

	ConstIterator& operator++()
	{
		is_reversed_ ? --num_ : ++num_;
		return *this;
	}

	const Node<T>& operator*() const { return (*children_)[num_]; }

	bool operator==(const ConstIterator& iterator)const noexcept { return num_ == iterator.num_; }
	bool operator!=(const ConstIterator& iterator)const noexcept { return num_ != iterator.num_; }

private:
		
	const Children<T>* children_;
	bool is_reversed_ = false;
	int num_ = { -1 };
};
	
class Iterator
{
public:

	using iterator_category = std::output_iterator_tag;
	using value_type        = Node<T>;
	using difference_type   = int;
	using pointer           = Node<T>*;
	using reference         = Node<T>&;

	Iterator(Children& children, int num, bool is_reversed):
		children_(&children),
		num_(num),
		is_reversed_(is_reversed)
	{}

	Iterator& operator++()noexcept
	{
		is_reversed_ ? --num_ : ++num_;
		return *this;
	}

	[[nodiscard]] Node<T>& operator*()              { return (*children_)[num_]; }
	[[nodiscard]] const Node<T>& operator*() const  { return (*children_)[num_]; }

	[[nodiscard]] bool operator==(const Iterator& iterator) const noexcept{ return num_ == iterator.num_; }
	[[nodiscard]] bool operator!=(const Iterator& iterator) const noexcept{ return num_ != iterator.num_; }

private:

	Children<T>* children_  {nullptr};
	bool is_reversed_       {false};
	int num_                { -1 };
};

	auto& data() { return children_; }

	/*!
	 * @brief   Constructs and adds a new Node into the children's vector
	 */
	template<class ... Args>
	Node<T>& emplace_back(Args&&... args)
	{
		if(parent_)
		{
			return *children_.emplace_back(std::make_unique<Node<T>>(parent_, std::forward<Args>(args)...));
		}
		return *children_.emplace_back(std::make_unique<Node<T>>(tree_, std::forward<Args>(args)...));
	}

	bool push_back(Node<T>& node) 
	{
		if(node.is_parent(*parent_))
		{
			return false;
		}

		std::vector<UniquePtr<Node<T>>>* children{nullptr};
		(node.parent())? children = &node.parent()->children().children_ : children = &tree_.children().children_;

		auto it = std::find_if(children->begin(), children->end(), [&](const auto& a){return a.get()==&node;});
		if(*it)
		{
			children_.push_back(std::make_unique<Node<T>>( *it->get()));
			children_.front()->parent_ = parent_;
			return true;
		}
		return false;
	}

	/*!
	 * @brief   Moves a node as the child of the current node
	 * 
	 *          If the node you try to move is a parent of the current node, the
	 *          operation fails and return false
	 */
	bool push_back(Node<T>&& node) 
	{
		if(node.is_parent(*parent_)) // Illegal, return error message
		{
			return false;
		}

		// Would love to have this be only 1 line
		std::vector<UniquePtr<Node<T>>>* children{nullptr};
		(node.parent())? children = &node.parent()->children().children_ : children = &tree_.children().children_;
		
		auto it = std::find_if(children->begin(), children->end(), [&](const auto& a){return a.get()==&node;});
		if(it->operator bool())
		{
			auto index = it - children->begin() ;
			children_.push_back(std::move((*it)));
			children_.back()->parent_ = parent_; 
			children->erase(children->begin()+index);
			return true;
		}
		return false;
	}

	void clear()noexcept { children_.clear(); }

	[[nodiscard]] Node<T>& operator[](int index) { return *children_[index]; }
	[[nodiscard]] const Node<T>& operator[](int index)const { return *children_[index]; }

	[[nodiscard]] Iterator begin() noexcept { return Iterator(*this, 0, false); }
	[[nodiscard]] ConstIterator begin() const noexcept { return ConstIterator(*this, 0, false); }

	[[nodiscard]] auto end() noexcept { return Iterator(*this, children_.size(), false); }
	[[nodiscard]] ConstIterator end() const noexcept{return ConstIterator (*this, children_.size(), false);}

	[[nodiscard]] auto rbegin() noexcept { return Iterator(*this, children_.size() - 1, true); }
	[[nodiscard]] auto rbegin() const noexcept { return Iterator(*this, children_.size() - 1, true); }

	[[nodiscard]] auto rend() noexcept { return Iterator(*this, -1, true); }
	[[nodiscard]] auto rend() const noexcept { return Iterator(*this, -1, true); }

	//  Capacity Functions

	[[nodiscard]] constexpr bool empty()const noexcept { return children_.empty(); }
	[[nodiscard]] size_t size()const noexcept { return children_.size(); }
	[[nodiscard]] auto max_size()const noexcept { return  children_.max_size(); }
	[[nodiscard]] auto capacity()const noexcept { return  children_.capacity(); }
	void shrink_to_fit() noexcept { children_.capacity(); }

private:

	Tree<T>&   tree_;
	Vector<UniquePtr<Node<T>>> children_;
	// Can be optional because this is used by the tree too
	Node<T>* parent_{nullptr};
};

template<class T>
class Node
{
public:

friend class Children<T>;

// Lifecycle

	template<class ... Args>
	explicit Node(Tree<T>& tree, Args&& ... args):
		value_(std::forward<Args>(args)...),
		tree_(tree)
	{
		children_ = { std::make_unique<Children<T>>(*this) };
		tree_.on_node_created_callback_(*this);
	}

	template<class ... Args>
	explicit Node(Node<T>* parent, Args&& ... args):
		value_(std::forward<Args>(args)...),
		parent_(parent),
		tree_(parent->tree_)
	{
		children_ = { std::make_unique<Children<T>>(*this) };
		tree_.on_node_created_callback_(*this);
	}

	Node(const Node<T>& node):
		tree_(node.tree_)
	{
		value_		= node.value_;
		children_	= { std::make_unique<Children<T>>(*this) };

		for(const auto& child : node.children())
		{
			children_->children_.emplace_back(std::make_unique<Node>(child));
		}
	}

	~Node()
	{
		tree_.on_node_removed_callback_(*this);
	}

	[[nodiscard]] bool is_parent(const Node& node) noexcept
	{
		if(!node.parent())
		{
			return false;
		}
		else
		{
			if(node.parent() == this)
			{
				return true;
			}
			else
			{
				return is_parent(*node.parent());
			}
		}
	}

	[[nodiscard]] auto begin(){return TreeIterator<T>(*this, mode_);}
	[[nodiscard]] auto begin()const noexcept{return TreeIterator<T>(*this, mode_);}

	[[nodiscard]] auto end()noexcept{return TreeIterator<T>();}
	[[nodiscard]] auto end()const noexcept{return TreeIterator<T>();}

	[[nodiscard]] bool is_leaf()const noexcept{return children_->empty();}

	[[nodiscard]] Node<T>* parent() noexcept{ return parent_;}
	[[nodiscard]] const Node<T>* parent()const noexcept{return parent_;}

	[[nodiscard]] T& get()noexcept{return value_;}
	[[nodiscard]] const T& get()const noexcept{return value_;}

	[[nodiscard]] Children<T>& children()noexcept{return *children_.get();}
	[[nodiscard]] const Children<T>& children()const noexcept{return *children_.get();}

	[[nodiscard]] T* operator->()noexcept{return &value_;}
	[[nodiscard]] const T* operator->()const noexcept{return &value_;}

	void iterator_mode(IteratorMode mode)noexcept{mode_=mode;}

	/*!
	 * @brief 	Detach the current node from its parent
	 * 
	 * 			Basically reattach this node the tree root
	 * 			Return false if you can't detach the node, true otherwise
	 */
	bool detach()noexcept
	{
		if(!parent_)
		{
			return false;
		}
		auto& c =  parent()->children().children_ ;

		// TODO : 	maybe I could also store the index of the node into it's
		// 			container so I don't have to std::find_if all the time
		auto it = std::find_if(c.begin(), c.end(), [&](const auto& a){return a.get()==this;});

		if(it->operator bool())
		{
			auto index = it - c.begin() ;
			tree_.children().children_.push_back(std::move((*it)));
			tree_.children().children_.back()->parent_=nullptr;
			c.erase(c.begin()+index);
			return true;
		}
		return true;
	}

private:

	UniquePtr<Children<T>>  children_;
	T                       value_;
	Node<T>*                parent_                 {nullptr};

	// Kinda need a reference to the tree the node is a part of
	Tree<T>&                tree_;

	// This part is used to iterate the children nodes
	IteratorMode            mode_                   {IteratorMode::DepthFirst};
	bool                    recursive_iterator_     {true};
	
};

// Note : I can't make the Tree class constexpr because of std::vector
template<class T>
class Tree
{
	friend class Node<T>;
	friend class Children<T>;

public:

	[[nodiscard]] Children<T>& children()noexcept;
	[[nodiscard]] const Children<T>& children()const noexcept;

	// So I know it might be a bit confusing, but you can actually
	// iterate over the tree with depth/breadth first search enabled
	[[nodiscard]] TreeIterator<T> begin() noexcept;
	[[nodiscard]] TreeIterator<T> end() noexcept;
	
	void iterator_mode(IteratorMode mode)noexcept;

	[[nodiscard]] auto& on_node_created_event()noexcept{return on_node_created_callback_;}
	[[nodiscard]] auto& on_node_removed_event()noexcept{return on_node_removed_callback_;}
	
private:

	Event<Node<T>&> on_node_created_callback_;
	Event<Node<T>&> on_node_removed_callback_;

	IteratorMode iterator_mode_{IteratorMode::DepthFirst};
	bool recursive_iterator_{ true };
	// Only using a unique_ptr instead of object here because otherwise 
	// the compiler complains because he doesn't know everything related to
	// Children yet
	UniquePtr<Children<T>> children_{std::make_unique<Children<T>>(*this)};
};

// Implementation

template<class T>   
Children<T>& Tree<T>::children()noexcept { return *children_.get(); }

template<class T>
const Children<T>& Tree<T>::children()const noexcept { return *children_.get(); }

template<class T>
TreeIterator<T> Tree<T>::begin() noexcept { return TreeIterator<T>(*this, iterator_mode_); }

template<class T>
TreeIterator<T> Tree<T>::end() noexcept { return TreeIterator<T>(); }

template<class T>
void Tree<T>::iterator_mode(IteratorMode mode) noexcept { iterator_mode_ = mode; }
}