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

#ifndef _WRITE_H_
#define _WRITE_H_

#include "nvmCmd.h"


class Write;    // forward definition
typedef boost::shared_ptr<Write>             SharedWritePtr;
typedef boost::shared_ptr<const Write>       ConstSharedWritePtr;
#define CAST_TO_WRITE(shared_trackable_ptr)  \
        boost::shared_polymorphic_downcast<Write>(shared_trackable_ptr);


/**
* This class implements the Write admin cmd
*
* @note This class may throw exceptions.
*/
class Write : public NVMCmd
{
public:
    Write(int fd);
    virtual ~Write();

    /// Used to compare for NULL pointers being returned by allocations
    static SharedWritePtr NullWritePtr;

    /**
     * Set the Starting Logical Block Address (SLBA).
     * @param lba Pass the LBA to start writing data
     */
    void     SetSLBA(uint64_t lba);
    uint64_t GetSLBA() const;

    /**
     * Set the Limited Retry (LR)
     * @param lr Pass true to set, otherwise false
     */
    void SetLR(bool lr);
    bool GetLR() const;

    /**
     * Set the Force Unit Access (FUA)
     * @param fua Pass true to set, otherwise false
     */
    void SetFUA(bool fua);
    bool GetFUA() const;

    /**
     * Set the Protection Information Field (PRINFO)
     * @param prinfo Pass any value which can be set in 4 bits
     */
    void    SetPRINFO(uint8_t prinfo);
    uint8_t GetPRINFO() const;

    /**
     * Set the Number of Logical Blocks (NLB)
     * @param nlb Pass the new value to set
     */
    void     SetNLB(uint16_t nlb);
    uint16_t GetNLB() const;

    /**
     * Set the Data Set Management (DSM) Incompressible
     * @param incompress Pass true to set, otherwise false
     */
    void SetDSMIncompress(bool incompress);
    bool GetDSMIncompress() const;

    /**
     * Set the Data Set Management (DSM) Sequential Request
     * @param seqReq Pass true to set, otherwise false
     */
    void SetDSMSeqRequest(bool seqReq);
    bool GetDSMSeqRequest() const;

    /**
     * Set the Data Set Management (DSM) Access Latency
     * @param accessLat Pass any value which can be set in 2 bits
     */
    void    SetDSMAccessLatent(uint8_t accessLat);
    uint8_t GetDSMAccessLatent() const;

    /**
     * Set the Data Set Management (DSM) Access Frequency
     * @param accessFreq Pass any value which can be set in 4 bits
     */
    void    SetDSMAccessFreq(uint8_t accessFreq);
    uint8_t GetDSMAccessFreq() const;

    /**
     * Set the Initial Logical Block Reference Tag (ILBRT)
     * @param ilbrt Pass the new value to set
     */
    void     SetILBRT(uint32_t ilbrt);
    uint32_t GetILBRT() const;

    /**
     * Set the Logcial Block Application Tag Mask (LBATM)
     * @param lbatm Pass the new value to set
     */
    void     SetLBATM(uint16_t lbatm);
    uint16_t GetLBATM() const;

    /**
     * Set the Logcial Block Application Tag (LBAT)
     * @param lbat Pass the new value to set
     */
    void     SetLBAT(uint16_t lbat);
    uint16_t GetLBAT() const;

    /**
     * Append the entire contents of this cmds' contents, any PRP payload,
     * and any meta data it may contain to the named file.
     * @param filename Pass the filename as generated by macro
     *      FileSystem::PrepLogFile().
     * @param fileHdr Pass a custom file header description to dump
     */
    void Dump(LogFilename filename, string fileHdr) const;


private:
    Write();
};


#endif