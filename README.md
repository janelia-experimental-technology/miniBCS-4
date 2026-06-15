# miniBSC-4
 Configurable I/O PCB for Teensy 4.1 and Max11300
 
 https://doi.org/10.25378/janelia.29669162
 
 
 System Overview

The miniBCS is a compact, general-purpose Behavioral Control System (BCS). It provides a powerful platform to control real-time experiments. It is highly adaptable in terms of types of outputs and connectors. It is also designed to be easy to use, self-documenting, inexpensive, and easy to customize.

Hardware Development

The miniBCS is designed around the Teensy processor system. Currently, the Teensy 3.5 processor board is being used. This board utilizes a 32-bit ARM processor with 120Mhz operation, 512K Flash, and 192K RAM. The Arduino programming environment makes writing and downloading code easy. A MAX11300 integrated circuit provides 12-bit bipolar ADC/DAC and other configurable IO not available on the Teensy. The various input/outputs (IOs) are brought out to various connector options, including DB9, BNC, latch connectors, and RJ9. Connectors can face vertically or horizontally. A waveform generator is also built in. builders can add a small monochrome graphic LCD and three pushbuttons to provide a simple user interface. The system is designed so that only the required features and connectors need to be added to the base board, minimizing cost and assembly time. The enclosure can be made from laser-cut material, making it easy to configure the unit, house the electronics, make attachment points, and engrave connector labels.

Capabilities (some are mutually exclusive):


    Up to three RS232 serial ports
    Host USB serial port (also used for programming)
    CAN bus
    Ethernet
    Up to 10 12-bit DACs with +/-5V, 0-10V, or -10 to 10V ranges (MAX11300)
    Up to 10 12-bit ADCs with 0-2.5, +/-5V, 0-10V, or -10 to 0V ranges (MAX11300)
    Up to 10 IOs with programmable thresholds or output ranges (MAX11300)
    8 isolated solenoid drivers with 11-35 Volt power supply range
    Frequency Generator with a range of 1Hz to 10MHz, sine, triangle, square wave
    2 temperature sensors (MAX11300)
    SPI/I2C Expansion Bus
    Up to 8 IOs that connect directly to the processor that can be programmed for:
        Interrupts
        0-3.3V DAC
        0-3.3V ADC
        3.3V digital output
        5V tolerant digital input
        PWM or servo drive
        Various timer/counter needs
