#include "imodule.h"

#include "ieventmanager.h"
#include "iuimanager.h"
#include "ientityinspector.h"
#include "icommandsystem.h"
#include "itextstream.h"

#include "AIHeadPropertyEditor.h"

class EditingModule : 
	public RegisterableModule
{
public:
	// RegisterableModule implementation
	virtual const std::string& getName() const {
		static std::string _name("DarkMod Editing");
		return _name;
	}
	
	virtual const StringSet& getDependencies() const {
		static StringSet _dependencies;

		if (_dependencies.empty())
		{
			_dependencies.insert(MODULE_ENTITYINSPECTOR);
			_dependencies.insert(MODULE_EVENTMANAGER);
			_dependencies.insert(MODULE_UIMANAGER);
			_dependencies.insert(MODULE_COMMANDSYSTEM);
		}

		return _dependencies;
	}
	
	virtual void initialiseModule(const ApplicationContext& ctx)
	{
		globalOutputStream() << getName() << "::initialiseModule called." << std::endl;

		// Associated "def_head" with an empty property editor instance
		GlobalEntityInspector().registerPropertyEditor(
			ui::DEF_HEAD_KEY, ui::IPropertyEditorPtr(new ui::AIHeadPropertyEditor())
		);
	}
};
typedef boost::shared_ptr<EditingModule> EditingModulePtr;

extern "C" void DARKRADIANT_DLLEXPORT RegisterModule(IModuleRegistry& registry)
{
	registry.registerModule(EditingModulePtr(new EditingModule));
	
	// Initialise the streams using the given application context
	module::initialiseStreams(registry.getApplicationContext());
	
	// Remember the reference to the ModuleRegistry
	module::RegistryReference::Instance().setRegistry(registry);
}