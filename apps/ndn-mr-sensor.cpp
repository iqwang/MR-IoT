/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "/usr/include/python2.7/Python.h"
#include "ndn-mr-sensor.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"

#include <memory>
#include <deque>
//////////////////

#include<unistd.h>
#include<stdio.h>
#include<errno.h>

/////////////////





NS_LOG_COMPONENT_DEFINE("ndn.MrSensor");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(MrSensor);

TypeId
MrSensor::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::MrSensor")
      .SetGroupName("Ndn")
      .SetParent<App>()
      .AddConstructor<MrSensor>()
      .AddAttribute("Prefix", "Prefix, for which producer has the data", StringValue("/"),
                    MakeNameAccessor(&MrSensor::m_prefix), MakeNameChecker())
      .AddAttribute(
         "Postfix",
         "Postfix that is added to the output data (e.g., for adding producer-uniqueness)",
         StringValue("/"), MakeNameAccessor(&MrSensor::m_postfix), MakeNameChecker())
      .AddAttribute("PayloadSize", "Virtual payload size for Content packets", UintegerValue(1024),
                    MakeUintegerAccessor(&MrSensor::m_virtualPayloadSize),
                    MakeUintegerChecker<uint32_t>())
      .AddAttribute("Freshness", "Freshness of data packets, if 0, then unlimited freshness",
                    TimeValue(Seconds(0)), MakeTimeAccessor(&MrSensor::m_freshness),
                    MakeTimeChecker())
      .AddAttribute(
         "Signature",
         "Fake signature, 0 valid signature (default), other values application-specific",
         UintegerValue(0), MakeUintegerAccessor(&MrSensor::m_signature),
         MakeUintegerChecker<uint32_t>())
      .AddAttribute("KeyLocator",
                    "Name to be used for key locator.  If root, then key locator is not used",
                    NameValue(), MakeNameAccessor(&MrSensor::m_keyLocator), MakeNameChecker());
  return tid;
}

MrSensor::MrSensor()
{
  NS_LOG_FUNCTION_NOARGS();
}

// inherited from Application base class.
void
MrSensor::StartApplication()
{
  NS_LOG_FUNCTION_NOARGS();
  App::StartApplication();

  FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
}

void
MrSensor::StopApplication()
{
  NS_LOG_FUNCTION_NOARGS();

  App::StopApplication();
}


bool mapperPython(std::string fileName, std::string kExpression, std::string vExpression, std::string *result){
	PyObject *pyName, *pyModule, *pyDict, *pyFunc, *pyArgs, *pyValue1, *pyValue2;

	pyName = PyString_FromString(fileName.c_str());
	/*char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL) std::cout << "Current Directory: " << cwd << std::endl;
	else perror("error");
	*/
	
	PySys_SetPath(".");
	pyModule = PyImport_Import(pyName);

	//std::cout << "33333333333333333" << std::endl;
	//std::cout << "" << sizeof(*pyModule) << std::endl;
	pyDict = PyModule_GetDict(pyModule);
	pyFunc = PyDict_GetItemString(pyDict, "mapper");
	
	/*
	pyArgs = PyTuple_New(2);
	std::cout << "6666666666666666" << std::endl;
	pyValue = PyInt_FromLong(2);
	std::cout << "77777777777777" << std::endl;
	PyTuple_SetItem(pyArgs, 0, pyValue);
	std::cout << "888888888888888" << std::endl;
	PyTuple_SetItem(pyArgs, 1, pyValue); 
	std::cout << "99999999999999" << std::endl;
	PyObject *pyResult = PyObject_CallObject(pyFunc, pyArgs);
	std::cout << "0000000000000" << std::endl;
	*/
	
	pyArgs = PyTuple_New(2);
	pyValue1 = PyString_FromString(kExpression.c_str());
  pyValue2 = PyString_FromString(vExpression.c_str());
	PyTuple_SetItem(pyArgs, 0, pyValue1);
  PyTuple_SetItem(pyArgs, 1, pyValue2);
	PyObject *pyResult = PyObject_CallObject(pyFunc, pyArgs);

	if(pyResult) {	
    PyObject* oR = PyObject_Repr(pyResult);
    *result = PyString_AsString(oR);
		return true;
		} else {
			std::cout<< "Error" << std::endl;
			return false;
			};	
	};
	  
/*    
std::string icn_map(std::string taskID, std::string key, std::string value){
	std::string dataSet;
	dataSet.append(taskID);
	std::string s1 = "/";
	std::string slashPos;
	std::deque<std::string> kList;
	uint8_t f1 = key.find(s1);
	if (f1 != std::string::npos){
		slashPos.push_back(f1);
		};
	while (f1 < key.size()-1){
		f1 = key.find(s1, f1+1);
		if (f1 != std::string::npos){
			slashPos.push_back(f1);
			};
		};

    for (uint8_t i =0; i<slashPos.size()-1; i++) {
		std::string str2 = key.substr(slashPos[i]+1, slashPos[i+1]-slashPos[i]-1);
		kList.push_back(str2);
		//std::cout<< "each k: "<< str2 <<std::endl;
		};
	
	Py_Initialize();
  std::string localFile = "MapFile";
  long pResult =0;
	std::string line = "-";
	std::string comma = ",";
	for (uint8_t i1=0; i1<kList.size(); i1++){
		dataSet.append(kList[i1]);	
		dataSet.append(line);
		int ran1 = rand() % 100 + 1;
		std::string ran2 = std::to_string(ran1);
		std::string userFunc =value.replace(0,1,ran2);
		//std::cout<< "v after replace: "<<s << std::endl;
		if(!callPython(localFile, userFunc, &pResult)) {
			std::cout<< "Run python error"<< std::endl;
			};
		std::string finResult = std::to_string(pResult);
		//std::cout<< "Run python: " <<ree << std::endl;
		dataSet.append(finResult);
		dataSet.append(comma);
		};
	//std::cout<< "map data: " <<dataSet << std::endl;
	return dataSet;
	Py_Finalize();	
};
*/

void
MrSensor::OnInterest(shared_ptr<const Interest> interest)
{
  App::OnInterest(interest); // tracing inside

  NS_LOG_FUNCTION(this << interest);

  if (!m_active)
    return;

  Name dataName(interest->getName());
  // dataName.append(m_postfix);
  // dataName.appendVersion();

  auto data = make_shared<Data>();
  data->setName(dataName);
  data->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));
  
	m_interestName = interest->getName().toUri();
	std::string slash1 = "/T";
	std::string slash2 = "-/map";
	int find1 = m_interestName.find(slash1);
	int find2 = m_interestName.find(slash2);
	//find the task id label
	std::string tNum = m_interestName.substr(find1+1, find2-find1-1);
	tNum.append(",");
	//std::cout<< "Map test0: " << tNum <<std::endl;
  
	std::string slash3 = ".k";
	std::string slash4 = ".v";
	std::string slash5 = "/%";
	int find3 = m_interestName.find(slash3);
	int find4 = m_interestName.find(slash4);
	int find5 = m_interestName.find(slash5);
	std::string kContent = m_interestName.substr(find3+3, find4-find3-3);
	std::string vContent = m_interestName.substr(find4+3, find5-find4-4);
	//std::cout<< "Map K: " << kContent <<" and V: " <<vContent <<std::endl;
	
  std::string dataSet;
	dataSet.append(tNum);
  Py_Initialize();
  std::string localFile = "MapFile";
  std::string pResult;
  
  if(!mapperPython(localFile, kContent, vContent, &pResult)) {
    std::cout<< "Run python error"<< std::endl;
    };  
  std::string tempData = pResult.substr(1,pResult.size()-2);
  dataSet.append(tempData);
  dataSet.push_back(',');
	std::string rawData = dataSet;
	//std::cout<< "Raw data: "<< rawData << std::endl;
	uint8_t* p = (uint8_t*) &(*rawData.begin());
    auto buffer = make_shared< ::ndn::Buffer>(p, rawData.size());
    data->setContent(buffer);

  /*
  char *taskNum = new char(tNum.length()+1);
  strcpy(taskNum, tNum.c_str());
  char *rawData = icn_map(taskNum);
  char buffer[10];
  strncpy(buffer, rawData, 10);
  //std::cout<< "Map Test000: " << icn_map(taskNum) <<std::endl;

  //put rawData into buffer: memcpy(dest, source, size)
  memcpy(buffer, &rawData, sizeof(int));
  //creat return content with the size of rawData
  data->setContent(reinterpret_cast<const uint8_t*>(buffer), sizeof(int));
 reinterpret_cast<new type>(expression): convert to new type
  data->setContent(reinterpret_cast<const uint8_t*>(buffer), rawData.size()); 
  //std::cout << "Send Buffer Size = " << data->getContent().getBuffer()->size() << std::endl;
   */
   
  Signature signature;
  SignatureInfo signatureInfo(static_cast< ::ndn::tlv::SignatureTypeValue>(255));

  if (m_keyLocator.size() > 0) {
    signatureInfo.setKeyLocator(m_keyLocator);
  }

  signature.setInfo(signatureInfo);
  signature.setValue(::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));

  data->setSignature(signature);

  NS_LOG_INFO("node(" << GetNode()->GetId() << ") responding with Data: " << data->getName());

  // to create real wire encoding
  data->wireEncode();

  m_transmittedDatas(data, this, m_face);
  m_appLink->onReceiveData(*data);
  std::cout << "Sensor return: " << rawData << std::endl;
}

} // namespace ndn
} // namespace ns3
