#include "module.h"
#include "defines.h"
#include <dlfcn.h>

void CModule::Execute()
{
	if (!m_moduleFun){
		return;
	}
	m_moduleFun(m_paramfd);
}

void CModule::LoadModule()
{
	string moduleDir;
	char path[MAX_PATH] = {0};
	int rval = readlink("/proc/self/exe",path,sizeof(path));
	if (rval == -1){
		perror("readlink error!\n");
		exit(-1);
	}
	string server_path(path);
	moduleDir = server_path.substr(0,server_path.rfind('/'));
	
	//Load Module xxx.so
	char module_path[MAX_PATH] = {0};
	sprintf(module_path, "%s/%s", moduleDir.c_str(),m_moduleName.c_str());
	m_moduleHandle = dlopen(module_path, RTLD_NOW);

	if (m_moduleHandle == NULL) {
		perror("Could not load module");
		return;
	}
	//Find Function in the module
	m_moduleFun = (ModuleFun)dlsym(m_moduleHandle,"module_generate_page");
	if (m_moduleFun == NULL) {
		perror("Could not find module function");
		dlclose(m_moduleHandle);
		return;
	}
}

void CModule::UnloadModule()
{
	dlclose(m_moduleHandle);
}