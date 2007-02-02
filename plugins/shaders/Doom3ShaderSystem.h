#ifndef DOOM3SHADERSYSTEM_H_
#define DOOM3SHADERSYSTEM_H_

#include "ishaders.h"
#include "moduleobserver.h"
#include "ShaderLibrary.h"

class Doom3ShaderSystem : 
	public ShaderSystem, 
	public ModuleObserver
{
	// The shaderlibrary stores all the known shaderdefinitions 
	// as well as the active shaders
	ShaderLibrary* _library;
	
public:
	// Constructor, allocates the library
	Doom3ShaderSystem();
	
	// Destructor, removes the library from heap
	~Doom3ShaderSystem();

	void realise();
	void unrealise();
	void refresh();
	
	// Is the shader system realised
	bool isRealised();

	// Return a shader by name
	IShader* getShaderForName(const std::string& name);

	void foreachShaderName(const ShaderNameCallback& callback);

	void beginActiveShadersIterator();
	bool endActiveShadersIterator();
	IShader* dereferenceActiveShadersIterator();
	void incrementActiveShadersIterator();
	
	void setActiveShadersChangedNotify(const Callback& notify);
	
	void attach(ModuleObserver& observer);
	void detach(ModuleObserver& observer);

	void setLightingEnabled(bool enabled);

	const char* getTexturePrefix() const;

}; // class Doom3ShaderSystem

// Accessor function for the shader system
Doom3ShaderSystem& GetShaderSystem();

#endif /*DOOM3SHADERSYSTEM_H_*/
