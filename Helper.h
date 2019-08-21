#if !defined(AFX_HELPER_H)
#define AFX_HELPER_H

#define SafeRelease(pInterface) if(pInterface != NULL) {pInterface->Release(); pInterface=NULL;}
#define SafeDelete(pObject) if(pObject != NULL) {delete pObject; pObject=NULL;}

#include <stdio.h>

#endif