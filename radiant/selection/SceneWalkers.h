#pragma once

#include "ientity.h"
#include "ieclass.h"
#include "itransformnode.h"
#include "itextstream.h"
#include "igroupnode.h"
#include "scenelib.h"
#include "iselectiontest.h"
#include "editable.h"
#include "brush/BrushNode.h"

// -------------- Helper functions -------------------------------------

inline AABB Node_getPivotBounds(const scene::INodePtr& node) {
	Entity* entity = Node_getEntity(node);
	if (entity != NULL && (entity->getEntityClass()->isFixedSize() || !node_is_group(node)))
	{
		EditablePtr editable = Node_getEditable(node);
		if (editable != NULL) {
			return AABB(node->localToWorld().getMultipliedBy(editable->getLocalPivot()).t().getVector3(), Vector3(0, 0, 0));
		}
		else {
			return node->worldAABB();
		}
	}

	return node->worldAABB();
}

// ----------- The Walker Classes ------------------------------------------------

// Sets the visited instance to <select> (true or false), this is used to select all instances in the graph
class SelectAllWalker :
	public scene::NodeVisitor
{
	bool _select;

public:
	SelectAllWalker(bool select) :
		_select(select)
	{}

	bool pre(const scene::INodePtr& node)
	{
		Node_setSelected(node, _select);
		return true;
	}
};

// Selects the visited component instances in the graph, according to the current component mode
class SelectAllComponentWalker :
	public scene::NodeVisitor
{
	bool _select;
	SelectionSystem::EComponentMode _mode;

public:
	SelectAllComponentWalker(bool select, SelectionSystem::EComponentMode mode) :
		_select(select),
		_mode(mode)
	{}

	bool pre(const scene::INodePtr& node)
	{
		ComponentSelectionTestablePtr componentSelectionTestable = Node_getComponentSelectionTestable(node);

		if (componentSelectionTestable != NULL)
		{
			componentSelectionTestable->setSelectedComponents(_select, _mode);
		}

		return true;
	}
};

// Traverses through the scenegraph and removes degenerated brushes from the selected.
// greebo: The actual erasure is performed in the destructor to keep the scenegraph intact during traversal.
class RemoveDegenerateBrushWalker :
	public SelectionSystem::Visitor
{
	mutable std::list<scene::INodePtr> _eraseList;
public:
	// Destructor removes marked paths
	~RemoveDegenerateBrushWalker() {
		for (std::list<scene::INodePtr>::iterator i = _eraseList.begin(); i != _eraseList.end(); ++i) {
			// Check if the parent has any children left at all
			scene::INodePtr parent = (*i)->getParent();

			// Remove the node from the scene
			scene::removeNodeFromParent(*i);

			if (parent != NULL && !parent->hasChildNodes()) {
				rError() << "Warning: removing empty parent entity." << std::endl;
				scene::removeNodeFromParent(parent);
			}
		}
	}

	void visit(const scene::INodePtr& node) const
	{
		Brush* brush = Node_getBrush(node);

		if (brush != NULL && !brush->hasContributingFaces())
		{
			// greebo: Mark this path for removal
			_eraseList.push_back(node);

			rError() << "Warning: removed degenerate brush!\n";
			return;
		}
	}
};

// As the name states, all visited instances have their transformations freezed
class FreezeTransforms :
	public scene::NodeVisitor
{
public:
	bool pre(const scene::INodePtr& node)
	{
		ITransformablePtr transform = Node_getTransformable(node);
		if (transform != 0)
		{
			transform->freezeTransform();
		}

		return true;
	}
};

// As the name states, all visited instances have their transformations reverted
class RevertTransforms :
	public scene::NodeVisitor
{
public:
	bool pre(const scene::INodePtr& node)
	{
		ITransformablePtr transform = Node_getTransformable(node);
		if (transform != 0)
		{
			transform->revertTransform();
		}

		return true;
	}
};

// As the name states, all visited SELECTED instances have their transformations reverted
// TODO: Remove this class, and use GlobalSelectionSystem().foreach instead
class RevertTransformForSelected :
	public scene::NodeVisitor
{
public:
	bool pre(const scene::INodePtr& node)
	{
		if (Node_isSelected(node))
		{
			ITransformablePtr transform = Node_getTransformable(node);
			if (transform != NULL)
			{
				transform->revertTransform();
			}
		}

		return true;
	}
};

/**
 * greebo: Calculates the axis-aligned bounding box of the current selection.
 * Use this walker to traverse the current selection and use the getBounds()
 * method to retrieve the calculated bounds.
 */
class BoundsAccumulator :
	public SelectionSystem::Visitor
{
	mutable AABB _bounds;
public:
	const AABB& getBounds() const {
		return _bounds;
	}

	void visit(const scene::INodePtr& node) const {
		_bounds.includeAABB(Node_getPivotBounds(node));
	}
};

// greebo: Calculates the axis-aligned bounding box of the selection components.
// The constructor is called with a reference to an AABB variable that is updated during the walk
class ComponentBoundsAccumulator :
	public SelectionSystem::Visitor
{
	mutable AABB _bounds;
public:
	ComponentBoundsAccumulator()
	{
		_bounds = AABB();
	}

	virtual void visit(const scene::INodePtr& node) const
	{
		ComponentEditablePtr componentEditable = Node_getComponentEditable(node);

		if (componentEditable != NULL)
		{
			_bounds.includeAABB(
				AABB::createFromOrientedAABBSafe(componentEditable->getSelectedComponentsBounds(),
											node->localToWorld()));
		}
	}

	const AABB& getBounds() const
	{
		return _bounds;
	}
};

/**
 * greebo: Traverses the selection and invokes the functor on
 * each encountered primitive.
 *
 * The SelectionWalker traverses the currently selected instances and
 * passes Brushes and Patches right to the PrimitiveVisitor. When
 * GroupNodes are encountered, the GroupNode itself is traversed
 * and all child primitives are passed to the PrimitiveVisitor as well.
 */
class SelectionWalker :
	public scene::NodeVisitor
{
public:
	void visit(const scene::INodePtr& node)
	{
		// Check if we have an entity
		scene::GroupNodePtr groupNode = Node_getGroupNode(node);

		if (groupNode != NULL)
		{
			// We have a selected groupnode, traverse it using self as walker
			node->traverse(*this);
			return;
		}

		if (node->visible())
		{
			handleNode(node);
		}
	}

	// NodeVisitor implemenatation
	bool pre(const scene::INodePtr& node)
	{
		visit(node);
		return true; // traverse further
	}

protected:
	// Type-specific handler method
	virtual void handleNode(const scene::INodePtr& node) = 0;
};

// Walker specialisation for Brushes
class BrushSelectionWalker :
	public SelectionWalker
{
	typedef std::function<void(Brush&)> VisitFunc;
	VisitFunc _functor;
public:
	BrushSelectionWalker(const VisitFunc& functor) :
		_functor(functor)
	{}

protected:
	void handleNode(const scene::INodePtr& node)
	{
		Brush* brush = Node_getBrush(node);

		if (brush != NULL)
		{
			_functor(*brush);
		}
	}
};

// Walker specialisation for Patches
class PatchSelectionWalker :
	public SelectionWalker
{
	typedef std::function<void(Patch&)> VisitFunc;
	VisitFunc _functor;
public:
	PatchSelectionWalker(const VisitFunc& functor) :
		_functor(functor)
	{}

protected:
	void handleNode(const scene::INodePtr& node)
	{
		Patch* patch = Node_getPatch(node);

		if (patch != NULL)
		{
			_functor(*patch);
		}
	}
};

// Walker specialisation for Faces
class FaceSelectionWalker :
	public SelectionWalker
{
	typedef std::function<void(Face&)> VisitFunc;
	VisitFunc _functor;
public:
	FaceSelectionWalker(const VisitFunc& functor) :
		_functor(functor)
	{}

protected:
	void handleNode(const scene::INodePtr& node)
	{
		Brush* brush = Node_getBrush(node);

		if (brush != NULL)
		{
			brush->forEachFace([&] (Face& face)
			{
				if (face.faceIsVisible())
				{
					_functor(face);
				}
			});
		}
	}
};
