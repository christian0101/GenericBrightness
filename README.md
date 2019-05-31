# GenericBrightness

## General information

### Goal

On some laptops, the screen brightness is not controlled by Intel registers. In that case, if your OEM ACPI methods are correct (this is rare but it does happen), you can use GenericBrightness. This is a simple kext that periodicaly checks if the brightness has changed and if so, uses the ACPI methods to enact that change.

To use it, you need a PNLF patch (to enable brightness handling in macOS) as well as a patch for you screen to be hooked to GenericBrightness. This is explained below.

### Author: [ivik](https://www.insanelymac.com/forum/profile/73374-ivik/)

The original kext was retrieved from https://github.com/chris1111/GenericBrightness/ and https://github.com/christophcharles/GenericBrightness/ but seems to have an older history, https://www.insanelymac.com/forum/topic/236835-updated-2012-genericbrightnesskext/.

### Configuration

The full explanation is still available at: https://www.insanelymac.com/forum/topic/236835-updated-2012-genericbrightnesskext/

Here is a quick reminder though:
- First, you need to add a PNLF device to your DSDT. This is quite straightfoward and can be done in the following way:

```
Device (PNLF)
{
    Name (_HID, EisaId ("APP0002"))
    Name (_CID, "backlight")
    Name (_UID, 0x0A)
    Name (_STA, 0x0B)
}
```

- Then, you need to mark your screen. Find you GPU node. It is usually something like ```_SB.PCI0.GFX0``` or if you renamed it to ```IGPU _SB.PCI0.IGPU```. Under this node, you will find something like ```LCD```, ```CRT``` or ```TV```. In my case, it was for instance ```_SB.PCI0.IGPU.LCD0```. The device must have the following methods :
    - ```_BCM```
    - ```_BCQ```
    - ```_BCL```
    If you have found it, at the line: ```Name (_HID, EisaId ("LCD1234"))```. This leads to something like:  

```
Device (LCD)
{
    Name (_ADR, 0x0400)
    Name (_HID, EisaId ("LCD1234")) //this must be added
    Method (_DCS, 0, NotSerialized)
    {
    }
    Method (_DGS, 0, NotSerialized)
    {
    }
    Method (_BCL, 0, NotSerialized)
    {
    }
    Method (_BCM, 1, NotSerialized)
    {
    }
    Method (_BQC, 0, NotSerialized)
    {
    }
}
```
