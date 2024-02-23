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

#ifndef NDN_MR_USER_H
#define NDN_MR_USER_H

#include "ns3/ndnSIM/model/ndn-common.hpp"

#include "ndn-consumer.hpp"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-apps
 * @brief Ndn application for sending out Interest packets at a "constant" rate (Poisson process)
 */
class MrUser : public Consumer {
public:
  static TypeId
  GetTypeId();

  /**
   * \brief Default constructor
   * Sets up randomizer function and packet sequence number
   */
  MrUser();
  virtual ~MrUser();
  
  virtual void
  OnData(shared_ptr<const Data> contentObject);
 
 virtual void
  OnInterest(shared_ptr<const Interest> interest);

protected:
  /**
   * \brief Constructs the Interest packet and sends it using a callback to the underlying NDN
   * protocol
   */
  virtual void
  ScheduleNextPacket();

  virtual void
  SendPacket();

	
	

  /**
   * @brief Set type of frequency randomization
   * @param value Either 'none', 'uniform', or 'exponential'
   */
  void
  SetRandomize(const std::string& value);

  /**
   * @brief Get type of frequency randomization
   * @returns either 'none', 'uniform', or 'exponential'
   */
  std::string
  GetRandomize() const;

protected:
  double m_frequency; // Frequency of interest packets (in hertz)
  bool m_firstTime;
  Ptr<RandomVariableStream> m_random;
  std::string m_randomType;
  
  std::string m_taskContent;
  std::string m_disNeiPrefix;
  std::vector<std::string> m_oneHopNeighbours;
  std::vector<std::string> m_sendJobNeis;
  bool m_buildTaskNeighbour;
  std::map<std::string, std::string> m_checkNeibMap;
  int m_sendDisNeiNum=0;
  int m_gotDisNeiNum=0;
  std::string m_ownPrefix;
  std::string m_disDownStream1;
  std::string m_disDownStream2;
  std::string m_disDownStream3;
  int m_sendDisDownNeiNum=0;
  int m_gotDisDownNeiNum=0;
  std::map<std::string, std::string> m_downNeiMap;
  std::string m_userTimeRecord;
  
};

} // namespace ndn
} // namespace ns3

#endif
