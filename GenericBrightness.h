#ifndef __GENERICBRIGHTNESS__
#define __GENERICBRIGHTNESS__

#include <IOKit/IOService.h>
#include <IOKit/IOTimerEventSource.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>
#include "IODisplay.h"


class GenericBrightness : public IOService
{
    OSDeclareDefaultStructors(GenericBrightness)

    private:
        IOACPIPlatformDevice* BrightnessMethods;

    protected:
        SInt32 fCurrentBrightness;
        SInt32 fMinBrightness;
        SInt32 fMaxBrightness;
        SInt32 fLastBrightness;
        SInt32 brightnessLevels;
        OSDictionary* displayParams;
        IODisplay* display;
        OSArray* brightnessTable;

        IOWorkLoop* BTWorkLoop;
        IOTimerEventSource* BTPollTimer;

    public:
        virtual IOService* probe(IOService* provider, SInt32* score);
        virtual bool start(IOService* provider);
        virtual void stop(IOService* provider);
        virtual bool init(OSDictionary* properties);
        virtual void free(void);

        void brightnessCheck(void);
        IODisplay& getDisplay(void);
        OSDictionary& getDisplayParams(void);
        IOReturn setBrightness(const char* method, OSObject* param);
        IOReturn getACPIBrightnessLevels(void);
        UInt32 getValueFromArray(const OSArray& array, const UInt8& index);
};

#endif
