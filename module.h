#ifndef MODULE_H
#define MODULE_H

#include "defines.h"

class CModule
{
public:
	CModule(string module_name,int fd):m_moduleName(module_name),m_paramfd(fd){
			LoadModule();
	}
	~CModule(){UnloadModule();}
	void Execute();
private:
	void LoadModule();
	void UnloadModule();
	void GetCurrentDir();
private:
	typedef void (*ModuleFun)(int);
	string	  m_moduleName;
	void*	  m_moduleHandle;
	ModuleFun m_moduleFun;
	int       m_paramfd;
};

#endif // MODULE_H