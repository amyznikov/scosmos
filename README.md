scosmos
=======

Utilites in apps/ directory:

  ssa-source-dump

    Dump SuperCOSMOS binary 'source' file (http://www-wfau.roe.ac.uk/www-data/ssa/source/)
    to stdout as ASCII text.

    Example:
      $ ssa-source-dump -h ssaSource000ra030.bin


  ssa-detection-dump

    Dump original SuperCOSMOS binary file (http://www-wfau.roe.ac.uk/www-data/ssa-detection/)
    to stdout as ASCII text.

    Example:
      $ ssa-detection-dump ssadetection000ra030.bin


  ssa-detection-dump-plateids

    Scan SuperCOSMOS binary file and print distinct plateid's present in this file.

    Example:
      $ ssa-detection-dump-plateids ssadetection000ra030.bin


  ssa-detection-plate-extract

    Extract specified single plate data from SuperCOSMOS binary file and write
    it into separate output file in binary format. If no plateid is specified
    then all plates of given survey will extracted into separate output files.

    Example:
      $ ssa-detection-plate-extract survey=1 ssadetection000ra030.bin


  ssa-plate-dump

    Dump SuperCOSMOS binary single-plate file as ASCII text or binary.
    The binary output is suitable to prepare 'clean' plates with junk
    detections removed. The single-plate binary file is extracted from original
    SuperCOSMOS binary file using 'ssa-detection-plate-extract'.

    Examples:
      $ ssa-plate-dump -h 1-65537.dat
      $ ssa-plate-dump -fcb 1-65537.dat -o 1-65537.clean.dat


  ssa-plate-stats

    Gather some SuperCOSMOS single-plate file statistics and print to stdout.
    The single-plate binary file is extracted from original SuperCOSMOS binary
    file using 'ssa-detection-plate-extract'.

    Example:
      $ ssa-plate-stats 1-65537.dat.bz2

