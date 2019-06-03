//--Author	JRM Annand  12th Apr 2019
//--Rev 	JRM Annand
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

#include "TVME_V2495adc.h"
#include "TDAQexperiment.h"

ClassImp(TVME_V2495adc)

//-----------------------------------------------------------------------------
TVME_V2495adc::TVME_V2495adc( Char_t* name, Char_t* file, FILE* log,
		      Char_t* line ):
  TVME_V2495sca( name, file, log, line )
{
  // Basic initialisation where it differs from scaler
  fCtrl = NULL;                            // no DAQ control functions
  fType = EDAQ_ADC;                        // ADC
  fNBits = 16;                             // 16-bit "ADC"
}

//-----------------------------------------------------------------------------
TVME_V2495adc::~TVME_V2495adc( )
{
  // Clean up
}


//---------------------------------------------------------------------------
void TVME_V2495adc::ReadIRQ( void** outBuffer )
{
  // Read and decode the memory buffer (MEB) of the V2495adc
  // Read V2495adc data buffer in 4-byte words
  // If 64-bit option enabled time stamp and each counter is saved as 2 32-bit
  // chunks
  //
  UInt_t scVal, scIndex;                // scaler value and index
  Int_t nword, event;
  if(fIsSWTrig)
    SWTrig();
  Int_t i = EV2495_MEB;
  UInt_t datum = Read(i++);             // 1st data header word
  scIndex = fBaseIndex;
  //
  // V1495 original format decode
  if(!fIsV2495Format){
    nword = (datum & 0xff);             // # data words
    nword--;
    event = (datum & 0x3ffff00) >> 8;   // event number
    if(datum & 0x80000000){             // check for time stamp
      scVal = Read(i++);                // time stamp
      ADCStore( outBuffer, (scVal&0xffff), scIndex++ );
      ADCStore( outBuffer, (scVal&0xffff0000)>>16, scIndex++ );
      nword--;
    }
    for(Int_t n=0; n<nword; n++){
      scVal = Read(i++);
      ADCStore( outBuffer, (scVal&0xffff), scIndex++ );// store values
      ADCStore( outBuffer, (scVal&0xffff0000)>>16, scIndex++ );// store values
    }
  }
  // V2495 format decode
  // This will only work correctly if saving of channel-mask register
  // into the data stream is disabled
  else{
    // check if header says its V2495 format
    if(!(datum & 0x40000000)){
      return;
    }
    nword = (datum & 0xfff);            // # data words
    event = (datum & 0x3ffff000) >> 12; // event number
    datum = Read(i++);                  // data available
    nword--;
    if(fIsTimeTagg){
      scVal = Read(i++);                          // time stamp 1st 32 bits
      ADCStore( outBuffer, (scVal&0xffff), scIndex++ ); // store values
      ADCStore( outBuffer, (scVal&0xffff0000)>>16, scIndex++ ); // store values
      nword--;
      if(fIs64bit){
	scVal = Read(i++);                          // 2nd 32 bits
	ADCStore( outBuffer, (scVal&0xffff), scIndex++ ); // store values
	ADCStore( outBuffer, (scVal&0xffff0000)>>16, scIndex++ ); // store
	nword--;
      }
    }
    // Now read the scalers
    for( i=1; i<=nword; i++ ){
      scVal = Read(i++);
      ADCStore( outBuffer, (scVal&0xffff), scIndex++ );// store values
      ADCStore( outBuffer, (scVal&0xffff0000)>>16, scIndex++ );// store values
    }
  }
  if(CheckStatus() == false)
    ClrMEB();
  ResetData();
  return;
}

//-------------------------------------------------------------------------
void TVME_V2495adc::PostInit( )
{
  fNChannel *= 2;
  TVME_V2495sca::PostInit();
}
