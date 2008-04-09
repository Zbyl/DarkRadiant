#ifndef KNOCKOUTCOMPONENTEDITOR_H_
#define KNOCKOUTCOMPONENTEDITOR_H_

#include "ComponentEditor.h"
#include "ComponentEditorFactory.h"
#include "SpecifierEditCombo.h"
#include "../ComponentType.h"

namespace objectives
{

namespace ce
{

/**
 * ComponentEditor subclass for KO component type.
 * 
 * A KO component requires that an AI must be knocked out. It can use all
 * specifiers except SPEC_GROUP.
 */
class KnockoutComponentEditor 
: public ComponentEditor
{
	// Registration class
	static struct RegHelper 
	{
		RegHelper() {
			ComponentEditorFactory::registerType(
				objectives::ComponentType::COMP_KO().getName(), 
				ComponentEditorPtr(new KnockoutComponentEditor())
			);
		}
	} regHelper;
	
	// Main widget
	GtkWidget* _widget;
	
	// Component to edit
	Component* _component;	
	
	// SpecifierEditCombo for the knockout target
	SpecifierEditCombo _targetCombo;
	
public:

	/**
	 * Construct a default KnockoutComponentEditor.
	 */
	KnockoutComponentEditor()
	: _component(NULL)
	{ }
	
	/**
	 * Construct a KnockoutComponentEditor with a Component object to display and
	 * edit.
	 * 
	 * @param component
	 * The Component to edit.
	 */
	KnockoutComponentEditor(Component& component);
	
	/**
	 * Destructor.
	 */
	~KnockoutComponentEditor();
	
	/* ComponentEditor implementation */
	
	ComponentEditorPtr clone(Component& component) const {
		return ComponentEditorPtr(new KnockoutComponentEditor(component));
	}
	
	GtkWidget* getWidget() const; 

};

}

}

#endif /*KNOCKOUTCOMPONENTEDITOR_H_*/
