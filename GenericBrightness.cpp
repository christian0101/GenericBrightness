#include "GenericBrightness.h"

#define super IOService

OSDefineMetaClassAndStructors(GenericBrightness, IOService)


IOService* GenericBrightness::probe(IOService* provider, SInt32* score)
{
    if (!super::probe(provider, score))
    {
        return 0;
    }
    
	return this;
}

bool GenericBrightness::start(IOService* provider)
{	
	
	if(!provider || !super::start(provider))
    {
		return false;
    }
	
	BrightnessMethods = (IOACPIPlatformDevice *) provider;

	provider->joinPMtree(this);
	this->registerService(0);


	if (getACPIBrightnessLevels() == kIOReturnUnsupported)
    {
        return false;
    }
    
	IOLog("ACPI brightness levels:%d, lowest brightness:%d, highest brightness:%d \n ",
          brightnessLevels - 2, getValueFromArray(*brightnessTable, 2), getValueFromArray(*brightnessTable, brightnessLevels - 1));
		
	BTWorkLoop = getWorkLoop();
	BTPollTimer = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &GenericBrightness::brightnessCheck));
    
	if (!BTWorkLoop || !BTPollTimer || (kIOReturnSuccess != BTWorkLoop->addEventSource(BTPollTimer))) 
	{ 
		IOLog("Timer not working\n");
        return false;
	}
	
	brightnessCheck(); 
	return true;	
}

OSDictionary& GenericBrightness::getDisplayParams(void)
{
    OSDictionary* params = NULL;
    
    if(display && (display->getProperty("IODisplayGUID") != 0))
    {
        params = OSDynamicCast(OSDictionary, display->getProperty("IODisplayParameters"));
    }
    
    return *params;
}

IODisplay& GenericBrightness::getDisplay(void)
{
    IODisplay* displayRes = NULL;
    OSIterator* displayList = NULL;
    displayList = getMatchingServices(serviceMatching("IOBacklightDisplay"));
    
    if (displayList)
    {
        IOService *obj = NULL;

        while((obj = (IOService *) displayList->getNextObject()))
        {
            displayRes = OSDynamicCast(IOBacklightDisplay, obj);
        }

        displayList->release();
        displayList = NULL;
    }
    
    return *displayRes;
}

void GenericBrightness::brightnessCheck(void)
{
    if (!displayParams || !display || display != &getDisplay() || &getDisplayParams() != displayParams
        || !IODisplay::getIntegerRange(displayParams, gIODisplayBrightnessKey, &fCurrentBrightness, &fMinBrightness, &fMaxBrightness))
	{
		// IOLog("We still don't have brightness entry in ioreg... waiting...\n");
        display = &getDisplay();
        if(display)
        {
            displayParams = &getDisplayParams();
 		}
        
		BTPollTimer->setTimeoutMS(100);
	}
	else
    {
		BTPollTimer->setTimeoutMS(10);
	}
	
	if (fLastBrightness != fCurrentBrightness)
	{
		fLastBrightness = fCurrentBrightness;
        
		OSObject* param = OSNumber::withNumber(getValueFromArray(
            *brightnessTable, ((fCurrentBrightness * (brightnessLevels - 3)) / fMaxBrightness) + 2), 8);

		setBrightness("_BCM", param);
	}				
}

IOReturn GenericBrightness::setBrightness(const char* method, OSObject* param)
{
	OSObject* acpi;
    
	if (BrightnessMethods->evaluateObject(method, &acpi, &param, 1) != kIOReturnSuccess)
	{
		IOLog("%s: No object of method %s\n", this->getName(), method);
        return kIOReturnError;
	}
    
	return kIOReturnSuccess;
}

IOReturn GenericBrightness::getACPIBrightnessLevels(void)
{
	OSObject* brightnessLevelsACPI;
	
	if (BrightnessMethods->evaluateObject("_BCL", &brightnessLevelsACPI) == kIOReturnSuccess)
    {
		brightnessTable = OSDynamicCast(OSArray, brightnessLevelsACPI);
		brightnessLevels = brightnessTable->getCount();
        return kIOReturnSuccess;
	}

    brightnessLevels = 0;
    return kIOReturnUnsupported;
}

UInt32 GenericBrightness::getValueFromArray(const OSArray& array, const UInt8& index)
{
	OSObject* object = array.getObject(index);
    
	if (object && (OSTypeIDInst(object) == OSTypeID(OSNumber)))
    {
		OSNumber * number = OSDynamicCast(OSNumber, object);
		if (number)
        {
            return number->unsigned32BitValue();
        }
	}
    
	return -1;
}


bool GenericBrightness::init(OSDictionary* properties)
{    
	return super::init(properties);
}

void GenericBrightness::stop(IOService* provider)
{	
	if(BTPollTimer)
    {
		BTWorkLoop->removeEventSource(BTPollTimer);
		BTPollTimer->release();
	}	

	if( BTWorkLoop )
    {
		BTWorkLoop->release();
	}
	
    super::stop(provider);
}

void GenericBrightness::free()
{
	super::free();
}
