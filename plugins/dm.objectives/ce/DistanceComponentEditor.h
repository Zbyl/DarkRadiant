#ifndef DISTANCE_COMPONENT_EDITOR_H_
#define DISTANCE_COMPONENT_EDITOR_H_

#include "ComponentEditor.h"
#include "ComponentEditorFactory.h"
#include "SpecifierEditCombo.h"
#include "../ComponentType.h"

namespace objectives {

namespace ce {

/**
 * ComponentEditor subclass for COMP_DISTANCE component type.
 * 
 * An COMP_DISTANCE component usually uses a two specifiers.
 */
class DistanceComponentEditor : 
	public ComponentEditor
{
	// Registration class
	static struct RegHelper 
	{
		RegHelper() {
			ComponentEditorFactory::registerType(
				objectives::ComponentType::COMP_DISTANCE().getName(), 
				ComponentEditorPtr(new DistanceComponentEditor())
			);
		}
	} regHelper;
	
	// Main widget
	GtkWidget* _widget;
	
	// Component to edit
	Component* _component;	
	
	// SpecifierEditCombo for the entity in question
	SpecifierEditCombo _entSpec;

	// SpecifierEditCombo for the location the entity should be in (or not)
	SpecifierEditCombo _locationSpec;
	
public:

	/**
	 * Construct a default DistanceComponentEditor.
	 */
	DistanceComponentEditor() : 
		_component(NULL)
	{}
	
	/**
	 * Construct a DistanceComponentEditor with a Component object to display and
	 * edit.
	 * 
	 * @param component
	 * The Component to edit.
	 */
	DistanceComponentEditor(Component& component);
	
	/**
	 * Destructor
	 */
	~DistanceComponentEditor();
	
	/* ComponentEditor implementation */
	
	ComponentEditorPtr clone(Component& component) const {
		return ComponentEditorPtr(new DistanceComponentEditor(component));
	}
	
	GtkWidget* getWidget() const; 

    void writeToComponent() const;
};

} // namespace ce

} // namespace objectives

#endif /* DISTANCE_COMPONENT_EDITOR_H_ */