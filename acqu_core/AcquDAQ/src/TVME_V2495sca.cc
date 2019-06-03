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

#include "TVME_V2495sca.h"
#include "TDAQexperiment.h"

ClassImp(TVME_V2495sca)

enum { EV2495_AcqCtrl=200, EV2495_ChenX, EV2495_DwellTm, EV2495_SWTrig,
       EV2495_ReData };
static Map_t kCAENKeys[] = {
  {"AcqCtrl:",            EV2495_AcqCtrl},
  {"ChanEnable:",         EV2495_ChenX},
  {"DwellTime:",          EV2495_DwellTm},
  {"SWTrigger:",          EV2495_SWTrig},
  {"ResetData:",          EV2495_ReData},
  {NULL,                  -1}
};

// Internal register offsets
VMEreg_t V2495scareg[] = {
  {0x0000,      0x0,  'l', EV2495_NMEB-1},     // multi event buffer
  {0x1000,      0x0,  'l', 0},       // control reg
  {0x1004,      0x0,  'l', 0},       // control reg set
  {0x1008,      0x0,  'l', 0},       // control reg clear
  {0x100c,      0x0,  'l', 0},       // firmware revision
  {0x1010,      0x0,  'l', 0},       // status reg
  {0x1014,      0x0,  'l', 0},       // commands reg
  {0x1020,      0x0,  'l', 0},       // chan enable mask A
  {0x1024,      0x0,  'l', 0},       // chan enable mask B
  {0x1028,      0x0,  'l', 0},       // chan enable mask D
  {0x102c,      0x0,  'l', 0},       // chan enable mask E
  {0x1030,      0x0,  'l', 0},       // dwell time (internal trigger)
  {0x1034,      0x0,  'l', 0},       // chan enable mask F
  {0x1038,      0x0,  'l', 0},       // ID daughter board
  {0x1100,      0x0,  'l', EV2495_NCtr-1},      // read counters A
  {0x1180,      0x0,  'l', EV2495_NCtr-1},      // read counters B
  {0x1200,      0x0,  'l', EV2495_NCtr-1},      // read counters D
  {0x1280,      0x0,  'l', EV2495_NCtr-1},      // read counters E
  {0x1300,      0x0,  'l', EV2495_NCtr-1},      // read counters F
  {0xffffffff,  0x0,  'l', 0},       // end of list
};


//-----------------------------------------------------------------------------
TVME_V2495sca::TVME_V2495sca( Char_t* name, Char_t* file, FILE* log,
		      Char_t* line ):
  TVMEmodule( name, file, log, line )
{
  // Basic initialisation 
  fCtrl = NULL;                            // no DAQ control functions
  fType = EDAQ_Scaler;                     // Scaler
  fNreg = EV2495_COUNTERS_F + EV2495_NCtr; // Last "hard-wired" register
  fHardID = 0;                             // firmware from daq init
  fNBits = 32;                             // 32-bit scaler
  // Operating mode:740
  // V2495 format, 64bit time, 64bit data, no write channel mask, write time tag
  // G1=inhibit, no autoreset, G-ports NIM, no int. trig, no ext trig.
  // Operating mode:040
  // V1495 format, 32bit time, 32bit data, no write channel mask, write time tag
  // G1=inhibit, no autoreset, G-ports NIM, no int. trig, no ext trig.
  fAcqCtrl = 0x040;
  fIsV2495Format = false;       // default V1495 format
  fIsTimeTagg = true;           // default read time tagg
  fIs64bit = false;             // default 32 bit
  fIsSWTrig = false;            // default NOT software trigger
  fClr = 0x4;                   // default don't flush counter after read
  fDwellTm = 1;                            // 1 us
  fChEnX[0] = fChEnX[1] = 0xffffffff;      // section A,B on
  fChEnX[2] = fChEnX[3] = fChEnX[4] = 0;   // section D,E,F off
  AddCmdList( kCAENKeys );                 // CAEN-specific setup commands
}

//-----------------------------------------------------------------------------
TVME_V2495sca::~TVME_V2495sca( )
{
  // Clean up
}

//-----------------------------------------------------------------------------
void TVME_V2495sca::SetConfig( Char_t* line, Int_t key )
{
  // Configuration from file 
  switch( key ){
  case EV2495_AcqCtrl:
    // Specific setting of AcqCtrl reg...overrides default in constructor
    // Bit 0: external trig enable (G0 signal)
    // Bit 1: internal trig enable (frequency set by dwell time)
    // Bit 2: 0 G-port NIM, 1 G-port TTL
    // Bit 3: auto-reset (by trigger)
    // Bit 4,5: 00 G1 inhibit; 01 G1 is reset; 10 G1 is test
    // Bit 6: save time tagg
    // Bit 7: save channel mask in data (leave 0)
    // Bit 8: 64 bit counters
    // Bit 9: 64 bit time stamp
    // Bit 10: data saved in V2495 format (otherwise V1495 format)
    // Bit 11: board clock provided on the C output
    // Bit 12-14: 0 input to A395D in D,E,F is NIM; 1 input TTL
    //            only if these optional mezzanines are installed
    if( sscanf(line,"%x",&fAcqCtrl) != 1 ){
      PrintError(line,"<Parse AcqCtrl setting>",EErrFatal);
      break;
    }
    // Some checks for "sensible" options
    if(fAcqCtrl & 0x100)
      fIs64bit = true;         // 64-bit data enabled
    if(fAcqCtrl & 0x40){
      fIsTimeTagg = true;      // time tag recorded
      if(fIs64bit)
	fAcqCtrl |= 0x200;     // ensure 64-bit time if 64-bit counters
    }
    if(fAcqCtrl & 0x400)       // 2495 data format?
      fIsV2495Format = true;
    break;
  case EV2495_ChenX:
    // Channel enable masks...overrides default in constructor
    if( sscanf(line,"%x%x%x%x%x",fChEnX,fChEnX+1,fChEnX+2,fChEnX+3,fChEnX+4) != 5 ){
      PrintError(line,"<Parse Channel Masks>",EErrFatal);
      break;
    }
    // determine number of active channels from channel masks
    fNChannel = 0;
    for(Int_t i=0; i<5; i++){
      Int_t bit = 1;
      for(Int_t j=0; j<32; j++){
	if(bit & fChEnX[i])
	  fNChannel++;
	bit *= 2;
      }
    }
    // check for extras...assumes AcqCtrl is already finalised
    if(fAcqCtrl & 0x40){
      fNChannel += 1;          // add a channel for time stamp
      if(fAcqCtrl & 0x100){    // 64 bit doubles number of channels
	fAcqCtrl |= 0x200;     // ensure time stamp is also 64 bit 
	fNChannel *= 2;
      }
    }
    break;
  case EV2495_DwellTm:
    // Internal trigger dwell time (us)...overrides default in constructor
    if( sscanf(line,"%d",&fDwellTm) != 1 ){
      PrintError(line,"<Parse dwell time>", EErrFatal);
      break;
    }
    break;
  case EV2495_SWTrig:
    // Use the software trigger to transfer counters to memory
    // Ensure that external and internal triggers are off in this case
    fIsSWTrig = true;
    fAcqCtrl &= 0xfffc;  // ensure bits 0,1 are zero
    break;
  case EV2495_ReData:
    // Turn on clear counters after latch and read
    if( sscanf(line,"%x",&fClr) != 1 ){
      PrintError(line,"<Parse reset command value>", EErrFatal);
      break;
    }
    break;
  default:
    // default try commands of TVMEmodule and then TDAQmodule
    TVMEmodule::SetConfig(line, key);
    break;
  }
}

//---------------------------------------------------------------------------
void TVME_V2495sca::ReadIRQ( void** outBuffer )
{
  // Read and decode the memory buffer (MEB) of the V2495sca
  // Read V2495sca data buffer in 4-byte words
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
      ScalerStore( outBuffer, scVal, scIndex++ );
      nword--;
    }
    for(Int_t n=0; n<nword; n++){
      scVal = Read(i++);
      ScalerStore( outBuffer, scVal, scIndex++ );// store values
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
      ScalerStore( outBuffer, scVal, scIndex++ ); // store values
      nword--;
      if(fIs64bit){
	scVal = Read(i++);                          // 2nd 32 bits
	ScalerStore( outBuffer, scVal, scIndex++ ); // store values
	nword--;
      }
    }
    // Now read the scalers
    for( i=1; i<=nword; i++ ){
      scVal = Read(i++);
      ScalerStore( outBuffer, scVal, scIndex++ );// store values
    }
  }
  ResetData();
  return;
}

//-------------------------------------------------------------------------
void TVME_V2495sca::PostInit( )
{
  // Check if any general register initialisation has been performed
  // Setup data memory, threshold registers and status registers.
  // Check if ID read from internal ROM matches the expected value
  // If OK carry out default init to perform write initialisation of registers
  if( fIsInit ) return;            // Init already done?
  InitReg(V2495scareg);
  TVMEmodule::PostInit();
  //
  Reset();                               // reset board
                                         // this also resets time-stamp counter
  Write(EV2495_ACQ_CTRL, fAcqCtrl);      // load acquisiton control bits
  // unless mezzanine boards fitted, only A,B relevant
  Write(EV2495_CHEN_A,fChEnX[0]);        // chan enable A
  Write(EV2495_CHEN_B,fChEnX[1]);        // chan enable B
  Write(EV2495_CHEN_D,fChEnX[2]);        // chan enable D
  Write(EV2495_CHEN_E,fChEnX[3]);        // chan enable E
  Write(EV2495_CHEN_F,fChEnX[4]);        // chan enable F
  //
  return;
}

//-------------------------------------------------------------------------
Bool_t TVME_V2495sca::CheckHardID( )
{
  // Read module firmware register, check against expected number
  // and print frimware info
  Int_t id = Read(EV2495_FWREV);
  fprintf(fLogStream,"%s ID Read: %x  Expected: %x\n",GetName(),id,fHardID);
  fprintf(fLogStream,"Firmware version: %d.%d dated %d/%d/%d\n",
	  (id&0xff00)>>8,id&0xff,((id&0xf0000000)>>28)+2016,(id&0x0f000000)>>24,
	  (id&0x00ff0000)>>16);
  if( id == fHardID ) return kTRUE;
  else
    PrintError("","<CAEN V2495sca Scaler hardware ID read error>",EErrFatal);
  return kFALSE;
}
