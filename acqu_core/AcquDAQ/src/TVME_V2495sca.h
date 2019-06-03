//--Author	JRM Annand  14th Feb 2019
//--Rev 	JRM Annand  11th Apr 2019 after 1st debug
//--Update 	JRM Annand
//--Description
//                *** AcquDAQ++ <-> Root ***
// DAQ for Sub-Atomic Physics Experiments.
//
// TVME_V2495sca
// CAEN VMEbus FPGA-based Scaler
// V2495 general purpose FPGA-based VMEbus module with CAEN scaler 
// firmware loaded. Can be configured both for 1495 (32-bit)
// and 2495 (32 or 64-bit) data readout.
// Time stamp can optionally be included in data stream
//

#ifndef __TVME_V2495sca_h__
#define __TVME_V2495sca_h__

#include "TVMEmodule.h"

enum { 
  EV2495_NCtr = 32,           // no. channels per counter section
  EV2495_NMEB = 0x1000,       // event buffer size
};
// device register ID
enum {
  EV2495_MEB,
  EV2495_ACQ_CTRL = EV2495_MEB + EV2495_NMEB,
  EV2495_ACQ_CTRL_SET,
  EV2495_ACQ_CTRL_CLR,
  EV2495_FWREV,
  EV2495_STATUS,
  EV2495_COMMANDS,
  EV2495_CHEN_A,
  EV2495_CHEN_B,
  EV2495_CHEN_D,
  EV2495_CHEN_E,
  EV2495_DWELL_TIME,
  EV2495_CHEN_F,
  EV2495_DB_ID,
  EV2495_COUNTERS_A,
  EV2495_COUNTERS_B = EV2495_COUNTERS_A + EV2495_NCtr,
  EV2495_COUNTERS_D = EV2495_COUNTERS_B + EV2495_NCtr,
  EV2495_COUNTERS_E = EV2495_COUNTERS_D + EV2495_NCtr,
  EV2495_COUNTERS_F = EV2495_COUNTERS_E + EV2495_NCtr,
};
//
class TVME_V2495sca : public TVMEmodule {
 protected:
  Bool_t fIsMebFull;
  Bool_t fIsV2495Format;
  Bool_t fIsTimeTagg;
  Bool_t fIs64bit;
  Bool_t fIsSWTrig;
  UInt_t fAcqCtrl;              // acquisition control bits
  Int_t fStatus;                // status (MEB full?)
  Int_t fCmds;                  // commands bits
  UInt_t fDwellTm;              // int-trigger dwell time (micro sec)
  Int_t fChEnX[5];              // chan enable mask A,B,D,E,F
  UInt_t fClr;                  // clear command to apply after each read
  public:
  TVME_V2495sca( Char_t*, Char_t*, FILE*, Char_t* );
  virtual ~TVME_V2495sca();
  virtual void SetConfig( Char_t*, Int_t );   // configure VME module
  virtual void PostInit( );
  virtual void ReadIRQ( void** );
  virtual Bool_t CheckHardID();
    //
  virtual void ResetData(){ Write(EV2495_COMMANDS,fClr); }
  virtual void Reset(){ Write(EV2495_COMMANDS,0x18); }
  virtual void ClrMEB(){ Write(EV2495_COMMANDS,0x4); }
  virtual void SWTrig(){ Write(EV2495_COMMANDS,0x1); }
  virtual Bool_t CheckStatus(){
    UInt_t status = Read(EV2495_STATUS);    // status reg...MEB full?
    if( (status & 0x1)  ){                    // 0'th bit set?
      return false;                           // MEB full
    }
    return true;                              // MEB not full
  }
  ClassDef(TVME_V2495sca,1)   

    };

#endif

