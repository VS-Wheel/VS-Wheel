#include "stdint.h"
#include "USBJoystick.h"
#include "Inputs.h"

setConstantForceReport SetConstantForceReport;
setEffectReport SetEffectReport;
effectOperationReport EffectOperationReport;
pidDeviceControl PidDeviceControl;
pidStateReport PidStateReport;

HID_REPORT inputRep;
HID_REPORT ffbRep;
HID_REPORT pidRep;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Joystick Inputs
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool USBJoystick::update(int16_t x, int16_t y, uint32_t buttons, int8_t throttle, int8_t brake, int8_t clutch) {

   x_ = x;
   y_ = y;
   buttons_ = buttons;
   throttle_ = throttle;
   brake_ = brake;
   clutch_ = clutch;

   inputRep.data[0] = 0x01;   // Report ID 1
   inputRep.data[1] = throttle_;
   inputRep.data[2] = brake_;
   inputRep.data[3] = clutch_;
   inputRep.data[4] = x_ & 0xff;
   inputRep.data[5] = x_ >> 8;         
   inputRep.data[6] = y_ & 0xff;
   inputRep.data[7] = y_ >> 8;            
   inputRep.data[8] = buttons_ & 0xff;
   inputRep.data[9] = buttons_ >> 8;                                                                         
   inputRep.data[10] = buttons_ >> 16;
   inputRep.data[11] = buttons_ >> 24;

   inputRep.length = 12;

   return send(&inputRep);
}

bool USBJoystick::retrieveFFBData() {
   if(readNB(&ffbRep))
   {
      switch(ffbRep.data[0]) // report ID
      {
        case 1: // ID 1
          SetEffectReport.reportID = 1;
          SetEffectReport.effectBlockIndex = ffbRep.data[1];
          SetEffectReport.directionEnable = ffbRep.data[2];
          SetEffectReport.duration = ffbRep.data[3] + (ffbRep.data[4] << 8);
          SetEffectReport.triggerRepeatInterval = ffbRep.data[5] + (ffbRep.data[6] << 8);
          SetEffectReport.samplePeriod = ffbRep.data[7] + (ffbRep.data[8] << 8);
          SetEffectReport.gain = ffbRep.data[9];
          SetEffectReport.triggerButton = ffbRep.data[10];
          SetEffectReport.ordinal1 = ffbRep.data[12];
          SetEffectReport.ordinal2 = ffbRep.data[13];
        break;
        case 5: // ID 5
          SetConstantForceReport.reportID = 5;
          SetConstantForceReport.effectBlockIndex = ffbRep.data[1];
          SetConstantForceReport.magnitude = ffbRep.data[2] + (ffbRep.data[3] << 8);
        break;
        case 10: // ID 10
          EffectOperationReport.reportID = 10;
          EffectOperationReport.effectBlockIndex = ffbRep.data[1];
          EffectOperationReport.opEffect = ffbRep.data[2];
          EffectOperationReport.loopCount = ffbRep.data[3];
        break;
        case 12: // ID 12
          PidDeviceControl.reportID = 12;
          PidDeviceControl.DC = ffbRep.data[1];
        break;
      }
      responseToHOST(ffbRep.data[0]);
   }
   return true;
}

void USBJoystick::responseToHOST(uint8_t id)
{   
   PidStateReport.reportID = 2;
    switch(id)
    {
      case 1:
              PidStateReport.deviceCtrl &= 1;
              PidStateReport.deviceCtrl |= (uint8_t)(SetEffectReport.effectBlockIndex << 1);
      break;
      case 5:
              PidStateReport.deviceCtrl &= 1;
              PidStateReport.deviceCtrl |= (uint8_t)(SetConstantForceReport.effectBlockIndex << 1);
      break;
      case 10:
              if(EffectOperationReport.opEffect == 3)
              {
                PidStateReport.deviceCtrl &= 254;
              }
              else
              {
                PidStateReport.deviceCtrl |= 1;
              }
              PidStateReport.deviceCtrl &= 1;
              PidStateReport.deviceCtrl |= (uint8_t)(EffectOperationReport.effectBlockIndex << 1);
      break;
      case 12:
              switch(PidDeviceControl.DC)
              {
                 case 1: // Enable Actuators
                  PidStateReport.stateReport |= 2;
                 break;
                 case 2: // Disable actuators
                  PidStateReport.stateReport &= 253;
                 break;
                 case 3: // Stop All Effects
                  PidStateReport.deviceCtrl &= 254;
                 break;
                 case 4: // Device Reset
                  PidStateReport.stateReport = 0;
                  PidStateReport.deviceCtrl = 0;
                 break;
                 case 5: // Device Pause
                  PidStateReport.stateReport |= 1;
                 break;
                 case 6: // Device continue
                  PidStateReport.stateReport &= 254;
                 break;
              }
      break;
    }
    pidRep.data[0] = PidStateReport.reportID;
    pidRep.data[1] = PidStateReport.stateReport;
    pidRep.data[2] = PidStateReport.deviceCtrl;
    pidRep.length = 3;
    send(&pidRep);
}

int16_t USBJoystick::get_magnitude(void)
{
    return (int16_t)SetConstantForceReport.magnitude;
}

bool USBJoystick::get_effectPlaying(void)
{
    return (bool)(PidStateReport.deviceCtrl & 1);
}

void USBJoystick::_init() {

   throttle_ = -127;
   brake_ = -127;
   clutch_ = -127;   
   x_ = 0;                       
   y_ = 0;     
   buttons_ = 0x00000000;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Report Descriptor

uint8_t * USBJoystick::reportDesc() {    
         static uint8_t reportDescriptor[] = {

 0x05,0x01, // Usage Page (Generic Desktop)
  0x15,0x00, // Logical Minimum (0)
  0x09,0x04, // Usage (Joystick)
  0xA1,0x01, // Collection (Application)
    0x05,0x02, // Usage Page (Simulation Controls)
    0x85,0x01, // Report ID (1)
    0x09,0xBB, // Usage (Throttle)
    0x09,0xC5, // Usage (Brake)
    //0x09,0xC6, // Usage (Clutch)
    0x15,0x81, // Logical Minimum (-127)
    0x25,0x7F, // Logical Maximum (127)
    0x75,0x08, // Report Size (8)
    0x95,0x02, // Report Count (2) - Because (Throttle, Brake, Clutch)
    0x81,0x02, // Input (Data, Variable, Absolute)
    0x05,0x01, // Usage Page (Generic Desktop)
    0x09,0x01, // Usage (Pointer)
    0xA1,0x00, // Collection (Physical)
      0x09,0x32, // Usage (Z)
      0x15,0x81, // Logical Minimum (-127)
      0x25,0x7F, // Logical Maximum (127)
      0x75,0x08, // Report Size (8)
      0x95,0x01, // Report Count (1)
      0x81,0x02, // Input (Data, Variable, Absolute)
    0xC0, // End Collection
    0xA1,0x00, // Collection (Physical)
      0x09,0x30, // Usage (X)
      0x09,0x31, // Usage (Y)
      0x15,0x00, // Logical Minimum (0)
      0x26,0xFF,0x7F, // Logical Maximum (32767)
      0x75,0x10, // Report Size (10)
      0x95,0x02, // Report Count (2)
      0x81,0x02, // Input (Data, Variable, Absolute)
    0xC0, // End Collection
    0x05,0x09, // Usage Page (Button)
    0x19,0x01, // Usage Minimum (1), Because 0x00 - Nothing pressed
    0x29,0x21, // Usage Maximum (33)
    0x15,0x00, // Logical Minimum (0)
    0x25,0x01, // Logical Maximum (1)
    0x75,0x01, // Report Size (1)
    0x95,0x20, // Report Count (32)
    0x55,0x00, // Unit Exponent (0)
    0x65,0x00, // Unit (None)
    0x81,0x02, // Input (Data, Variable, Absolute)
    
  // Force feedback section taken from vJoy by Shaul Eizikovich (http://sourceforge.net/p/vjoystick/code/HEAD/tree/tags/2.1.5/RC5-050115/driver/sys/hidReportDescSingle.h)
  /*********************** Force Feedback section Device 1 [Start] ***********************/
  /*
    Input
    Collection Datalink (sub-collection)
    Physical Interface (Usage: PID State report)
    ID: 2
    state report:   5X1bit
    Padding:      3bit
    PID Device Control: 1bit
    Effect Block Index: 7bit  
  */
    0x05,0x0F,        //    Usage Page Physical Interface
    0x09,0x92,        //    Usage PID State report 
    0xA1,0x02,        //    Collection Datalink (logical)
       0x85,0x02,    //    Report ID 2
       0x09,0x9F,    //    Usage Device is Pause 
       0x09,0xA0,    //    Usage Actuators Enabled
       0x09,0xA4,    //    Usage Safety Switch
       0x09,0xA5,    //    Usage Actuator Override Switch
       0x09,0xA6,    //    Usage Actuator Power
       0x15,0x00,    //    Logical Minimum 0
       0x25,0x01,    //    Logical Maximum 1
       0x35,0x00,    //    Physical Minimum 0
       0x45,0x01,    //    Physical Maximum 1
       0x75,0x01,    //    Report Size 1
       0x95,0x05,    //    Report Count 5
       0x81,0x02,    //    Input (Variable)
       0x95,0x03,    //    Report Count 3
       0x81,0x03,    //    Input (Constant, Variable)
       0x09,0x94,    //    Usage Effect Playing
       0x15,0x00,    //    Logical Minimum 0
       0x25,0x01,    //    Logical Maximum 1
       0x35,0x00,    //    Physical Minimum 0
       0x45,0x01,    //    Physical Maximum 1
       0x75,0x01,    //    Report Size 1
       0x95,0x01,    //    Report Count 1
       0x81,0x02,    //    Input (Variable)
       0x09,0x22,    //    Usage Effect Block Index
       0x15,0x01,    //    Logical Minimum 1
       0x25,0x28,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x28,    //    Physical Maximum 28h (40d)
       0x75,0x07,    //    Report Size 7
       0x95,0x01,    //    Report Count 1
       0x81,0x02,    //    Input (Variable)
    0xC0    ,    // End Collection
  
  /*
    Output
    Collection  Datalink:
    Usage Set Effect Report
    
    ID:1
    Effect Block Index: 8bit
    
    subcollection Effect Type
    12 effect types, 8bit each
    
  */
    0x09,0x21,    //    Usage Set Effect Report
    0xA1,0x02,    //    Collection Datalink (Logical)
       0x85,0x01,    //    Report ID 1
       0x09,0x22,    //    Usage Effect Block Index
       0x15,0x01,    //    Logical Minimum 1
       0x25,0x28,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x28,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x09,0x25,    //    Usage Effect Type
       0xA1,0x02,    //    Collection Datalink
          0x09,0x26,    //    Usage ET Constant Force
          0x09,0x27,    //    Usage ET Ramp
          0x09,0x30,    //    Usage ET Square
          0x09,0x31,    //    Usage ET Sine
          0x09,0x32,    //    Usage ET Triangle
          0x09,0x33,    //    Usage ET Sawtooth Up
          0x09,0x34,    //    Usage ET Sawtooth Down
          0x09,0x40,    //    Usage ET Spring
          0x09,0x41,    //    Usage ET Damper
          0x09,0x42,    //    Usage ET Inertia
          0x09,0x43,    //    Usage ET Friction
          0x09,0x28,    //    Usage ET Custom Force Data
          0x25,0x0C,    //    Logical Maximum Ch (12d)
          0x15,0x01,    //    Logical Minimum 1
          0x35,0x01,    //    Physical Minimum 1
          0x45,0x0C,    //    Physical Maximum Ch (12d)
          0x75,0x08,    //    Report Size 8
          0x95,0x01,    //    Report Count 1
          0x91,0x00,    //    Output
       0xC0    ,          //    End Collection
       0x09,0x50,         //    Usage Duration
       0x09,0x54,         //    Usage Trigger Repeat Interval
       0x09,0x51,         //    Usage Sample Period
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x7F,    //    Logical Maximum 7FFFh (32767d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x7F,    //    Physical Maximum 7FFFh (32767d)
       0x66,0x03,0x10,    //    Unit 1003h (4099d)
       0x55,0xFD,         //    Unit Exponent FDh (253d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x03,         //    Report Count 3
       0x91,0x02,         //    Output (Variable)
       0x55,0x00,         //    Unit Exponent 0
       0x66,0x00,0x00,    //    Unit 0
       0x09,0x52,         //    Usage Gain
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x53,         //    Usage Trigger Button
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x08,         //    Logical Maximum 8
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x08,         //    Physical Maximum 8
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)

       0x09,0x55,       //    Usage Axes Enable
       0xA1,0x02,       //    Collection Datalink
          0x05,0x01,    //    Usage Page Generic Desktop
          0x09,0x30,    //    Usage X
          0x09,0x31,    //    Usage Y
          0x15,0x00,    //    Logical Minimum 0
          0x25,0x01,    //    Logical Maximum 1
          0x75,0x01,    //    Report Size 1
          0x95,0x02,    //    Report Count 2
          0x91,0x02,    //    Output (Variable)
       0xC0     ,    // End Collection

       0x05,0x0F,    //    Usage Page Physical Interface
       0x09,0x56,    //    Usage Direction Enable
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x95,0x05,    //    Report Count 5
       0x91,0x03,    //    Output (Constant, Variable)

       0x09,0x57,    //    Usage Direction
       0xA1,0x02,    //    Collection Datalink
          0x0B,0x01,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 1
          0x0B,0x02,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 2
          0x66,0x14,0x00,              //    Unit 14h (20d)
          0x55,0xFE,                   //    Unit Exponent FEh (254d)
          0x15,0x00,                   //    Logical Minimum 0
          0x26,0xFF,0x00,              //    Logical Maximum FFh (255d)
          0x35,0x00,                   //    Physical Minimum 0
          0x47,0xA0,0x8C,0x00,0x00,    //    Physical Maximum 8CA0h (36000d)
          0x66,0x00,0x00,              //    Unit 0
          0x75,0x08,                   //    Report Size 8
          0x95,0x02,                   //    Report Count 2
          0x91,0x02,                   //    Output (Variable)
          0x55,0x00,                   //    Unit Exponent 0
          0x66,0x00,0x00,              //    Unit 0
       0xC0,                           //    End Collection

       0x05, 0x0F,        //     USAGE_PAGE (Physical Interface)
       0x09, 0x58,        //     USAGE (Type Specific Block Offset)
       0xA1, 0x02,        //     COLLECTION (Logical) 
          0x0B, 0x01, 0x00, 0x0A, 0x00, //USAGE (Ordinals:Instance 1
          0x0B, 0x02, 0x00, 0x0A, 0x00, //USAGE (Ordinals:Instance 2)
          0x26, 0xFD, 0x7F, //   LOGICAL_MAXIMUM (32765) ; 32K RAM or ROM max.
          0x75, 0x10,     //     REPORT_SIZE (16)
          0x95, 0x02,     //     REPORT_COUNT (2)
          0x91, 0x02,     //     OUTPUT (Data,Var,Abs)
       0xC0,              //     END_COLLECTION
    0xC0,                 //     END_COLLECTION

  // Envelope Report Definition
    0x09,0x5A,    //    Usage Set Envelope Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x02,         //    Report ID 2
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x28,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x28,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x5B,         //    Usage Attack Level
       0x09,0x5D,         //    Usage Fade Level
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
       0x09,0x5C,         //    Usage Attack Time
       0x09,0x5E,         //    Usage Fade Time
       0x66,0x03,0x10,    //    Unit 1003h (4099d)
       0x55,0xFD,         //    Unit Exponent FDh (253d)
       0x26,0xFF,0x7F,    //    Logical Maximum 7FFFh (32767d)
       0x46,0xFF,0x7F,    //    Physical Maximum 7FFFh (32767d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x91,0x02,         //    Output (Variable)
       0x45,0x00,         //    Physical Maximum 0
       0x66,0x00,0x00,    //    Unit 0
       0x55,0x00,         //    Unit Exponent 0
    0xC0     ,            //    End Collection
    0x09,0x5F,    //    Usage Set Condition Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x03,    //    Report ID 3
       0x09,0x22,    //    Usage Effect Block Index
       0x15,0x01,    //    Logical Minimum 1
       0x25,0x28,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x28,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x09,0x23,    //    Usage Parameter Block Offset
       0x15,0x00,    //    Logical Minimum 0
       0x25,0x03,    //    Logical Maximum 3
       0x35,0x00,    //    Physical Minimum 0
       0x45,0x03,    //    Physical Maximum 3
       0x75,0x04,    //    Report Size 4
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x09,0x58,    //    Usage Type Specific Block Off...
       0xA1,0x02,    //    Collection Datalink
          0x0B,0x01,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 1
          0x0B,0x02,0x00,0x0A,0x00,    //    Usage Ordinals: Instance 2
          0x75,0x02,                   //    Report Size 2
          0x95,0x02,                   //    Report Count 2
          0x91,0x02,                   //    Output (Variable)
       0xC0     ,         //    End Collection
       0x15,0x80,         //    Logical Minimum 80h (-128d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x09,0x60,         //    Usage CP Offset
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x09,0x61,         //    Usage Positive Coefficient
       0x09,0x62,         //    Usage Negative Coefficient
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x09,0x63,         //    Usage Positive Saturation
       0x09,0x64,         //    Usage Negative Saturation
       0x75,0x08,         //    Report Size 8
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
       0x09,0x65,         //    Usage Dead Band
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
    0xC0     ,    //    End Collection
    0x09,0x6E,    //    Usage Set Periodic Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x04,                   //    Report ID 4
       0x09,0x22,                   //    Usage Effect Block Index
       0x15,0x01,                   //    Logical Minimum 1
       0x25,0x28,                   //    Logical Maximum 28h (40d)
       0x35,0x01,                   //    Physical Minimum 1
       0x45,0x28,                   //    Physical Maximum 28h (40d)
       0x75,0x08,                   //    Report Size 8
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x09,0x70,                   //    Usage Magnitude
       0x15,0x00,                   //    Logical Minimum 0
       0x26,0xFF,0x00,              //    Logical Maximum FFh (255d)
       0x35,0x00,                   //    Physical Minimum 0
       0x46,0x10,0x27,              //    Physical Maximum 2710h (10000d)
       0x75,0x08,                   //    Report Size 8
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x09,0x6F,                   //    Usage Offset
       0x15,0x80,                   //    Logical Minimum 80h (-128d)
       0x25,0x7F,                   //    Logical Maximum 7Fh (127d)
       0x36,0xF0,0xD8,              //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,              //    Physical Maximum 2710h (10000d)
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x09,0x71,                   //    Usage Phase
       0x66,0x14,0x00,              //    Unit 14h (20d)
       0x55,0xFE,                   //    Unit Exponent FEh (254d)
       0x15,0x00,                   //    Logical Minimum 0
       0x26,0xFF,0x00,              //    Logical Maximum FFh (255d)
       0x35,0x00,                   //    Physical Minimum 0
       0x47,0xA0,0x8C,0x00,0x00,    //    Physical Maximum 8CA0h (36000d)
       0x91,0x02,                   //    Output (Variable)
       0x09,0x72,                   //    Usage Period
       0x26,0xFF,0x7F,              //    Logical Maximum 7FFFh (32767d)
       0x46,0xFF,0x7F,              //    Physical Maximum 7FFFh (32767d)
       0x66,0x03,0x10,              //    Unit 1003h (4099d)
       0x55,0xFD,                   //    Unit Exponent FDh (253d)
       0x75,0x10,                   //    Report Size 10h (16d)
       0x95,0x01,                   //    Report Count 1
       0x91,0x02,                   //    Output (Variable)
       0x66,0x00,0x00,              //    Unit 0
       0x55,0x00,                   //    Unit Exponent 0
    0xC0     ,    // End Collection
    0x09,0x73,    //    Usage Set Constant Force Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x05,         //    Report ID 5
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x28,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x28,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x70,         //    Usage Magnitude
       0x16,0x01,0xFF,    //    Logical Minimum FF01h (-255d)
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
    0xC0     ,    //    End Collection
    0x09,0x74,    //    Usage Set Ramp Force Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x06,         //    Report ID 6
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x28,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x28,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x75,         //    Usage Ramp Start
       0x09,0x76,         //    Usage Ramp End
       0x15,0x80,         //    Logical Minimum 80h (-128d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x36,0xF0,0xD8,    //    Physical Minimum D8F0h (-10000d)
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x08,         //    Report Size 8
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
    0xC0     ,    //    End Collection
    0x09,0x68,    //    Usage Custom Force Data Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x07,         //    Report ID 7
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x28,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x28,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x6C,         //    Usage Custom Force Data Offset
       0x15,0x00,         //    Logical Minimum 0
       0x26,0x10,0x27,    //    Logical Maximum 2710h (10000d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x69,         //    Usage Custom Force Data
       0x15,0x81,         //    Logical Minimum 81h (-127d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x75,0x08,         //    Report Size 8
       0x95,0x0C,         //    Report Count Ch (12d)
       0x92,0x02,0x01,    //       Output (Variable, Buffered)
    0xC0     ,    //    End Collection
    0x09,0x66,    //    Usage Download Force Sample
    0xA1,0x02,    //    Collection Datalink
       0x85,0x08,         //    Report ID 8
       0x05,0x01,         //    Usage Page Generic Desktop
       0x09,0x30,         //    Usage X
       0x09,0x31,         //    Usage Y
       0x15,0x81,         //    Logical Minimum 81h (-127d)
       0x25,0x7F,         //    Logical Maximum 7Fh (127d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x75,0x08,         //    Report Size 8
       0x95,0x02,         //    Report Count 2
       0x91,0x02,         //    Output (Variable)
    0xC0     ,   //    End Collection
    0x05,0x0F,   //    Usage Page Physical Interface
    0x09,0x77,   //    Usage Effect Operation Report
    0xA1,0x02,   //    Collection Datalink
       0x85,0x0A,    //    Report ID Ah (10d)
       0x09,0x22,    //    Usage Effect Block Index
       0x15,0x01,    //    Logical Minimum 1
       0x25,0x28,    //    Logical Maximum 28h (40d)
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x28,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
       0x09,0x78,    //    Usage Effect Operation
       0xA1,0x02,    //    Collection Datalink
          0x09,0x79,    //    Usage Op Effect Start
          0x09,0x7A,    //    Usage Op Effect Start Solo
          0x09,0x7B,    //    Usage Op Effect Stop
          0x15,0x01,    //    Logical Minimum 1
          0x25,0x03,    //    Logical Maximum 3
          0x75,0x08,    //    Report Size 8
          0x95,0x01,    //    Report Count 1
          0x91,0x00,    //    Output
       0xC0     ,         //    End Collection
       0x09,0x7C,         //    Usage Loop Count
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x91,0x02,         //    Output (Variable)
    0xC0     ,    //    End Collection
    0x09,0x90,    //    Usage PID Block Free Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x0B,    //    Report ID Bh (11d)
       0x09,0x22,    //    Usage Effect Block Index
       0x25,0x28,    //    Logical Maximum 28h (40d)
       0x15,0x01,    //    Logical Minimum 1
       0x35,0x01,    //    Physical Minimum 1
       0x45,0x28,    //    Physical Maximum 28h (40d)
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x02,    //    Output (Variable)
    0xC0     ,    //    End Collection
    0x09,0x96,    //    Usage PID Device Control
    0xA1,0x02,    //    Collection Datalink
       0x85,0x0C,    //    Report ID Ch (12d)
       0x09,0x97,    //    Usage DC Enable Actuators
       0x09,0x98,    //    Usage DC Disable Actuators
       0x09,0x99,    //    Usage DC Stop All Effects
       0x09,0x9A,    //    Usage DC Device Reset
       0x09,0x9B,    //    Usage DC Device Pause
       0x09,0x9C,    //    Usage DC Device Continue
       0x15,0x01,    //    Logical Minimum 1
       0x25,0x06,    //    Logical Maximum 6
       0x75,0x08,    //    Report Size 8
       0x95,0x01,    //    Report Count 1
       0x91,0x00,    //    Output
    0xC0     ,    //    End Collection
    0x09,0x7D,    //    Usage Device Gain Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x0D,         //    Report ID Dh (13d)
       0x09,0x7E,         //    Usage Device Gain
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0x10,0x27,    //    Physical Maximum 2710h (10000d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
    0xC0     ,            //    End Collection
    0x09,0x6B,    //    Usage Set Custom Force Report
    0xA1,0x02,    //    Collection Datalink
       0x85,0x0E,         //    Report ID Eh (14d)
       0x09,0x22,         //    Usage Effect Block Index
       0x15,0x01,         //    Logical Minimum 1
       0x25,0x28,         //    Logical Maximum 28h (40d)
       0x35,0x01,         //    Physical Minimum 1
       0x45,0x28,         //    Physical Maximum 28h (40d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x6D,         //    Usage Sample Count
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x00,    //    Logical Maximum FFh (255d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x00,    //    Physical Maximum FFh (255d)
       0x75,0x08,         //    Report Size 8
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x09,0x51,         //    Usage Sample Period
       0x66,0x03,0x10,    //    Unit 1003h (4099d)
       0x55,0xFD,         //    Unit Exponent FDh (253d)
       0x15,0x00,         //    Logical Minimum 0
       0x26,0xFF,0x7F,    //    Logical Maximum 7FFFh (32767d)
       0x35,0x00,         //    Physical Minimum 0
       0x46,0xFF,0x7F,    //    Physical Maximum 7FFFh (32767d)
       0x75,0x10,         //    Report Size 10h (16d)
       0x95,0x01,         //    Report Count 1
       0x91,0x02,         //    Output (Variable)
       0x55,0x00,         //    Unit Exponent 0
       0x66,0x00,0x00,    //    Unit 0
    0xC0,    //    End Collection
  0xC0    //    End Collection
        };
      reportLength = sizeof(reportDescriptor);
      return reportDescriptor;
}