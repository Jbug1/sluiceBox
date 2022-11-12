##### Table of Contents  
[Description](#Description)  
[Instructions](#Instructions)  
[Test](#Test)  

.
<a name="Description"/>
## Description

Filtah is an RNAseq data prefilter designed to reduce alignment time while preserving relative expression data with high fidelity. It relies on a basic 
[Bloom Filter]( https://en.wikipedia.org/wiki/Bloom_filter) implementation to filter out RNAseq reads whose k-mers of some user-specified length are not found in the Bloom Filter array generated from a reference genome (positive mode). Basic steps are outlined below:

1) Populate an array with hashed values of all k-mers from the reference genome.
2) Generate non-overlapping k-mers from RNAseq reads to check the filter for. 
3) Write reads to output file based on acceptance criteria (positive mode - write reads that are found in the filter , negative mode-write reads that are not found in the filter)

Filtah accepts Fastq files as input for RNAseq data and fasta files as input for reference genome data.

.
<a name="Instructions"/>
## Instructions

In order to display help, run filtah.exe --help. The following message will show in the terminal, explaining how to pass different parameters

Arguments must be passed in sequentially. The order for space separated command line arguments is as follows
.
<a name="Test"/>
## Test



