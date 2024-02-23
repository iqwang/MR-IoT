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

#ifndef NDN_MRMAPPERTEST_H
#define NDN_MRMAPPERTEST_H

#include "ns3/ndnSIM/model/ndn-common.hpp"

#include "ndn-app.hpp"
#include "ns3/ndnSIM/model/ndn-common.hpp"

#include "ns3/nstime.h"
#include "ns3/ptr.h"

#include <deque>

#include "ns3/random-variable-stream.h"


namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-apps
 * @brief A simple Interest-sink applia simple Interest-sink application
 *
 * A simple Interest-sink applia simple Interest-sink application,
 * which replying every incoming Interest with Data packet with a specified
 * size and name same as in Interest.cation, which replying every incoming Interest
 * with Data packet with a specified size and name same as in Interest.
 */
class MrMapperTest : public App {
public:
  static TypeId
  GetTypeId(void);

  MrMapperTest();

  // inherited from NdnApp
  virtual void
  OnInterest(shared_ptr<const Interest> interest);
  
  virtual void
  OnData(shared_ptr<const Data> contentObject);
  

protected:
  // inherited from Application base class.
  virtual void
  StartApplication(); // Called at time specified by Start

  virtual void
  StopApplication(); // Called at time specified by Stop

private:
  Ptr<UniformRandomVariable> m_rand;
  Name m_prefix;
  Name m_postfix;
  uint32_t m_virtualPayloadSize;
  Time m_freshness;
  uint32_t m_signature;
  Name m_keyLocator;
  
  Name m_interestName;
  Name m_pendingInterestName;
  std::string m_askPitPrefix;
  std::map<std::string, std::string> m_disTreeInterestMap;
  std::string m_treeTag;
  int m_askPitNum =0;
  int m_gotPitNum =0;
  std::string m_askFibPrefix;
};

} // namespace ndn
} // namespace ns3

#endif // NDN_MRSENSORTEST_H
