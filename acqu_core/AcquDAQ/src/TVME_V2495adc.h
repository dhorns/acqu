//--Author	JRM Annand  14th Feb 2019
//--Rev 	JRM Annand  11th Apr 2019 after 1st debug
//--Update 	JRM Annand
//--Description
//                *** AcquDAQ++ <-> Root ***
// DAQ for Sub-Atomic Physics Experiments.
//
// TVME_V2495adc
// CAEN VMEbus FPGA-based Scaler...read every event as ADC
// V2495 general purpose FPGA-based VMEbus module with CAEN scaler 
// firmware loaded. Can be configured both for 1495 (32-bit)
// and 2495 (32 or 64-bit) data readout.
// Time stamp can optionally be included in data stream
//

#ifndef __TVME_V2495adc_h__
#define __TVME_V2495adc_h__

#include "TVME_V2495sca.h"
//
class TVME_V2495adc : public TVME_V2495sca {
 protected:
  public:
  TVME_V2495adc( Char_t*, Char_t*, FILE*, Char_t* );
  virtual ~TVME_V2495adc();
  // virtual void SetConfig( Char_t*, Int_t );   // configure VME module
  virtual void PostInit( );
  virtual void ReadIRQ( void** );
  //virtual Bool_t CheckHardID();
    //

  ClassDef(TVME_V2495adc,1)   

    };

#endif

