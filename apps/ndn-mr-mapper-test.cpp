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
#include "ndn-mr-mapper-test.hpp"
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





NS_LOG_COMPONENT_DEFINE("ndn.MrMapperTest");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(MrMapperTest);

TypeId
MrMapperTest::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::MrMapperTest")
      .SetGroupName("Ndn")
      .SetParent<App>()
      .AddConstructor<MrMapperTest>()
      .AddAttribute("Prefix", "Prefix, for which producer has the data", StringValue("/"),
                    MakeNameAccessor(&MrMapperTest::m_prefix), MakeNameChecker())
      .AddAttribute(
         "Postfix",
         "Postfix that is added to the output data (e.g., for adding producer-uniqueness)",
         StringValue("/"), MakeNameAccessor(&MrMapperTest::m_postfix), MakeNameChecker())
      .AddAttribute("PayloadSize", "Virtual payload size for Content packets", UintegerValue(1024),
                    MakeUintegerAccessor(&MrMapperTest::m_virtualPayloadSize),
                    MakeUintegerChecker<uint32_t>())
      .AddAttribute("Freshness", "Freshness of data packets, if 0, then unlimited freshness",
                    TimeValue(Seconds(0)), MakeTimeAccessor(&MrMapperTest::m_freshness),
                    MakeTimeChecker())
      .AddAttribute(
         "Signature",
         "Fake signature, 0 valid signature (default), other values application-specific",
         UintegerValue(0), MakeUintegerAccessor(&MrMapperTest::m_signature),
         MakeUintegerChecker<uint32_t>())
      .AddAttribute("KeyLocator",
                    "Name to be used for key locator.  If root, then key locator is not used",
                    NameValue(), MakeNameAccessor(&MrMapperTest::m_keyLocator), MakeNameChecker());
  return tid;
}

MrMapperTest::MrMapperTest()
{
  m_rand = CreateObject<UniformRandomVariable>();
  NS_LOG_FUNCTION_NOARGS();
}

// inherited from Application base class.
void
MrMapperTest::StartApplication()
{
  NS_LOG_FUNCTION_NOARGS();
  App::StartApplication();

  FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
}

void
MrMapperTest::StopApplication()
{
  NS_LOG_FUNCTION_NOARGS();

  App::StopApplication();
}

bool mapPython(std::string fileName, std::string kExpression, std::string vExpression, std::string *result)
{
	PyObject *pyName, *pyModule, *pyDict, *pyFunc, *pyArgs, *pyValue1, *pyValue2;

	pyName = PyString_FromString(fileName.c_str());
	
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


void
MrMapperTest::OnInterest(shared_ptr<const Interest> interest)
{
  App::OnInterest(interest); // tracing inside

  NS_LOG_FUNCTION(this << interest);

  if (!m_active)
    return;
  
    m_pendingInterestName = interest->getName();
    //std::cout <<"interest name is : " << m_pendingInterestName.toUri() << std::endl;
    uint64_t sp = m_pendingInterestName.toUri().find("/discover");
    //std::cout <<"p is: " << sp << std::endl;
  
    //interest for discover tree
    if (sp != std::string::npos) 
    {
      //std::cout <<m_prefix.toUri() <<"000 m_askPitNum: "<< m_askPitNum <<std::endl;
      std::cout <<m_prefix.toUri() << " got disTree Interest: "<< m_pendingInterestName.toUri()<< std::endl;
      m_askPitPrefix = "/p-";
      m_disTreeInterestMap.insert(std::pair<std::string, std::string>(m_pendingInterestName.toUri(),"0"));
      
      uint64_t t1 = m_pendingInterestName.toUri().find("(");
      uint64_t t2 = m_pendingInterestName.toUri().find(")");
      m_treeTag = m_pendingInterestName.toUri().substr(t1+1, t2-t1-1);
      //std::cout <<"tree tag: "<< m_treeTag <<std::endl;
  
      std::string tempAskPit = m_askPitPrefix + m_pendingInterestName.toUri();
      //std::cout <<"tempAskPit: "<< tempAskPit <<std::endl;
      shared_ptr<Name> askPitName = make_shared<Name>(tempAskPit);
      //subBtInterest->appendSequenceNumber(m_rand->GetValue(0, std::numeric_limits<uint16_t>::max()));
      shared_ptr<Interest> askPitInterest = make_shared<Interest>();
      askPitInterest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
      askPitInterest->setName(*askPitName);
      askPitInterest->setInterestLifetime(ndn::time::seconds(1));
      m_transmittedInterests(askPitInterest, this, m_face);
      m_appLink->onReceiveInterest(*askPitInterest);
      m_askPitNum++;
      //std::cout <<m_prefix.toUri() <<"111 m_askPitNum: "<< m_askPitNum <<std::endl;
    } 
    else 
    {
      std::cout << m_prefix.toUri() <<" get normal Interest: " << m_pendingInterestName.toUri() <<std::endl;
      std::string symbol = "->";
      std::string rSymbol = "/reduce";
      int mf1 = m_pendingInterestName.toUri().find(symbol);
      int mf2 = m_pendingInterestName.toUri().find(rSymbol);
      std::string mapFunc = m_pendingInterestName.toUri().substr(mf1+3, mf2-mf1-5);
      //std::cout << m_prefix.toUri() <<" map func: " << mapFunc <<std::endl;
      int mf3 = mapFunc.find(",");
      std::string kFunc = mapFunc.substr(0,mf3);
      std::string vFunc = mapFunc.substr(mf3+1);
      //std::cout << m_prefix.toUri() <<" k func: " << kFunc << " and v: " << vFunc <<std::endl;
      
      Py_Initialize();
      std::string localFile = "MapFile";
      std::string pResult;
      if(!mapPython(localFile, kFunc, vFunc, &pResult)) {
        std::cout<< "Run python error"<< std::endl;
      };  
      //std::cout<< "python result: "<< pResult << std::endl;
      std::string rawData = pResult.substr(1,pResult.size()-2);
 
      Name dataName(m_pendingInterestName);
      auto taskData = make_shared<Data>();
      taskData->setName(dataName);
      taskData->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));
      const uint8_t* p = reinterpret_cast<const uint8_t*>(rawData.c_str());
      auto buffer = make_shared< ::ndn::Buffer>(p, rawData.size());
      taskData->setContent(buffer);
   
      Signature signature;
      SignatureInfo signatureInfo(static_cast< ::ndn::tlv::SignatureTypeValue>(255));
      if (m_keyLocator.size() > 0) {
        signatureInfo.setKeyLocator(m_keyLocator);
      }
      signature.setInfo(signatureInfo);
      signature.setValue(::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));
      taskData->setSignature(signature);
      NS_LOG_INFO("node(" << GetNode()->GetId() << ") responding with Data: " << taskData->getName());

      // to create real wire encoding
      taskData->wireEncode();
      m_transmittedDatas(taskData, this, m_face);
      m_appLink->onReceiveData(*taskData);
      std::cout <<m_prefix.toUri() << " return: " << rawData << std::endl;
      rawData.clear();
    }
  
}

void
MrMapperTest::OnData(shared_ptr<const Data> data)
{
    if (!m_active)
    return;

    App::OnData(data); // tracing inside
    
    //parse data content
    auto *tmpContent = ((uint8_t*)data->getContent().value());
    std::string receivedData;
    for(uint8_t i=0; i < data->getContent().value_size(); i++) {
      receivedData.push_back((char)tmpContent[i]);
    };
    //std::cout << m_prefix.toUri() <<" get Data: " << receivedData << std::endl;
    std::string gotData = data->getName().toUri();
    
    switch(gotData[1])
    {
      case 'p':
      {
        //std::cout << m_prefix.toUri() <<" get pit Data: " << receivedData << std::endl;
        uint64_t tp = gotData.find_first_of("-");
        std::string tempInName = gotData.substr(tp+1);
        std::map<std::string, std::string>::iterator it = m_disTreeInterestMap.find(tempInName);
        //std::cout<<"tempInName " << tempInName << std::endl;
        if(it != m_disTreeInterestMap.end())
        {
          m_disTreeInterestMap[tempInName] = receivedData;
          m_gotPitNum++;
          //std::cout <<m_prefix.toUri() <<" m_gotPitNum: "<< m_gotPitNum <<std::endl;
        }
        
        if(m_gotPitNum == m_askPitNum)
        {
          //send Interest to ask FIB nexthop face
          m_askFibPrefix = "/f-";
          //m_askFibPrefix = "/r1-";
          std::string tempAskFib = m_askFibPrefix + m_treeTag;
          //std::cout <<m_prefix.toUri() <<" tempAskFib: "<< tempAskFib <<std::endl;
          shared_ptr<Name> askFibName = make_shared<Name>(tempAskFib);
          shared_ptr<Interest> askFibInterest = make_shared<Interest>();
          askFibInterest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
          askFibInterest->setName(*askFibName);
          askFibInterest->setInterestLifetime(ndn::time::seconds(1));
          m_transmittedInterests(askFibInterest, this, m_face);
          m_appLink->onReceiveInterest(*askFibInterest);
          //std::cout<< "go for fib round" <<std::endl;
          m_askPitNum = 0;
          m_gotPitNum = 0;
        }
        break;
      }
      
      case 'f':
      {
        //std::cout << m_prefix.toUri() <<" get fib Data: " << receivedData << std::endl;
        for(std::map<std::string, std::string>::iterator it=m_disTreeInterestMap.begin(); it!=m_disTreeInterestMap.end(); ++it)
        {
          std::string replyDisName = it->first;
          Name replyDisDataName(replyDisName);
          auto replyDisData = make_shared<Data>();
          replyDisData->setName(replyDisName);
          replyDisData->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));
        
          Signature signature;
          SignatureInfo signatureInfo(static_cast< ::ndn::tlv::SignatureTypeValue>(255));
          if (m_keyLocator.size() > 0) {
            signatureInfo.setKeyLocator(m_keyLocator);
          }
          signature.setInfo(signatureInfo);
          signature.setValue(::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));
          replyDisData->setSignature(signature);
          NS_LOG_INFO("node(" << GetNode()->GetId() << ") responding with Data: " << replyDisData->getName());
  
          if (it->second != receivedData)
          {
            std::string rawData("nope");
            const uint8_t* p = reinterpret_cast<const uint8_t*>(rawData.c_str());
            auto buffer = make_shared< ::ndn::Buffer>(p, rawData.size());
            replyDisData->setContent(buffer);
            // to create real wire encoding
            replyDisData->wireEncode();
            m_transmittedDatas(replyDisData, this, m_face);
            m_appLink->onReceiveData(*replyDisData);
            std::cout << m_prefix.toUri() << " reply Nack for disTree: " << rawData << std::endl;
          }
          // map size = 1 && the face = FIBface, got the selected upstream, not reply immediately, continue explore downstreams
          else 
          {
            std::string rawData("yes");
            const uint8_t* p = reinterpret_cast<const uint8_t*>(rawData.c_str());
            auto buffer = make_shared< ::ndn::Buffer>(p, rawData.size());
            replyDisData->setContent(buffer);
            
            // to create real wire encoding
            replyDisData->wireEncode();
            m_transmittedDatas(replyDisData, this, m_face);
            m_appLink->onReceiveData(*replyDisData);
            std::cout << m_prefix.toUri() << " reply Yes for disTree: " << rawData << std::endl;
          }
        }
        m_disTreeInterestMap.clear();
        break;
      }
    }
}


} // namespace ndn
} // namespace ns3
