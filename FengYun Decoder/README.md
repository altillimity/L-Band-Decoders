# FengYun Decoder

Small progran that can take synced symbols from a QPSK demodulator and do the Viterbi / Diff decoding, deframing and RS. Output is synced, derandomized and corrected CADUs.

Synced symbols can be obtained from the demodulator flowchart in the Flowcharts directory.

Usage : `./FengYun symbols.bin outputframes.bin`