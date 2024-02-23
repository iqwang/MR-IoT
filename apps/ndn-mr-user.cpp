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

#include "ndn-mr-user.hpp"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"
#include <time.h>
#include <stdio.h>
#include <fstream>

NS_LOG_COMPONENT_DEFINE("ndn.MrUser");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(MrUser);

TypeId
MrUser::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::MrUser")
      .SetGroupName("Ndn")
      .SetParent<Consumer>()
      .AddConstructor<MrUser>()

      .AddAttribute("Frequency", "Frequency of interest packets", StringValue("10.0"),
                    MakeDoubleAccessor(&MrUser::m_frequency), MakeDoubleChecker<double>())

      .AddAttribute("Randomize",
                    "Type of send time randomization: none (default), uniform, exponential",
                    StringValue("none"),
                    MakeStringAccessor(&MrUser::SetRandomize, &MrUser::GetRandomize),
                    MakeStringChecker())

      .AddAttribute("MaxSeq", "Maximum sequence number to request",
                    IntegerValue(std::numeric_limits<uint32_t>::max()),
                    MakeIntegerAccessor(&MrUser::m_seqMax), MakeIntegerChecker<uint32_t>());

  return tid;
}

MrUser::MrUser()
  : m_frequency(1.0)
  , m_firstTime(true)
{
  NS_LOG_FUNCTION_NOARGS();
  m_seqMax = std::numeric_limits<uint32_t>::max();
  //m_interestName = "/mr--/map.k-/T/H/L/.v-v/10--/reduce.a+b";
  // m_interestName = "/mr--/map(k,v)->(k+1,v/2)-/reduce.a+b";
  //m_interestName = "/mr--/map.k";
  
  //m_interestName = "/r";
  //m_taskContent = "/map.k-k+1.v-v/10--/reduce.a+b";
  m_taskContent = "/map(k,v)->(k+1,v/2)-/reduce(a,b)->(a+b)";
  m_sendJobNeis.clear();
  m_buildTaskNeighbour = false;
  m_disNeiPrefix = "/nei-";
  m_ownPrefix = "/user-";
  m_disDownStream1 = "/discover(";
  m_disDownStream2 = ")-";
  m_disDownStream3 = "/(";
}


MrUser::~MrUser()
{
}

void
MrUser::SendPacket()
{
	if (!m_active)
	return;
  
	NS_LOG_FUNCTION_NOARGS();
  
  
  time_t now = std::time(0);
  struct tm tstruct;
  tstruct = *localtime(&now);
  char output[200];
  strftime(output, sizeof(output), "%Y-%M-%D.%X", &tstruct);
  std::ofstream recording;
  recording.open("userTimeRecord.txt", std::ios_base::app);
  recording << "##############" << std::endl<< "start: " << output << std::endl;
  recording.close();
  
  
  std::cout << "Job Neighbour Num: " << m_sendJobNeis.size() <<std::endl;
  std::cout << "One-hop Neighbour Num: " << m_oneHopNeighbours.size() <<std::endl;
   
  if(m_sendJobNeis.size() != 0) 
  {
    //send MapReduce Task
    std::cout << "User Finally!!!" <<std::endl;
    for(uint8_t j=0; j<m_sendJobNeis.size(); j++)
    {
      std::string taskString = m_sendJobNeis[j] + m_disDownStream3 + m_ownPrefix + m_disDownStream2 + m_taskContent + "-";
      std::cout << "Assign task: " << taskString << std::endl;
      shared_ptr<Name> taskName = make_shared<Name>(taskString);
      taskName->appendSequenceNumber(m_rand->GetValue(0, std::numeric_limits<uint16_t>::max()));
      shared_ptr<Interest> taskInterest = make_shared<Interest>();
      taskInterest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
      taskInterest->setName(*taskName);
      taskInterest->setInterestLifetime(time::milliseconds(m_interestLifeTime.GetMilliSeconds()));
      m_transmittedInterests(taskInterest, this, m_face);
      m_appLink->onReceiveInterest(*taskInterest);
      ScheduleNextPacket();
    }
  } 
  else if (m_oneHopNeighbours.size() != 0)
  {
    //start build task-tree
    for(uint8_t t=0; t<m_oneHopNeighbours.size(); t++)
    {
      m_downNeiMap.insert(std::pair<std::string, std::string>(m_oneHopNeighbours[t],"0"));
      std::string tempDisTree = m_oneHopNeighbours[t] + m_ownPrefix + m_disDownStream1 + m_ownPrefix + m_disDownStream2;
      //std::string tempDisTree = "/m1-";
      std::cout << "dis tree: " << tempDisTree << std::endl;
      shared_ptr<Name> disTreeName = make_shared<Name>(tempDisTree);
      disTreeName->appendSequenceNumber(m_rand->GetValue(0, std::numeric_limits<uint16_t>::max()));
      shared_ptr<Interest> disTreeInterest = make_shared<Interest>();
      disTreeInterest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
      disTreeInterest->setName(*disTreeName);
      disTreeInterest->setInterestLifetime(time::milliseconds(m_interestLifeTime.GetMilliSeconds()));
      m_transmittedInterests(disTreeInterest, this, m_face);
      m_appLink->onReceiveInterest(*disTreeInterest);
      m_sendDisDownNeiNum++;
      //ScheduleNextPacket();
    }  
  }
  else
  {
    //start discovery neighbours
    for(uint8_t i=0; i < m_allNodeName.size(); i++)
    {
      if(m_allNodeName[i] != m_ownPrefix)
      {
        m_checkNeibMap.insert(std::pair<std::string, std::string>(m_allNodeName[i],"0"));
        // written as: /nei-/user-
        std::string tempDisNei = m_disNeiPrefix + m_allNodeName[i];
        shared_ptr<Name> disNeiName = make_shared<Name>(tempDisNei);
        //originalInterest->appendSequenceNumber(m_rand->GetValue(0, std::numeric_limits<uint16_t>::max()));
        shared_ptr<Interest> disNeiInterest = make_shared<Interest>();
        disNeiInterest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
        disNeiInterest->setName(*disNeiName);
        disNeiInterest->setInterestLifetime(time::milliseconds(m_interestLifeTime.GetMilliSeconds()));
        m_transmittedInterests(disNeiInterest, this, m_face);
        m_appLink->onReceiveInterest(*disNeiInterest);
        m_sendDisNeiNum +=1;
      }
    };
  };
  
  
  /*
  //for multi-route test
  std::vector<int> rList = {1,2,3};
  std::string line = "-";
  for (uint i =0; i < rList.size(); i++) {
   //std::cout << reducerList[i] << std::endl;
    int n1 = rList[i];
    std::string n = std::to_string(n1);
    //std::cout << n << std::endl;
    m_interestToR = m_interestName + n + line + m_btTask;
    shared_ptr<Name> originalInterest = make_shared<Name>(m_interestToR);
    originalInterest->appendSequenceNumber(m_rand->GetValue(0, std::numeric_limits<uint16_t>::max()));
    shared_ptr<Interest> interestToR = make_shared<Interest>();
    interestToR->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
    interestToR->setName(*originalInterest);
    interestToR->setInterestLifetime(time::milliseconds(m_interestLifeTime.GetMilliSeconds()));
    std::cout << "Assign task to r" << i+1 << ": " << *originalInterest << std::endl;
    m_transmittedInterests(interestToR, this, m_face);
    m_appLink->onReceiveInterest(*interestToR);
    };
    ScheduleNextPacket();
 */

}

void
MrUser::ScheduleNextPacket()
{
  // double mean = 8.0 * m_payloadSize / m_desiredRate.GetBitRate ();
  // std::cout << "next: " << Simulator::Now().ToDouble(Time::S) + mean << "s\n";

  if (m_firstTime) {
    m_sendEvent = Simulator::Schedule(Seconds(0.0), &MrUser::SendPacket, this);
    m_firstTime = false;
  }
  else if (!m_sendEvent.IsRunning())
    m_sendEvent = Simulator::Schedule((m_random == 0) ? Seconds(1.0 / m_frequency)
                                                      : Seconds(m_random->GetValue()),
                                      &MrUser::SendPacket, this);
}

void
MrUser::SetRandomize(const std::string& value)
{
  if (value == "uniform") {
    m_random = CreateObject<UniformRandomVariable>();
    m_random->SetAttribute("Min", DoubleValue(0.0));
    m_random->SetAttribute("Max", DoubleValue(2 * 1.0 / m_frequency));
  }
  else if (value == "exponential") {
    m_random = CreateObject<ExponentialRandomVariable>();
    m_random->SetAttribute("Mean", DoubleValue(1.0 / m_frequency));
    m_random->SetAttribute("Bound", DoubleValue(50 * 1.0 / m_frequency));
  }
  else
    m_random = 0;

  m_randomType = value;
}

std::string
MrUser::GetRandomize() const
{
  return m_randomType;
}

void
MrUser::OnData(shared_ptr<const Data> data)
{
  //parse data content
    auto *tmpContent = ((uint8_t*)data->getContent().value());
    std::string receivedData;
    for(uint8_t i=0; i < data->getContent().value_size(); i++) {
      receivedData.push_back((char)tmpContent[i]);
    };
    //std::cout << "User Receive Data: " << receivedData << " from " << data->getName().toUri() << std::endl;
    std::string gotData = data->getName().toUri();
    uint64_t checkD= gotData.find("/discover");
    
    if(gotData[1] == 'n')
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
          std::cout << "receive all disNeiInterests" <<std::endl;
          for(it=m_checkNeibMap.begin(); it != m_checkNeibMap.end(); ++it)
          {
            if(it->second == "1")
            { 
              m_oneHopNeighbours.push_back(it->first);
              //std::cout << "it -> first: " << it->first <<std::endl;
            }
          }
          //std::cout << "next-hop neighbour number: " << m_oneHopNeighbours.size() <<std::endl;
          SendPacket();
          m_checkNeibMap.clear();
        }
    }
    else if(checkD != std::string::npos)
    {
      uint64_t temp1 = gotData.find("-");
      std::string temp2 = gotData.substr(0,temp1+1);
      std::map<std::string, std::string>::iterator downIt = m_downNeiMap.find(temp2);
      if(downIt != m_downNeiMap.end())
      {
        m_downNeiMap[temp2] = receivedData;
        m_gotDisDownNeiNum++;
      }
      
      if(m_gotDisDownNeiNum == m_sendDisDownNeiNum)
      {
        for(downIt=m_downNeiMap.begin(); downIt != m_downNeiMap.end(); ++downIt)
        {
          if(downIt->second == "yes")
          {
            //m_sendJobNeis += downIt->first;
            m_sendJobNeis.push_back(downIt->first);
          }
        }
        std::cout << "job Ref Neighbours: " << m_sendJobNeis.size() << std::endl;
        SendPacket();
        m_downNeiMap.clear();
        m_gotDisDownNeiNum = m_sendDisDownNeiNum =0;
      }
    }
    else
    {
      std::cout << "User Receive Data: " << receivedData << std::endl;
        time_t now = std::time(0);
        struct tm tstruct;
        tstruct = *localtime(&now);
        char output[200];
        strftime(output, sizeof(output), "%Y-%M-%D.%X", &tstruct);
        //std::cout << "User ends at: " << output <<std::endl;
        std::ofstream recording;
        recording.open("userTimeRecord.txt", std::ios_base::app);
        recording << "ends: " << output << std::endl;
        recording.close();
      
    }
	
}

void
MrUser::OnInterest(shared_ptr<const Interest> interest)
{


  App::OnInterest(interest); // tracing inside

  NS_LOG_FUNCTION(this << interest);

  if (!m_active)
    return;
  std::cout << " user!!! got Interest: " << interest->getName().toUri() <<std::endl;

}

} // namespace ndn
} // namespace ns3
