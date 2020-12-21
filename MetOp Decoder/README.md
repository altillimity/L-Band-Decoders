# MetOp Decoder

Small progran that can take synced symbols from a QPSK demodulator and do the Viterbi decoding, deframing and RS correction. Output is synced, derandomized and corrected CADUs.

Synced symbols can be obtained from the demodulator flowchart in the Flowcharts directory.

Usage : `./MetOp symbols.bin outputframes.bin`