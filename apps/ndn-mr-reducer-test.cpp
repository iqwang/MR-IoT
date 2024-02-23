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
#include "ndn-mr-reducer-test.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"
#include <memory>

#include <deque>

NS_LOG_COMPONENT_DEFINE("ndn.MrReducerTest");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(MrReducerTest);

TypeId
MrReducerTest::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::MrReducerTest")
      .SetGroupName("Ndn")
      .SetParent<App>()
      .AddConstructor<MrReducerTest>()
      .AddAttribute("Prefix", "Prefix, for which producer has the data", StringValue("/"),
                    MakeNameAccessor(&MrReducerTest::m_prefix), MakeNameChecker())
      .AddAttribute("Postfix", "Postfix that is added to the output data (e.g., for adding producer-uniqueness)",
					StringValue("/"), MakeNameAccessor(&MrReducerTest::m_postfix), MakeNameChecker())
      .AddAttribute("PayloadSize", "Virtual payload size for Content packets", UintegerValue(1024),
                    MakeUintegerAccessor(&MrReducerTest::m_virtualPayloadSize),
                    MakeUintegerChecker<uint32_t>())
      .AddAttribute("Freshness", "Freshness of data packets, if 0, then unlimited freshness",
                    TimeValue(Seconds(0)), MakeTimeAccessor(&MrReducerTest::m_freshness),
                    MakeTimeChecker())
      .AddAttribute("Signature", "Fake signature, 0 valid signature (default), other values application-specific",
					UintegerValue(0), MakeUintegerAccessor(&MrReducerTest::m_signature),
					MakeUintegerChecker<uint32_t>())
      .AddAttribute("KeyLocator", "Name to be used for key locator.  If root, then key locator is not used",
                    NameValue(), MakeNameAccessor(&MrReducerTest::m_keyLocator), MakeNameChecker()); 
  return tid;
}

MrReducerTest::MrReducerTest()
{


  m_rand = CreateObject<UniformRandomVariable>();
  NS_LOG_FUNCTION_NOARGS();
}

// inherited from Application base class.
void
MrReducerTest::StartApplication()
{
  NS_LOG_FUNCTION_NOARGS();
  App::StartApplication();

  FibHelper::AddRoute(GetNode(), m_prefix, m_face, 0);
}

void
MrReducerTest::StopApplication()
{
  NS_LOG_FUNCTION_NOARGS();

  App::StopApplication();
}


void
MrReducerTest::FindNeighbours()
{
  //std::cout << "find neighbours: " << m_pendingInterestName.toUri() << std::endl;
  m_disNeiPrefix = "/nei-";
  for(uint8_t i=0; i < m_allNodeName.size(); i++)
    {
      if(m_allNodeName[i] != m_prefix.toUri())
      {
        m_checkNeibMap.insert(std::pair<std::string, std::string>(m_allNodeName[i],"0"));
        // written as: /nei-/user-
        std::string tempDisNei = m_disNeiPrefix + m_allNodeName[i];
        shared_ptr<Name> disNeiName = make_shared<Name>(tempDisNei);
        //originalInterest->appendSequenceNumber(m_rand->GetValue(0, std::numeric_limits<uint16_t>::max()));
        shared_ptr<Interest> disNeiInterest = make_shared<Interest>();
        disNeiInterest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
        disNeiInterest->setName(*disNeiName);
        disNeiInterest->setInterestLifetime(ndn::time::seconds(1));
        m_transmittedInterests(disNeiInterest, this, m_face);
        m_appLink->onReceiveInterest(*disNeiInterest);
        m_sendDisNeiNum +=1;
      }
    };
};


void
MrReducerTest::DiscoverDownstreams()
{
  m_disDownStream1 = "/discover(";
  m_disDownStream2 = ")-";
  //std::cout << m_prefix.toUri() << "continue discover: " << m_upDisNodes.size() << std::endl;
  
  if(m_oneHopNeighbours.size() == 0) 
  {
    FindNeighbours();
  }
  else
  {
    //std::cout << m_prefix.toUri() << "continue discover: " << m_upDisNodes.size() << std::endl;
    for(uint8_t t=0; t<m_oneHopNeighbours.size(); t++)
    {
      if(m_oneHopNeighbours[t] != m_treeTag && m_oneHopNeighbours[t] != m_selectNodeName )
      {
        m_disDownNodeMap.insert(std::pair<std::string, std::string>(m_oneHopNeighbours[t],"0"));
        std::string tempDisTree = m_oneHopNeighbours[t] + m_prefix.toUri() + m_disDownStream1 + m_treeTag + m_disDownStream2;
        //std::string tempDisTree = m_oneHopNeighbours[t];
        std::cout << m_prefix.toUri() << " send dis tree: " << tempDisTree << std::endl;
        shared_ptr<Name> disTreeName = make_shared<Name>(tempDisTree);
        disTreeName->appendSequenceNumber(m_rand->GetValue(0, std::numeric_limits<uint16_t>::max()));
        shared_ptr<Interest> disTreeInterest = make_shared<Interest>();
        disTreeInterest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
        disTreeInterest->setName(*disTreeName);
        disTreeInterest->setInterestLifetime(ndn::time::seconds(1));
        m_transmittedInterests(disTreeInterest, this, m_face);
        m_appLink->onReceiveInterest(*disTreeInterest);
        m_sendDisDownNeiNum++;
      }
      else if (m_oneHopNeighbours.size() ==1 && m_oneHopNeighbours[0] == m_selectNodeName)
      {
        // reply nope to upstream because no downstream
        std::string replyNackName = m_selectUpstream;
        Name replyNackDataName(replyNackName);
        auto replyNackData = make_shared<Data>();
        replyNackData->setName(replyNackDataName);
        replyNackData->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));
        std::string rawData("nope");
        const uint8_t* p = reinterpret_cast<const uint8_t*>(rawData.c_str());
        auto buffer = make_shared< ::ndn::Buffer>(p, rawData.size());
        replyNackData->setContent(buffer);
          
        Signature signature;
        SignatureInfo signatureInfo(static_cast< ::ndn::tlv::SignatureTypeValue>(255));

        if (m_keyLocator.size() > 0) {
          signatureInfo.setKeyLocator(m_keyLocator);
        }
        signature.setInfo(signatureInfo);
        signature.setValue(::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));

        replyNackData->setSignature(signature);
        NS_LOG_INFO("node(" << GetNode()->GetId() << ") responding with Data: " << replyNackData->getName());

        // to create real wire encoding
        replyNackData->wireEncode();
        m_transmittedDatas(replyNackData, this, m_face);
        m_appLink->onReceiveData(*replyNackData);
        std::cout << m_prefix.toUri() << " reply (no downstream): " << rawData << " for: " << replyNackName << std::endl;
      }
    }
  }
    
};



void
MrReducerTest::QueryPit()
{
  //std::cout << m_prefix.toUri() <<" map size " <<m_buildTreeInterestMap.size() << std::endl;
  for(std::map<std::string, std::string>::iterator it=m_buildTreeInterestMap.begin(); it!=m_buildTreeInterestMap.end(); ++it)
  {
    m_askPitPrefix = "/p-";
    std::string tempAskPit = m_askPitPrefix + it->first;
    //std::cout << m_prefix.toUri() <<"tempAskPit: "<< tempAskPit <<std::endl;
    shared_ptr<Name> askPitName = make_shared<Name>(tempAskPit);
    //subBtInterest->appendSequenceNumber(m_rand->GetValue(0, std::numeric_limits<uint16_t>::max()));
    shared_ptr<Interest> askPitInterest = make_shared<Interest>();
    askPitInterest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
    askPitInterest->setName(*askPitName);
    askPitInterest->setInterestLifetime(ndn::time::seconds(1));
    m_transmittedInterests(askPitInterest, this, m_face);
    m_appLink->onReceiveInterest(*askPitInterest);
    m_askPitNum++;
  }  
};



bool reducePython(std::string fileName, std::string expression, std::string kvInput, std::string *result){
    PyObject *pyName, *pyModule, *pyDict, *pyFunc, *pyArgs, *pyValue1,*pyValue2;

    pyName = PyString_FromString(fileName.c_str());
    PySys_SetPath(".");
    pyModule = PyImport_Import(pyName);

    //std::cout << "33333333333333333" << std::endl;
    //std::cout << "" << sizeof(*pyModule) << std::endl;
    pyDict = PyModule_GetDict(pyModule);
    pyFunc = PyDict_GetItemString(pyDict, "reducer");
    //std::cout << "44444444444444" << std::endl;
    pyArgs = PyTuple_New(2);
    pyValue1 = PyString_FromString(expression.c_str());
    pyValue2 = PyString_FromString(kvInput.c_str());
    //std::cout << "5555555555555" << std::endl;
    PyTuple_SetItem(pyArgs, 0, pyValue1);
    PyTuple_SetItem(pyArgs, 1, pyValue2);
    //std::cout << "6666666666666" << std::endl;
  
    PyObject *pyResult = PyObject_CallObject(pyFunc, pyArgs);
    //std::cout << "wqwqw" << pyResult->ob_type << std::endl;
    if(pyResult) {	
      PyObject* oR = PyObject_Repr(pyResult);
      //const char* s = PyString_AsString(oR);
      *result = PyString_AsString(oR);
      //std::cout << "result: " << *result << std::endl;
      return true;
      } else {
        std::cout<< "Error" << std::endl;
        return false;
        };
};


void
MrReducerTest::OnInterest(shared_ptr<const Interest> interest)
{
  App::OnInterest(interest); // tracing inside
  NS_LOG_FUNCTION(this << interest);

  if (!m_active)
    return;
    
    m_pendingInterestName = interest->getName();
    //std::cout <<"interest name is : " << m_pendingInterestName.toUri() << std::endl;
    uint64_t p = m_pendingInterestName.toUri().find("/discover");
    //std::cout <<"p is: " << p << std::endl;
    
    //get current userId
    uint64_t t1 = m_pendingInterestName.toUri().find("(");
    uint64_t t2 = m_pendingInterestName.toUri().find(")");
    m_treeTag = m_pendingInterestName.toUri().substr(t1+1, t2-t1-1);
    //std::cout <<"tree tag: "<< m_treeTag <<std::endl;
    
    // Interest for build tree
    if (p != std::string::npos) 
    {
      //check if discovery procedure already done for current userId
      std::map<std::string, std::string>::iterator userIdIter = m_jobRefMap.find(m_treeTag);
      if(userIdIter != m_jobRefMap.end())
      {
       //discovery done
       std::cout <<"Report discovery done"<<std::endl;
      }
      else 
      {
        if (m_currentTreeFlag == "0") 
        { 
          m_currentTreeFlag = m_treeTag;
        }

        if (m_currentTreeFlag == m_treeTag) 
        {
          //check if get upstream node from fib
          std::map<std::string, std::string>::iterator fibIter = m_fibResult.find(m_treeTag);
          //not found
          if(fibIter == m_fibResult.end())
          {
            m_fibResult.insert(std::pair<std::string, std::string>(m_treeTag,"0"));
            m_buildTreeInterestMap.insert(std::pair<std::string, std::string>(m_pendingInterestName.toUri(),"0"));
            //send Interest to ask FIB nexthop face
            m_askFibPrefix = "/f-";
            std::string tempAskFib = m_askFibPrefix + m_treeTag;
            //std::cout <<"tempAskFib: "<< tempAskFib <<std::endl;
            shared_ptr<Name> askFibName = make_shared<Name>(tempAskFib);
            shared_ptr<Interest> askFibInterest = make_shared<Interest>();
            askFibInterest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
            askFibInterest->setName(*askFibName);
            askFibInterest->setInterestLifetime(ndn::time::seconds(1));
            m_transmittedInterests(askFibInterest, this, m_face);
            m_appLink->onReceiveInterest(*askFibInterest);
            //std::cout<< "go for fib round" <<std::endl;
          }
          //get fib info already
          else
          {
            m_askPitPrefix = "/p-";
            m_buildTreeInterestMap.insert(std::pair<std::string, std::string>(m_pendingInterestName.toUri(),"0"));
            std::string tempAskPit = m_askPitPrefix + m_pendingInterestName.toUri();
            //std::cout << m_prefix.toUri() <<"tempAskPit: "<< tempAskPit <<std::endl;
            shared_ptr<Name> askPitName = make_shared<Name>(tempAskPit);
            //subBtInterest->appendSequenceNumber(m_rand->GetValue(0, std::numeric_limits<uint16_t>::max()));
            shared_ptr<Interest> askPitInterest = make_shared<Interest>();
            askPitInterest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
            askPitInterest->setName(*askPitName);
            askPitInterest->setInterestLifetime(ndn::time::seconds(1));
            m_transmittedInterests(askPitInterest, this, m_face);
            m_appLink->onReceiveInterest(*askPitInterest);
            m_askPitNum++;
          }
        }
        else
        {
          //other tree is processing, current tree build should wait
          m_pendingTreeTagList.push_back(m_treeTag);
          //store pending interest and waiting userID
          m_pendTreeJobMap.insert(std::pair<std::string, std::string>(m_pendingInterestName.toUri(),m_treeTag));
        }
      }      
    } 
    //normal Interest
    else 
    {
      std::cout << m_prefix.toUri() <<" get normal Interest: " << m_pendingInterestName.toUri() <<std::endl;
      m_taskInterestName = m_pendingInterestName.toUri();
      uint64_t u1 = m_pendingInterestName.toUri().find("(");
      uint64_t u2 = m_pendingInterestName.toUri().find(")");
      std::string userId = m_pendingInterestName.toUri().substr(u1+1, u2-u1-1);
      //std::cout << m_prefix.toUri() << " userID: " << userId << std::endl;
      std::map<std::string, std::string>::iterator fUser = m_jobRefMap.find(userId);
      //std::cout << m_prefix.toUri() << " neis: " << fUser->second << std::endl;
      if(fUser != m_jobRefMap.end())
      {
        if(m_nodeList4Task.size() == 0)
        {
          std::string taskRefNodes = fUser->second;
          //std::cout << m_prefix.toUri() << " find task Ref: " << taskRefNodes << std::endl;
          std::deque<int> slashQ;
          std::deque<int> lineQ;
          for(uint8_t k=0; k<taskRefNodes.size(); k++)
          { 
            if(taskRefNodes[k] == '/')
            {
              slashQ.push_back(k);
            }
            if(taskRefNodes[k] == '-')
            {
              lineQ.push_back(k);
            }
          }
          
          for(uint8_t k1=0; k1<slashQ.size(); k1++)
          {
            std::string tempNodeName =  taskRefNodes.substr(slashQ[k1],lineQ[k1]-slashQ[k1]+1);
            //std::cout << m_prefix.toUri() << " 1. node name: " << tempNodeName << std::endl;
            m_nodeList4Task.push_back(tempNodeName);
          }
        }
        
        std::string slash = "-/%";
        std::string ab = "(a,b)";
        std::string line = "-";
        int find2 = m_taskInterestName.find(slash);
        int find4 = m_taskInterestName.find(ab);
        int find5 = m_taskInterestName.find_first_of(line);
        m_assignTask = m_taskInterestName.substr(find5+1, find2-find5-1);
        m_reduceFunc = m_taskInterestName.substr(find4+8, 3);
        //std::cout <<"combine task: " << m_assignTask<< std::endl;
       //std::cout <<m_prefix.toUri() << "reduce function: " << m_reduceFunc << std::endl;
      
        for(uint8_t s=0; s<m_nodeList4Task.size(); s++)
        {
          std::string creatTask = m_nodeList4Task[s] + m_assignTask + "-";
          //std::cout << m_prefix.toUri() << " creat: " << creatTask << std::endl; 
          shared_ptr<Name> mapTaskName = make_shared<Name>(creatTask);
          mapTaskName->appendSequenceNumber(m_rand->GetValue(0, std::numeric_limits<uint16_t>::max()));
          shared_ptr<Interest> mapTaskInterest = make_shared<Interest>();
          mapTaskInterest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
          mapTaskInterest->setName(*mapTaskName);
          mapTaskInterest->setInterestLifetime(ndn::time::seconds(1));
          //std::cout << "111 to sensor " << ": " << *subInterest << std::endl;
          m_transmittedInterests(mapTaskInterest, this, m_face);
          m_appLink->onReceiveInterest(*mapTaskInterest);
          m_sendTaskNum++;
          creatTask.clear();
        }
        
        m_nodeList4Task.clear();
      }
      else 
      {
        //no job reference for current user
        std::cout <<"Return data to user: deploy failed, start discovery process"<<std::endl;
      }
    }
 
};


void
MrReducerTest::OnData(shared_ptr<const Data> data)
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
    //std::cout << m_prefix.toUri() <<" get Data: " << receivedData << " from " << data->getName().toUri() << std::endl;
    std::string gotData = data->getName().toUri();
    
    switch(gotData[1])
    {
      case 'p':
      {
        uint64_t tp = gotData.find_first_of("-");
        std::string tempInName = gotData.substr(tp+1);
        std::map<std::string, std::string>::iterator it = m_buildTreeInterestMap.find(tempInName);
        //std::cout<<"tempInName " << tempInName << std::endl;
        if(it != m_buildTreeInterestMap.end())
        {
          m_buildTreeInterestMap[tempInName] = receivedData;
          m_gotPitNum++;
        }
        
        if(m_askPitNum == m_gotPitNum)
        {
          std::map<std::string, std::string>::iterator fibIt = m_fibResult.find(m_treeTag);
          std::string fibContent = fibIt->second; 
          for(std::map<std::string, std::string>::iterator it=m_buildTreeInterestMap.begin(); it!=m_buildTreeInterestMap.end(); ++it)
          {
            //std::cout << m_prefix.toUri() << " 111 " << it->first << std::endl;
            if (it->second != fibContent)
            {
              //std::cout << m_prefix.toUri() << " 222 " << it->first << std::endl;
              std::string replyNackName = it->first;
              Name replyNackDataName(replyNackName);
              auto replyNackData = make_shared<Data>();
              replyNackData->setName(replyNackDataName);
              replyNackData->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));
              std::string rawData("nope");
              const uint8_t* p = reinterpret_cast<const uint8_t*>(rawData.c_str());
              auto buffer = make_shared< ::ndn::Buffer>(p, rawData.size());
              replyNackData->setContent(buffer);
          
              Signature signature;
              SignatureInfo signatureInfo(static_cast< ::ndn::tlv::SignatureTypeValue>(255));

              if (m_keyLocator.size() > 0) {
                signatureInfo.setKeyLocator(m_keyLocator);
              }

              signature.setInfo(signatureInfo);
              signature.setValue(::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));

              replyNackData->setSignature(signature);
  
              NS_LOG_INFO("node(" << GetNode()->GetId() << ") responding with Data: " << replyNackData->getName());

              // to create real wire encoding
              replyNackData->wireEncode();
              m_transmittedDatas(replyNackData, this, m_face);
              m_appLink->onReceiveData(*replyNackData);
              std::cout << m_prefix.toUri() << "R reply: " << rawData << " for: " << replyNackName << std::endl;
            }
            else 
            {
              // map size = 1 && the face = FIBface, got the selected upstream, not reply immediately, continue explore downstreams
              m_selectUpstream = it->first;
              //std::cout<< "selected: " << m_selectUpstream <<std::endl;
      
              //upstream node name
              uint64_t p1 = m_selectUpstream.find_first_of("-");
              uint64_t p2 = m_selectUpstream.find("/discover");
              m_selectNodeName = m_selectUpstream.substr(p1+1, p2-p1-1);
              //std::cout<<m_prefix.toUri() << " selected: " << m_selectNodeName <<std::endl;
              
              //initiate new round to ask one-hop neighbour
              if (m_oneHopNeighbours.size() != 0)
              {
                //send interests to one-hop to discover tree
                DiscoverDownstreams(); 
              }
              else 
              {
                FindNeighbours();
              }
              
            }
          }
        }
        m_buildTreeInterestMap.clear();
        break;
        break;
      }
      
      case 'f':
      {
        //std::cout <<m_prefix.toUri() << "fib data: " << receivedData <<std::endl;
        std::map<std::string, std::string>::iterator fibIter1 = m_fibResult.find(m_treeTag);
        if(fibIter1 != m_fibResult.end())
        {
          fibIter1->second = receivedData;
          QueryPit();
        }
        break;
      }
      
      case 'n':
      {
        uint64_t d1 = gotData.find("-");
        std::string neiName = gotData.substr(d1+1, gotData.size()-d1-1);
        std::map<std::string, std::string>::iterator it = m_checkNeibMap.find(neiName);
        if(it != m_checkNeibMap.end())
        {
          //const uint8_t* p = reinterpret_cast<const uint8_t*>(rawData.c_str());
          //const uint8_t* tempVal = reinterpret_cast<const uint8_t*>(receivedData.c_str());
          m_checkNeibMap[neiName] = receivedData;
          m_gotDisNeiNum += 1;
        }
        //std::cout << "neighour name: " << neiName << std::endl;
        if(m_sendDisNeiNum == m_gotDisNeiNum) 
        { 
          //std::cout << "R receive all disNeiInterests" <<std::endl;
          for(it=m_checkNeibMap.begin(); it != m_checkNeibMap.end(); ++it)
          {
            if(it->second == "1")
            { 
              m_oneHopNeighbours.push_back(it->first);
              //std::cout << "it -> first: " << it->first <<std::endl;
            }
          }
          //std::cout << "next-hop neighbour number: " << m_oneHopNeighbours.size() <<std::endl;
          DiscoverDownstreams();
          m_checkNeibMap.clear();
        }
        break;
      }
    }
    
    uint64_t findDis = gotData.find("/discover");
    uint64_t findDis2 = gotData.find("/map");
    if (findDis != std::string::npos && gotData[1] != 'p')
    {
      m_gotDisDownNeiNum++;
      std::cout << m_prefix.toUri() <<" get Data: " << receivedData << " from " << data->getName().toUri() << std::endl;
      std::string uriData = data->getName().toUri();
      uint64_t fu = uriData.find_first_of("-");
      std::string receDownNode = uriData.substr(0,fu+1);
      //std::cout << m_prefix.toUri() <<" get Data from " << receDownNode << " content is "<< receivedData << std::endl;
      //std::cout << m_prefix.toUri() << m_gotDisDownNeiNum << " and " << m_sendDisDownNeiNum << std::endl;
      std::map<std::string, std::string>::iterator disDownIt;
      for(disDownIt =m_disDownNodeMap.begin(); disDownIt!=m_disDownNodeMap.end(); ++disDownIt)
      {
        //std::cout << m_prefix.toUri() << "iterator first " << disDownIt->first << std::endl;
        if(disDownIt->first == receDownNode)
        {
          disDownIt->second = receivedData;
        }
      }
      
      if(m_gotDisDownNeiNum == m_sendDisDownNeiNum)
      {
        for(disDownIt =m_disDownNodeMap.begin(); disDownIt!=m_disDownNodeMap.end(); ++disDownIt)
        {
          if(disDownIt->second == "yes")
          {
            m_jobRefNei += disDownIt->first;
            std::cout << m_prefix.toUri() <<" jobRef nei " << m_jobRefNei << std::endl;
          }
        }
        
        //reply for upstream node
        Name replyYesDataName(m_selectUpstream);
        auto replyYesData = make_shared<Data>();
        replyYesData->setName(replyYesDataName);
        replyYesData->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));
        std::string rawData;
        Signature signature;
        SignatureInfo signatureInfo(static_cast< ::ndn::tlv::SignatureTypeValue>(255));
        if (m_keyLocator.size() > 0) {
          signatureInfo.setKeyLocator(m_keyLocator);
        }
        signature.setInfo(signatureInfo);
        signature.setValue(::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));
        replyYesData->setSignature(signature);
        NS_LOG_INFO("node(" << GetNode()->GetId() << ") responding with Data: " << replyYesData->getName());
        
        if(m_jobRefNei != "0")
        {
          // store the downstrem nodes for current task
          m_jobRefMap.insert(std::pair<std::string, std::string>(m_currentTreeFlag,m_jobRefNei));
          
          rawData ="yes";
          const uint8_t* p = reinterpret_cast<const uint8_t*>(rawData.c_str());
          auto buffer = make_shared< ::ndn::Buffer>(p, rawData.size());
          replyYesData->setContent(buffer);
          replyYesData->wireEncode();
          m_transmittedDatas(replyYesData, this, m_face);
          m_appLink->onReceiveData(*replyYesData);
          std::cout << m_prefix.toUri() << " reply " << rawData << " for " << m_selectUpstream << std::endl;
  
          m_currentTreeFlag = "0";
        }
        else 
        {
          rawData = "nope";
          const uint8_t* p = reinterpret_cast<const uint8_t*>(rawData.c_str());
          auto buffer = make_shared< ::ndn::Buffer>(p, rawData.size());
          replyYesData->setContent(buffer);
          replyYesData->wireEncode();
          m_transmittedDatas(replyYesData, this, m_face);
          m_appLink->onReceiveData(*replyYesData);
          std::cout << m_prefix.toUri() << " reply NOPE for discover Interest: " << rawData << std::endl;
          
          //m_gotDisDownNeiNum = m_sendDisDownNeiNum =0;
          m_currentTreeFlag = "0";
        }
      }
    }
    //data for normal Interest
    else if (findDis2 != std::string::npos)
    {
      m_gotTaskNum++;
      //std::cout<< m_prefix.toUri() << " receive: " << receivedData << std::endl;
      if(m_tContent == "0")
      {
          m_tContent = receivedData;
          m_tContent += ",";
      }
      else
      {
        m_tContent += receivedData;
        m_tContent += ",";
      }
      //std::cout<< m_prefix.toUri() << " receive: " << m_tContent << std::endl;

      if(m_gotTaskNum == m_sendTaskNum)
      {
        Name dataName(m_pendingInterestName);
        auto taskData = make_shared<Data>();
        taskData->setName(dataName);
        taskData->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));
        std::string rawData;
        
        Signature signature;
        SignatureInfo signatureInfo(static_cast< ::ndn::tlv::SignatureTypeValue>(255));
        if (m_keyLocator.size() > 0) {
          signatureInfo.setKeyLocator(m_keyLocator);
        }
        signature.setInfo(signatureInfo);
        signature.setValue(::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));
        taskData->setSignature(signature);
        NS_LOG_INFO("node(" << GetNode()->GetId() << ") responding with Data: " << taskData->getName());
        
        uint8_t lineF = m_tContent.find_last_of("-");
        if (lineF>5) 
        {
          //std::cout<< m_prefix.toUri() << "reduce func: " << m_reduceFunc << " and " << m_tContent << std::endl;
          std::string localFile = "ReduceFile";
          std::string pResult;
          if(!reducePython(localFile, m_reduceFunc, m_tContent, &pResult)) {
            std::cout<< "Run python error"<< std::endl;
            };  
          //std::cout<< "python return result: " <<pResult << std::endl;
          rawData = pResult.substr(1,pResult.size()-3);
          const uint8_t* p = reinterpret_cast<const uint8_t*>(rawData.c_str());
          auto buffer = make_shared< ::ndn::Buffer>(p, rawData.size());
          taskData->setContent(buffer);
  
          // to create real wire encoding
          taskData->wireEncode();
          m_transmittedDatas(taskData, this, m_face);
          m_appLink->onReceiveData(*taskData);
          std::cout <<m_prefix.toUri() << " return: " << rawData << std::endl;
          
          m_assignTask.clear();
          m_reduceFunc.clear();
          rawData.clear();
          pResult.clear();
          m_tContent.clear();
        }
        else
        {
          rawData = receivedData.substr(0,receivedData.size());
          const uint8_t* p = reinterpret_cast<const uint8_t*>(rawData.c_str());
          auto buffer = make_shared< ::ndn::Buffer>(p, rawData.size());
          taskData->setContent(buffer);
          // to create real wire encoding
          taskData->wireEncode();
          m_transmittedDatas(taskData, this, m_face);
          m_appLink->onReceiveData(*taskData);
          std::cout <<m_prefix.toUri() << " return: " << rawData << std::endl;
          
          m_assignTask.clear();
          m_reduceFunc.clear();
          rawData.clear();
          receivedData.clear();
          m_tContent.clear();
        }
      }
    }
};


} // namespace ndn
} // namespace ns3
