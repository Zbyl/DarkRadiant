#ifndef _ENTITY_NODE_H_
#define _ENTITY_NODE_H_

#include "ientity.h"
#include "inamespace.h"

#include "selectionlib.h"
#include "transformlib.h"

#include "NamespaceManager.h"
#include "target/TargetableNode.h"
#include "NameKey.h"

namespace entity {

/**
 * greebo: This is the common base class of all map entities.
 */
class EntityNode :
	public IEntityNode,
	public SelectableNode, // derives from scene::Node
	public Namespaced,
	public TargetableNode,
	public Renderable,
	public Nameable,
	public Transformable
{
protected:
	// The entity class
	IEntityClassConstPtr _eclass;

	// The actual entity (which contains the key/value pairs)
	// TODO: Rename this to "spawnargs"?
	Doom3Entity _entity;

	// The class taking care of all the namespace-relevant stuff
	NamespaceManager _namespaceManager;

	// A helper class observing the "name" keyvalue
	// Used for rendering the name and as Nameable implementation
	NameKey _nameKey;

	// The OpenGLRenderable, using the NameKey helper class to retrieve the name
	RenderableNameKey _renderableName;

public:
	// The Constructor needs the eclass
	EntityNode(const IEntityClassConstPtr& eclass);

	// Copy constructor
	EntityNode(const EntityNode& other);

	virtual ~EntityNode();

	// Namespaced implementation
	// Gets/sets the namespace of this named object
	std::string getName() const;
	void setNamespace(INamespace* space);
	INamespace* getNamespace() const;
	void connectNameObservers();
	void disconnectNameObservers();
	void changeName(const std::string& newName);

	void attachNames();
	void detachNames();

	// Nameable implementation
	virtual std::string name() const;

	// Renderable implementation, can be overridden by subclasses
	virtual void renderSolid(RenderableCollector& collector, const VolumeTest& volume) const;
	virtual void renderWireframe(RenderableCollector& collector, const VolumeTest& volume) const;

protected:
	// Gets called by the Transformable implementation whenever
	// scale, rotation or translation is changed.
	virtual void _onTransformationChanged();

	// Called by the Transformable implementation before freezing
	// or when reverting transformations.
	virtual void _applyTransformation();

private:
	// Routines used by constructor and destructor, should be non-virtual
	void construct();
	void destruct();
};

} // namespace entity

#endif /* _ENTITY_NODE_H_ */
