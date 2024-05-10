/*
 * FALSA Model Problem
 * 
 * Copyright 2024 Carnegie Mellon University.
 * 
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED, AS
 * TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE
 * OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
 * MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
 * WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 * Licensed under a MIT (SEI)-style license, please see license.txt or contact
 * permission@sei.cmu.edu for full terms.
 * 
 * [DISTRIBUTION STATEMENT A] This material has been approved for public
 * release and unlimited distribution.  Please see Copyright notice for non-US
 * Government use and distribution.
 * 
 * This Software includes and/or makes use of Third-Party Software each subject
 * to its own license.
 * 
 * DM24-0251
 */

#ifndef IPAYLOAD_H_H
#define IPAYLOAD_H_H

#include "google/protobuf/empty.pb.h"
#include "google/protobuf/wrappers.pb.h"

class IPayload {

public:
  /**
   * Returns the lock status of the release mechanism.
   */
  virtual bool getLockStatus() = 0;
  /**
   * Returns true if the payload has already been released (i.e., true after a
   * successful invocation of releasePayload())
   */
  virtual bool hasReleased() = 0;
  /**
   * Locks the payload release mechanism.
   */
  virtual void lockReleaseMechanism() = 0;
  /**
   * Release supplies. This will only be possible when the release mechanism is
   * unlocked.
   */
  virtual bool releasePayload() = 0;
  /**
   * Unlocks the release mechanism.
   */
  virtual void unlockReleaseMechanism() = 0;
};

#endif
