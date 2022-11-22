---
title: 'Gala: A Python package for galactic dynamics'
tags:
  - C++
  - Transcriptomics
  - Genomics
  - High Throughput Sequencing
  - Metagenomics
  - Metatranscriptomics
authors:
  - name: Jonah Poczobutt
    orcid: 0000-0000-0000-0000
    equal-contrib: true
    affiliation:
  - name: Armin Anderson
    equal-contrib: true # (This is how you can denote equal contributions between multiple authors)
    affiliation:

date: 21 November 2022


---

# Summary
sluiceBox is an RNAseq data prefilter created by Jonah Poczobutt (jonahpocz@gmail.com) and Armin Anderson designed to reduce alignment time while preserving relative expression data with high fidelity. It relies on a Bloom Filter implementation to filter out RNAseq reads whose k-mers of some user-specified length are not found in the Bloom Filter array generated from a reference genome (positive mode). Basic steps are outlined below:

Populate an array with hashed values of all k-mers from the reference genome.
Generate non-overlapping k-mers from RNAseq reads to check the filter for.
Write reads to output file based on acceptance criteria (positive mode - write reads that are found in the filter , negative mode-write reads that are not found in the filter)
sluiceBox accepts Fastq files as input for RNAseq data and fasta files as input for reference genome data. It can be compiled on Windows and OSX.

sluiceBox is named for the placer mining sluice box tool popular during the California gold rush. Sluice boxes were used to filter out gold from ore deposited into the box.


# Statement of need

The motivating concept behind sluiceBox is that it should be possible to get a representative subset of reads by a process that is much less computationally intensive than aligning all RNAseq reads to a reference genome. Of course checking RNAseq reads for an exact match in a reference genome will not prove fruitful. Even slight discrepancies (sequencing errors, splice gaps, snps, etc.) will lead to a failure of exact matching. Computing more robust similarity/distance scores for all points on the genome quickly increases computational needs to the point where we may as well just align the full RNAseq dataset. sluiceBox aims to strike a balance between these two extremes.

SluiceBox is easy to download and compile from github, presenting users with a low friction option for beginning RNA seq filtering. This ease of use is a key advantage of the software.

sluiceBox first breaks down a given reference genome into all k-mers of a given length (keySize parameter). Storing all these k-mers requires approximately O(nk) memory, where k is keySize. For large genomes, storing/accessing a hash set of this size could prove problematic for many machine specifications. sluiceBox thus makes use of a bloom filter data structure, in order to decrease memory needs. The bloom filter implementation also offers improved query speed over other hashed types.

sluiceBox begins with a large zero array. K-mer values are then passed through two hash functions in order to determine which bits in the array to set to 1 during filter population. During examination of RNAseq data, k-mers of the same length are processed through the same two hash functions and checked in the filter. If sluiceBox is being run in positive mode, only those reads whose k-mers show a 1 value at both hased positions in the filter will be retained. This implementation guarantees that if we check the filter with a k-mer that was identical to some k-mer used to populate the filter, that read will be retained. There is a potential for false positives in this case, but not false negatives. If sluiceBox is being run in negative mode, there is no such guarantee.

The flexFactor parameter determines the maximum number of non-overlapping k-mers are checked in the filter for each RNAseq read. K-mers are chosen to be non-overlapping in order to guarantee that a RNA-seq read with a given number of discrepancies from the reference genome will be retained. Discrepancies can include any discrepancy in the sequence itself or a splice gap. A flex factor of 2 guarantees that an RNA-seq read with 1 such discrepancy will be retained, for example, assuming that the read length is at least keySize*2. Set the flex factor such that key size * flex factor is longer than the longest read in the set to check all possible non-overlapping k-mers.

