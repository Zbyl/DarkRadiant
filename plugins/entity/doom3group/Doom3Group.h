#ifndef DOOM3GROUP_H_
#define DOOM3GROUP_H_

#include "Bounded.h"
#include "editable.h"
#include "entitylib.h"
#include "traverselib.h"
#include "pivot.h"

#include "Doom3GroupOrigin.h"
#include "../keyobservers.h"
#include "../model.h"
#include "../origin.h"
#include "../rotation.h"
#include "../namedentity.h"
#include "../namekeys.h"
#include "../curve.h"
#include "../modelskinkey.h"

namespace entity {

class Doom3Group :
	public Bounded,
	public Snappable 
{
	EntityKeyValues m_entity;
	KeyObserverMap m_keyObservers;
	TraversableNodeSet m_traverse;
	MatrixTransform m_transform;

	SingletonModel m_model;
	OriginKey m_originKey;
	Vector3 m_origin;
	RotationKey m_rotationKey;
	Float9 m_rotation;

	NamedEntity m_named;
	NameKeys m_nameKeys;
	TraversableObserverPairRelay m_traverseObservers;
	Doom3GroupOrigin m_funcStaticOrigin;
	RenderablePivot m_renderOrigin;
	RenderableNamedEntity m_renderName;
	ModelSkinKey m_skin;

	mutable AABB m_curveBounds;

	Callback m_transformChanged;
	Callback m_evaluateTransform;

	// The value of the "name" key for this Doom3Group.
	std::string m_name;

	// The value of the "model" key for this Doom3Group.
	std::string m_modelKey;

	// Flag to indicate this Doom3Group is a model (i.e. does not contain
	// brushes).
	bool m_isModel;

	scene::Traversable* m_traversable;

public:
	NURBSCurve m_curveNURBS;
	SignalHandlerId m_curveNURBSChanged;
	CatmullRomSpline m_curveCatmullRom;
	SignalHandlerId m_curveCatmullRomChanged;
	InstanceCounter m_instanceCounter;

	/** greebo: The constructor takes the Entity class and the Node as argument
	 * as well as some callbacks for transformation and bounds changes.
	 * 
	 * These callbacks point to and InstanceSet::transformChangedCaller(), for example.  
	 */
	Doom3Group(IEntityClassPtr eclass, scene::Node& node, 
			   const Callback& transformChanged, 
			   const Callback& boundsChanged, 
			   const Callback& evaluateTransform);
	
	// Copy constructor
	Doom3Group(const Doom3Group& other, scene::Node& node, 
			   const Callback& transformChanged, 
			   const Callback& boundsChanged, 
			   const Callback& evaluateTransform);
	
	~Doom3Group();

	void instanceAttach(const scene::Path& path);
	void instanceDetach(const scene::Path& path);

	EntityKeyValues& getEntity();
	const EntityKeyValues& getEntity() const;

	scene::Traversable& getTraversable();
	Namespaced& getNamespaced();
	Nameable& getNameable();
	TransformNode& getTransformNode();
	ModelSkin& getModelSkin();

	// This gets called by the Doom3GroupNode constructor and attaches the
	// Node class as observer of this Doom3Group
	void attach(scene::Traversable::Observer* observer);
	void detach(scene::Traversable::Observer* observer);

	const AABB& localAABB() const;

	void renderSolid(Renderer& renderer, const VolumeTest& volume, const Matrix4& localToWorld, bool selected) const;
	void renderWireframe(Renderer& renderer, const VolumeTest& volume, const Matrix4& localToWorld, bool selected) const;

	void testSelect(Selector& selector, SelectionTest& test, SelectionIntersection& best);

	void translate(const Vector3& translation);
	void rotate(const Quaternion& rotation);
	void snapto(float snap);
	
	void revertTransform();
	void freezeTransform();
	void transformChanged();
	typedef MemberCaller<Doom3Group, &Doom3Group::transformChanged> TransformChangedCaller;
	
private:
	void construct();
	void destroy();

	void attachModel();
	void detachModel();
	
	void attachTraverse();
	void detachTraverse();

	bool isModel() const;

	void setIsModel(bool newValue);

	/** Determine if this Doom3Group is a model (func_static) or a
	 * brush-containing entity. If the "model" key is equal to the
	 * "name" key, then this is a brush-based entity, otherwise it is
	 * a model entity. The exception to this is for the "worldspawn"
	 * entity class, which is always a brush-based entity.
	 */
	void updateIsModel();

	void nameChanged(const char* value);
	typedef MemberCaller1<Doom3Group, const char*, &Doom3Group::nameChanged> NameChangedCaller;

	void modelChanged(const char* value);
	typedef MemberCaller1<Doom3Group, const char*, &Doom3Group::modelChanged> ModelChangedCaller;

	void updateTransform();
	typedef MemberCaller<Doom3Group, &Doom3Group::updateTransform> UpdateTransformCaller;

	void originChanged();
	typedef MemberCaller<Doom3Group, &Doom3Group::originChanged> OriginChangedCaller;

	void rotationChanged();
	typedef MemberCaller<Doom3Group, &Doom3Group::rotationChanged> RotationChangedCaller;

	void skinChanged();
	typedef MemberCaller<Doom3Group, &Doom3Group::skinChanged> SkinChangedCaller;
};

} // namespace entity

#endif /*DOOM3GROUP_H_*/
