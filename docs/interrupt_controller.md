
# Interrupt Controller

A basic interrupt controller is embedded in the mainboard CPLD. Given the limited resources of the CPLD the interrupt controller is designed to be a no frills device. To support a fully fledged OS, such as ELKS, with only minimal changes to the code, the interrupt controller mimics some of the features of an Intel 8259A:

 - Four positive edge triggered hardware interrupts (IRQ0 -> IRQ3)
 - Each IRQ can be enabled/disabled individually
 - 8259A fully nested mode
 - IRQ priority (IRQ0 highest -> IRQ3 lowest)
 - Configurable vector base address
 - Generates vectors BASE + 0x00 -> 0x03 (IRQ0 -> IRQ3) where BASE defaults to 0x20


## Power-on State

At power up all four IRQ's are disabled.
The BASE vector is set to 0x20

## IO Ports

There are three I/O ports for communicating with the interrupt controller:

 - 0x18 Ctrl
 - 0x1A Data (EOI; end of interrupt)
 - 0x1C Base (Set base vector)

The IRQ's can be controlled by writing to the ctrl port (1 == enable, 0 == disable):

 - Bit 7-4 reserved
 - Bit 3 IRQ3
 - Bit 2 IRQ2
 - Bit 1 IRQ1
 - Bit 0 IRQ0

Reading from the ctrl port returns the current status of the IRQ's (enabled/disabled).

When an IRQ is disabled the hardware interrupt line is ignored and any previously unacknowledged interrupts are lost. When an IRQ is enabled it must see a positive edge on the hardware line before it will raise an interrupt with the CPU. Any further positive edges are ignored until the interrupt has been acknowledged.

The base address can be set by writing to the base port.

 - Bit 7-3 new base address
 - Bit 2-0 ignored

When the CPU request the interrupt vector it's constructed as follows:

 - Bit 7-3 base address
 - Bit 2 = 0
 - Bit 1-0 = IRQ number (e.g. 0, 1, 2, or 3)

## Nested Mode

From the Intersil 82C59A data sheet describing fully nested mode (with tweaks):

*The interrupt requests are ordered in priority from 0 through 3 (0 highest). When an interrupt is acknowledged the highest priority request is determined and its vector placed on the bus. Additionally, a bit of the Interrupt Service register (IS0 - 3) is set. This bit remains set until the microprocessor issues an End of Interrupt (EOI) command immediately before returning from the service routine. While the IS bit is set, all further interrupts of the same or lower priority are inhibited, while higher levels will generate an interrupt (which will be acknowledged only if the microprocessor internal interrupt enable flip-flop has been re-enabled through software).*

An EOI can be sent by writing any value to the EOI port.


## Future changes

The following are possible enhancements to the interrupt controller that haven't been explored:

 - Additional IRQ lines, possibly using the USER1 -> USER4 connections on the bus.
 - Level triggered interrupts

