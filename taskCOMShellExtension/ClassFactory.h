/****************************** Module Header ******************************\
Module Name:  ClassFactory.h
Project:      taskCOMShellExtension

The file declares the class factory for the FileContextMenuExt COM class. 
\***************************************************************************/

#ifndef CLASSFACTORY_H
#define CLASSFACTORY_H

#include "stdafx.h"


class ClassFactory : public IClassFactory {
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IClassFactory
    IFACEMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv);
    IFACEMETHODIMP LockServer(BOOL fLock);

    ClassFactory();

protected:
    ~ClassFactory();

private:
    long m_cRef;
};

#endif // CLASSFACTORY_H
