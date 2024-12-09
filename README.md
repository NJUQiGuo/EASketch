# EASketch
EASketch is an elastic and adaptive sketch tailored for temporal frequency queries, providing both efficient storage and high accuracy over extended time periods. EASketch dynamically adapts to changing data patterns and optimizing memory utilization.  This repostory contains all the codes related to the manuscript submitted to IEEE ICDE.
# Files 
cmsketch - CM sketch  
cusketch - CU sketch  
countsk - Count sketch  
hpwin_sketch: Virtual base class 
pbf: Multi-layer Structure
* multi_layer1 - multi-layer structure for PCM, PCM-H
* multi_layer2 - multi-layer structure for PPCM, EASketch
hopping: Structure of each layer in Multi-layer
* pcm1_base - PCM  
* hopping_cm - PCM-H
* pyramid_pcm - PPCM
* easketch - EASketch
* easketch_with_compress - EASketch with the time-based compression
PCMSketch: Atomic sketch of PPCM
DynamicSketch: Atomic sketch of EASketch
DynamicSketchWithCompress: Atomic sketch of EASketch with the time-based compression

