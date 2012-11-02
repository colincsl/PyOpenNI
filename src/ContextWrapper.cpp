/* ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This file is part of PyOpenNI.
 *
 * PyOpenNI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PyOpenNI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with PyOpenNI.  If not, see <http://www.gnu.org/licenses/>.
 *
 * PyOpenNI is Copyright (C) 2011, Xavier Mendez (jmendeth).
 * OpenNI Python Wrapper (ONIPY) is Copyright (C) 2011, Gabriele Nataneli (gamix).
 *
 * ***** END GPL LICENSE BLOCK ***** */

#include "ContextWrapper.h"
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <XnTypes.h>
using namespace xn;


#include "wrapperTypes.h"
#include "wrapperExceptions.h"
#include "util/PythonOutputStream.h"

Device g_Device;
ScriptNode g_scriptNode;

void Context_InitFromXmlFile_wrapped(xn::Context& self, const std::string& initializationFilename) {

#ifdef _DEBUG
    PyCout << "Initializing OpenNI.." << std::endl;
#endif

    XnStatus returnCode;
    returnCode = self.InitFromXmlFile(initializationFilename.c_str());

#ifdef _DEBUG
    if (returnCode == XN_STATUS_OK)
        PyCout << "OpenNI was initialized successfully" << std::endl;
    else {
        PyCout << "OpenNI failed to initialize: " <<
                xnGetStatusName(returnCode) << std::endl;
    }
#endif

    return check(returnCode);
}

void Context_Shutdown_wrapped(xn::Context& self) {

#ifdef _DEBUG
    PyCout << "Shutting down OpenNI.." << std::endl;
#endif
    self.Shutdown();

}

void Context_Init_wrapped(xn::Context& self) {
 
#ifdef _DEBUG
    PyCout << "Initializing OpenNI.." << std::endl;
#endif

    XnStatus returnCode;
    returnCode = self.Init();

#ifdef _DEBUG
    if (returnCode == XN_STATUS_OK)
        PyCout << "OpenNI was initialized successfully" << std::endl;
    else {
        PyCout << "OpenNI failed to initialize: " <<
                xnGetStatusName(returnCode) << std::endl;
    }
#endif

    check(returnCode);    
}

BP::object Context_OpenFileRecording_wrapped(xn::Context& self, const std::string& recordedFile) {
#ifdef _DEBUG
    PyCout << "Loading recorded file.." << std::endl;
#endif

    XnStatus returnCode;
    returnCode = self.OpenFileRecording(recordedFile.c_str());

#ifdef _DEBUG
    if (returnCode == XN_STATUS_OK)
        PyCout << "Recording loaded successfully" << std::endl;
    else {
        PyCout << "Recording failed to load: " <<
                xnGetStatusName(returnCode) << std::endl;
    }
#endif

    check( returnCode );

    return Context_FindExistingNode_wrapped(self, XN_NODE_TYPE_PLAYER);
}

void Context_WaitAndUpdateAll_wrapped(xn::Context& self) {
    check( self.WaitAndUpdateAll() );
}

void Context_WaitAnyUpdateAll_wrapped(xn::Context& self) {
    check( self.WaitAnyUpdateAll() );
}

void Context_WaitNoneUpdateAll_wrapped(xn::Context& self) {
    check( self.WaitNoneUpdateAll() );
}

void Context_WaitOneUpdateAll_wrapped(xn::Context& self, xn::ProductionNode& node) {
    check( self.WaitOneUpdateAll(node) );
}

BP::object Context_FindExistingNode_wrapped(xn::Context& self, XnProductionNodeType type) {
    XnNodeHandle ret = NULL;
    xnFindExistingRefNodeByType(self.GetUnderlyingObject(), type, &ret);
    return wrapNode(ret);
}

void Context_InitFromXmlFileByDeviceID_wrapped(xn::Context& self, const std::string& initializationFilename, int deviceID) {

    int i=0;
	XnStatus nRetVal = XN_STATUS_OK;
	
    if (initializationFilename.compare("") != 0)
    {
        xnLogInitFromXmlFile(initializationFilename.c_str());
    }
    nRetVal = self.Init();
	check(nRetVal);
	
	// find devices
	NodeInfoList list;
	nRetVal = self.EnumerateProductionTrees(XN_NODE_TYPE_DEVICE, NULL, list);
	check(nRetVal);
	
    if (deviceID == -1)
    {
        printf("The following devices were found:\n");
    
        int i = 1;
        for (NodeInfoList::Iterator it = list.Begin(); it != list.End(); ++it, ++i)
        {
            NodeInfo deviceNodeInfo = *it;
            
            Device deviceNode;
            deviceNodeInfo.GetInstance(deviceNode);
            XnBool bExists = deviceNode.IsValid();
            if (!bExists)
            {
                self.CreateProductionTree(deviceNodeInfo, deviceNode);
                // this might fail.
            }
            
            if (deviceNode.IsValid() && deviceNode.IsCapabilitySupported(XN_CAPABILITY_DEVICE_IDENTIFICATION))
            {
                const XnUInt32 nStringBufferSize = 200;
                XnChar strDeviceName[nStringBufferSize];
                XnChar strSerialNumber[nStringBufferSize];
                
                XnUInt32 nLength = nStringBufferSize;
                deviceNode.GetIdentificationCap().GetDeviceName(strDeviceName, nLength);
                nLength = nStringBufferSize;
                deviceNode.GetIdentificationCap().GetSerialNumber(strSerialNumber, nLength);
                printf("[%d] %s (%s)\n", i, strDeviceName, strSerialNumber);
            }
            else
            {
                printf("[%d] %s\n", i, deviceNodeInfo.GetCreationInfo());
            }
            
            // release the device if we created it
            if (!bExists && deviceNode.IsValid())
            {
                deviceNode.Release();
            }
        }
    
        printf("\n");
        printf("Choose device to open (1): ");
	
        int nRetval = scanf("%d", &deviceID);
    }
	
	// create it
	NodeInfoList::Iterator it = list.Begin();
	for (i = 1; i < deviceID; ++i)
	{
		it++;
	}
	
	NodeInfo deviceNode = *it;
	nRetVal = self.CreateProductionTree(deviceNode, g_Device);
	check(nRetVal);
	
    if (initializationFilename.compare("") != 0)
    {
        // now run the rest of the XML
        printf("Init from config file.\n");
        nRetVal = self.RunXmlScriptFromFile(initializationFilename.c_str(), g_scriptNode);
        check(nRetVal);
    }

	return;	
}

void Context_InitFromXmlFileByChoice_wrapped(xn::Context& self, const std::string& initializationFilename) {
    Context_InitFromXmlFileByDeviceID_wrapped(self, initializationFilename, -1);
}

void Context_InitByDeviceID_wrapped(xn::Context& self, int deviceID) {
    const std::string initializationFilename = "";
    Context_InitFromXmlFileByDeviceID_wrapped(self, initializationFilename, deviceID);
}

int Context_GetDeviceCount_wrapped(xn::Context& self)
{
	XnStatus nRetVal = XN_STATUS_OK;
		
	nRetVal = self.Init();
	check(nRetVal);
	
	// find devices
	NodeInfoList list;
	nRetVal = self.EnumerateProductionTrees(XN_NODE_TYPE_DEVICE, NULL, list);
	check(nRetVal);
	   
    int deviceCount = 0;
	for (NodeInfoList::Iterator it = list.Begin(); it != list.End(); ++it)
        deviceCount++;
    
    self.Shutdown();
    
	return deviceCount;
}


void Context_StartGeneratingAll_wrapped(xn::Context& self) {
    check( self.StartGeneratingAll() );
}
void Context_StopGeneratingAll_wrapped(xn::Context& self) {
    check( self.StopGeneratingAll() );
}

XnBool Context_IsValid(xn::Context& self) {
    return (self.GetUnderlyingObject() != NULL);
}
