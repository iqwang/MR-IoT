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

#include "ndn-mr-normal-user.hpp"
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

#include "utils/ndn-ns3-packet-tag.hpp"
#include "utils/ndn-rtt-mean-deviation.hpp"

#include <ndn-cxx/lp/tags.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/ref.hpp>

#include <time.h>
#include <stdio.h>
#include <fstream>

NS_LOG_COMPONENT_DEFINE("ndn.MrNormalUser");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(MrNormalUser);

TypeId
MrNormalUser::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::MrNormalUser")
      .SetGroupName("Ndn")
      .SetParent<Consumer>()
      .AddConstructor<MrNormalUser>()
      
      .AddAttribute("Frequency", "Frequency of interest packets", StringValue("10.0"),
                    MakeDoubleAccessor(&MrNormalUser::m_frequency), MakeDoubleChecker<double>())

      .AddAttribute("Randomize",
                    "Type of send time randomization: none (default), uniform, exponential",
                    StringValue("none"),
                    MakeStringAccessor(&MrNormalUser::SetRandomize, &MrNormalUser::GetRandomize),
                    MakeStringChecker())

      .AddAttribute("MaxSeq", "Maximum sequence number to request",
                    IntegerValue(std::numeric_limits<uint32_t>::max()),
                    MakeIntegerAccessor(&MrNormalUser::m_seqMax), MakeIntegerChecker<uint32_t>());
                    
  return tid;
}

  
MrNormalUser::MrNormalUser()
  : m_frequency(1.0)
  , m_firstTime(true)
{
  NS_LOG_FUNCTION_NOARGS();
  m_seqMax = std::numeric_limits<uint32_t>::max();
  
  m_taskPrefix1 = "/";
  m_taskPrefix2 = "-";
}

MrNormalUser::~MrNormalUser()
{
}


std::string
MrNormalUser::GetRandomize() const
{
  return m_randomType;
}


void
MrNormalUser::SendPacket()
{
  if (!m_active)
    return;
  NS_LOG_FUNCTION_NOARGS();

    time_t now = std::time(0);
    struct tm tstruct;
    tstruct = *localtime(&now);
    char output[200];
    strftime(output, sizeof(output), "%Y-%M-%D.%X", &tstruct);
    //std::cout << "User starts at: " << output <<std::endl;
    std::ofstream recording;
    recording.open("normalUserTimeRecord.txt", std::ios_base::app);
    recording << "start: " << output << std::endl;
    recording.close();
    
    std::vector<std::string> producer66Names = {"7", "8", "11","12","13","14","15","16","17","18","19","20","21","22","23",
                                                "24","25","26","27","28","29","30","31","32","33","34","35","36","37",
                                                "39","40","41","42","43","44","45","46","47","48","49","50","51",
                                                "52","53","54","55","56","57","58","59","60","61","62","63","64","65",};
    
    
    /*                                           
    std::vector<std::string> producer39Names = {"7","8","11","12","13","14","15","16","17", "18","19","20","21","22","23",
                                                "24","25","26","27","28", "29","30","31","32","33","34","35","36","37",};   
     */
       
    /* 
    std::vector<std::string> producer93Names = {"7", "8", "11","12","13","14","15","16","17","18","19","20","21","22","23",
                                                "24","25","26","27","28","29","30","31","32","33","34","35","36","37",
                                                "39","40","41","42","43","44","45","46","47","48","49","50","51",
                                                "52","53","54","55","56","57","58","59","60","61","62","63","64","65",
                                                "66","67","68","69","70","71","72","73","74","75","76","77","78","79",
                                                "80","81","82","83","84","85","86","87","88","89","90","91","92",
                                                };
      */ 
                                                
                                                                                         
    m_sendNum = producer66Names.size();
    
    for(uint8_t i=0; i<producer66Names.size(); i++)
    {
      std::string taskString = m_taskPrefix1 + producer66Names[i] + m_taskPrefix2;
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


void
MrNormalUser::ScheduleNextPacket()
{
  // double mean = 8.0 * m_payloadSize / m_desiredRate.GetBitRate ();
  // std::cout << "next: " << Simulator::Now().ToDouble(Time::S) + mean << "s\n";

  if (m_firstTime) {
    m_sendEvent = Simulator::Schedule(Seconds(0.0), &MrNormalUser::SendPacket, this);
    m_firstTime = false;
  }
  else if (!m_sendEvent.IsRunning())
    m_sendEvent = Simulator::Schedule((m_random == 0) ? Seconds(1.0 / m_frequency)
                                                      : Seconds(m_random->GetValue()),
                                      &MrNormalUser::SendPacket, this);
}


void
MrNormalUser::OnInterest(shared_ptr<const Interest> interest)
{
  App::OnInterest(interest); // tracing inside

  NS_LOG_FUNCTION(this << interest);

  if (!m_active)
    return;
}


///////////////////////////////////////////////////
//          Process incoming packets             //
///////////////////////////////////////////////////

void
MrNormalUser::OnData(shared_ptr<const Data> data)
{
  if (!m_active)
    return;

  App::OnData(data); // tracing inside

  NS_LOG_FUNCTION(this << data);
  
    
   //parse data content
    auto *tmpContent = ((uint8_t*)data->getContent().value());
    std::string receivedData;
    for(uint8_t i=0; i < data->getContent().value_size(); i++) {
      receivedData.push_back((char)tmpContent[i]);
    };
    
    m_dataList.push_back(receivedData);

    if(m_dataList.size() == m_sendNum)
    {
      std::cout << "Get all Data " <<std::endl;
      time_t now = std::time(0);
      struct tm tstruct;
      tstruct = *localtime(&now);
      char output[200];
      strftime(output, sizeof(output), "%Y-%M-%D.%X", &tstruct);
      //std::cout << "User ends at: " << output <<std::endl;
      std::ofstream recording;
      recording.open("normalUserTimeRecord.txt", std::ios_base::app);
      recording <<"ends: " << output << std::endl;
      recording.close();
      m_dataList.clear();
    }
  

}


void
MrNormalUser::SetRandomize(const std::string& value)
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


} // namespace ndn
} // namespace ns3
