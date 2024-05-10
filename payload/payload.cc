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

#include "payload.h"
#include <unistd.h>

ImplPayload::ImplPayload(void) {
  // Start with default values
  lock_state = false;
  release_state = false;
}

ImplPayload::~ImplPayload() {}

/**
 * Returns the lock status of the release mechanism.
 */
bool ImplPayload::getLockStatus() { return lock_state; }
/**
 * Returns true if the payload has already been released (i.e., true after a
 * successful invocation of releasePayload())
 */
bool ImplPayload::hasReleased() { return release_state; }
/**
 * Locks the payload release mechanism.
 */
void ImplPayload::lockReleaseMechanism() { lock_state = true; }
/**
 * Release supplies. This will only be possible when the release mechanism is
 * unlocked.
 */
bool ImplPayload::releasePayload() {
  if (lock_state == false) {
    release_state = true;
  }
  return release_state;
}
/**
 * Unlocks the release mechanism.
 */
void ImplPayload::unlockReleaseMechanism() { lock_state = false; }
