/*
 * Copyright (c) 2011, Intel Corporation.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <boost/format.hpp>
#include "fidAsyncEventCfg_r10b.h"
#include "globals.h"
#include "grpDefs.h"
#include "../Queues/acq.h"
#include "../Queues/asq.h"
#include "../Utils/kernelAPI.h"
#include "../Utils/irq.h"
#include "../Utils/io.h"
#include "../Cmds/baseFeatures.h"
#include "../Cmds/getFeatures.h"
#include "../Cmds/setFeatures.h"

namespace GrpAdminSetGetFeatCombo {


FIDAsyncEventCfg_r10b::FIDAsyncEventCfg_r10b(
    string grpName, string testName) :
    Test(grpName, testName, SPECREV_10b)
{
    // 63 chars allowed:     xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    mTestDesc.SetCompliance("revision 1.0b, section 5");
    mTestDesc.SetShort(     "Verify changes are allowed on FID = Async write config");
    // No string size limit for the long description
    mTestDesc.SetLong(
        "Reset ctrlr to cause a clearing of DUT state. Issue GetFeatures, "
        "FID = 0x0B. Programmatically and dynamically force new, legal values "
        "into all fields of CE.DW0 and assoc changes with SetFeatures. Redo "
        "GetFeatures to verify settings were accepted.");

}


FIDAsyncEventCfg_r10b::~FIDAsyncEventCfg_r10b()
{
    ///////////////////////////////////////////////////////////////////////////
    // Allocations taken from the heap and not under the control of the
    // RsrcMngr need to be freed/deleted here.
    ///////////////////////////////////////////////////////////////////////////
}


FIDAsyncEventCfg_r10b::
FIDAsyncEventCfg_r10b(const FIDAsyncEventCfg_r10b &other) : Test(other)
{
    ///////////////////////////////////////////////////////////////////////////
    // All pointers in this object must be NULL, never allow shallow or deep
    // copies, see Test::Clone() header comment.
    ///////////////////////////////////////////////////////////////////////////
}


FIDAsyncEventCfg_r10b &
FIDAsyncEventCfg_r10b::operator=(const FIDAsyncEventCfg_r10b &other)
{
    ///////////////////////////////////////////////////////////////////////////
    // All pointers in this object must be NULL, never allow shallow or deep
    // copies, see Test::Clone() header comment.
    ///////////////////////////////////////////////////////////////////////////
    Test::operator=(other);
    return *this;
}


Test::RunType
FIDAsyncEventCfg_r10b::RunnableCoreTest(bool preserve)
{
    ///////////////////////////////////////////////////////////////////////////
    // All code contained herein must never permanently modify the state or
    // configuration of the DUT. Permanence is defined as state or configuration
    // changes that will not be restored after a cold hard reset.
    ///////////////////////////////////////////////////////////////////////////

    return ((preserve == true) ? RUN_FALSE : RUN_TRUE);   // Test is destructive
}


void
FIDAsyncEventCfg_r10b::RunCoreTest()
{
    /** \verbatim
     * Assumptions:
     * None.
     * \endverbatim
     */
    string work;
    union CE ce;
    struct nvme_gen_cq acqMetrics;

    if (gCtrlrConfig->SetState(ST_DISABLE_COMPLETELY) == false)
        throw FrmwkEx(HERE);

    LOG_NRM("Create admin queues ACQ and ASQ for test lifetime");
    SharedACQPtr acq = SharedACQPtr(new ACQ(gDutFd));
    acq->Init(5);

    SharedASQPtr asq = SharedASQPtr(new ASQ(gDutFd));
    asq->Init(5);

    // All queues will use identical IRQ vector
    IRQ::SetAnySchemeSpecifyNum(1);

    gCtrlrConfig->SetCSS(CtrlrConfig::CSS_NVM_CMDSET);
    if (gCtrlrConfig->SetState(ST_ENABLE) == false)
        throw FrmwkEx(HERE);

    LOG_NRM("Create Get features and set features cmds");
    SharedGetFeaturesPtr getFeaturesCmd =
        SharedGetFeaturesPtr(new GetFeatures());
    SharedSetFeaturesPtr setFeaturesCmd =
        SharedSetFeaturesPtr(new SetFeatures());

    LOG_NRM("Set and Get features for async event config (FID = 0x%x)",
        BaseFeatures::FID_ASYNC_EVENT_CONFIG);
    getFeaturesCmd->SetFID(BaseFeatures::FID_ASYNC_EVENT_CONFIG);
    setFeaturesCmd->SetFID(BaseFeatures::FID_ASYNC_EVENT_CONFIG);

    uint8_t asyncEventMismatch = 0;

    for (uint8_t asyncEvent = 0; asyncEvent <= 0x1F; asyncEvent++) {
        LOG_NRM("Set and Get features for async event config # %d", asyncEvent);
        setFeaturesCmd->SetAsyncEventConfigSMART(asyncEvent);

        work = str(boost::format("asyncEvent.%d") % (uint32_t)asyncEvent);
        IO::SendAndReapCmd(mGrpName, mTestName, CALC_TIMEOUT_ms(1),
            asq, acq, setFeaturesCmd, work, true);

        acqMetrics = acq->GetQMetrics();
        LOG_NRM("Issue get features cmd & check for asyncEvent = %d", asyncEvent);
        IO::SendAndReapCmd(mGrpName, mTestName, CALC_TIMEOUT_ms(1),
            asq, acq, getFeaturesCmd, work, false);

        ce = acq->PeekCE(acqMetrics.head_ptr);
        LOG_NRM("Async event config using Get Features = %d", ce.t.dw0);
        if (asyncEvent != (ce.t.dw0 & 0x1F)) {
            LOG_ERR("Async event config get feat does not match set feat"
                "(expected, rcvd) = (%d, %d)", asyncEvent, ce.t.dw0);
            asyncEventMismatch = 0xFF;
        }
    }

    if (asyncEventMismatch)
        throw FrmwkEx(HERE, "Async event config setting mismatched.");
}


}   // namespace
