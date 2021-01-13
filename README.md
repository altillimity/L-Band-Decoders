# L-Band Satellite Decoders

A few small projects aimed at decoding various L-Band satellites

**Note for users updating from older releases : The new MetOp and FengYun decoders now include a deframer. If using MetFy3x, derandomization and Reed-Solomon should be disabled or the file will fail to process. The CADU-Synchroderand is no longer necessary before using the other tools in this repo.**

### Dependencies

Some projects will require (or / and) :
- [libccsds](https://github.com/altillimity/libccsds)
- [libcorrect](https://github.com/quiet/libcorrect)
- [libfec](https://github.com/quiet/libfec)
- [libpng](https://github.com/glennrp/libpng) + [zlib](https://github.com/madler/zlib)

The flowcharts require GNU Radio 3.8 or above.

### Standalone Demodulators

All flowcharts contained in this repository can be substitued with those standalone demodulators. It will be easier and faster to use for most users https://github.com/altillimity/Standalone-Demodulators.

# FengYun-3 A/B/C

**Supported downlink :** AHRPT, 1704.5Mhz (3C 1701.4Mhz)    
**Modulation :** QPSK  
**Symbolrate :** 2.8Mbps (3C 2.6MSPS)  
**Recording bandwidth :** >= 3MSPS, 6MSPS or so preferred 

**Decoding :**
- Record a baseband 
- Demodulate with the FY Demodulator flowchart
- Process the soft symbols with FengYun Decoder (use 3b mode for 3c, seems like the coding was changed)   
- Run the resulting CADU file through the FengYun VIRR Decoder / You can also use MetFy3x instead  

# MetOp A/B/C

**Supported downlink :** AHRPT, 1701.3Mhz  
**Modulation :** QPSK  
**Symbolrate :** 2.33Mbps  
**Recording bandwidth :** >= 3MSPS, 6MSPS or so preferred 

**Decoding :**
- Record a baseband 
- Demodulate with the MetOp Demodulator flowchart
- Process the soft symbols with MetOp Decoder   
- Run the resulting CADU file through the MetOp AVHRR Decoder / You can also use MetFy3x instead  
- Run the resulting CADU file through the MetOp AMSU Decoder to get AMSU A1 and A2 data  
- Run the resulting CADU file through the MetOp MHS Decoder to get MHS data  
- Run the resulting CADU file through the MetOp MHS Decoder to get HIRS data - The HIRS instrument is only functional onboard MetOp-A  
- Run the resulting CADU file through the MetOp IASI Decoder to get IASI data