##### Table of Contents  
[Description](#Description)  
[Instructions](#Instructions)  
[Test](#Test)  

 .
<a name="Description"/>
## Description

sluiceBox is an RNAseq data prefilter created by Jonah Poczobutt and Armin Anderson designed to reduce alignment time while preserving relative expression data with high fidelity. It relies on a [Bloom Filter]( https://en.wikipedia.org/wiki/Bloom_filter) implementation to filter out RNAseq reads whose k-mers of some user-specified length are not found in the Bloom Filter array generated from a reference genome (positive mode). Basic steps are outlined below:

1) Populate an array with hashed values of all k-mers from the reference genome.
2) Generate non-overlapping k-mers from RNAseq reads to check the filter for. 
3) Write reads to output file based on acceptance criteria (positive mode - write reads that are found in the filter , negative mode-write reads that are not found in the filter)

sluiceBox accepts Fastq files as input for RNAseq data and fasta files as input for reference genome data.

sluiceBox is named for the placer mining sluice box tool popular during the California gold rush. [Sluice boxes](https://en.wikipedia.org/wiki/Placer_mining#Sluice_box)  were used to filter out gold from ore deposited into the box.

The motivating concept behind Filtah is that it should be possible to get a representative subset of reads by a process that is much less computationally intensive than aligning all RNAseq reads to a reference genome. Of course checking RNAseq reads for an exact match in a reference genome will not prove fruitful. Even slight discrepancies (sequencing errors, isoforms, snps, etc.) will lead to a failure of exact matching. Computing more robust similarity/distance scores for all points on the genome quickly increases computational needs to the point where we may as well just align the full RNAseq dataset. Filtah aims to strike a balance between these two extremes.

Filtah first breaks down a given reference genome into all k-mers of a given length (keySize parameter). Storing all these k-mers requires approximately O(nk) memory, where k is keySize. For large genomes, storing/accessing a hash set of this size could prove problematic for many machine specifications. Filtah thus makes use of a bloom filter data structure, in order to decrease memory needs. The bloom filter implementation also offers improved query speed over other hashed types. 

Filtah begins with a large zero array. K-mer values are then passed through two hash functions in order to determine which bits in the array to set to 1 during filter population. During examination of RNAseq data, k-mers of the same length are processed through the same two hash functions and checked in the filter. If Filtah is being run in positive mode, only those reads whose k-mers show a 1 value at both hased positions in the filter will be retained. This implementation guarantees that if we check the filter with a k-mer that was identical to some k-mer used to populate the filter, that read will be retained. There is a potential for false positives in this case, but not false negatives. If filtah is being run in negative mode, there is no such guarantee.

The flexFactor parameter determines the maximum number of non-overlapping k-mers are checked in the filter for each RNAseq read. K-mers are chosen to be non-overlapping in order to guarantee that a RNA-seq read with a given number of discrepancies from the reference genome will be retained. Discrepancies can include any discrepancy in the sequence itself or a splice gap. A flex factor of 2 guarantees that an RNA-seq read with 1 such discrepancy will be retained, for example, assuming that the read length is at least keySize*2. Set the flex factor such that key size * flex factor is longer than the longest read in the set to check all possible non-overlapping k-mers.

.
<a name="Instructions"/>
## Instructions

In order to display help, run filtah.exe --help. The following message will show in the terminal, explaining how to pass different parameters

Arguments must be passed in sequentially. The order for space separated command line arguments is as follows
1) mode - "p" for positive "n" for negative
   Positive mode retains any read that has at least one k-mer which is found in the filter. Negative mode retains reads that have no k-mers found in the    filter, given the flex factor specified. Note that negative mode does not guarantee that all reads that do not have a k-mer that appears in the          reference genome will be retained. This is because of the possibility of false positives in negative mode.
2) keySize - int
   The size (in bp) of k-mers used to populate the filter from the reference genome and the size of non-overlapping k-mers grabbed from the RNAseq file    to be checked in the filter. Longer key size reduces the chance of a false positive from a spurious match, but reduces sensititity. A reasonable        default size is 18.
3) flexFactor - int
   The maximum number of non-overlapping k-mers to check in the filter for a given read
4) genomeFile - str 
   Filepath to the reference genome (fasta format)
5) RNAseqFile - str or strings
   Filepath to RNAseq reads file (fastq format)
   You may pass amultiple RNASeq files. All will be processed using the same filter.
   
Filtah will notify through command line output once the filter has been created, and again for every RNAseq file chunk that is processed and written to the output file.
.
<a name="Test"/>
## Test

Prerequisite software installations for running this test are R with [Rsubread](https://rdocumentation.org/packages/Rsubread/versions/1.22.2) and [LSA](https://rdocumentation.org/packages/lsa/versions/0.73.3) packages installed, as well as mapping software [bowtie2](https://anaconda.org/bioconda/bowtie2).

All test data to run this test can be found in the test_data directory

First, we will run filtah on two test datasets. The first is an NCBI downloaded ecoli RNAseq file (ecoli_sample.fastq). The second (sample_combined.fastq) is a combination of ecoli_sample.fastq and additional RNAseq data from another organism (fruit fly) that has been appended. Fruit fly RNAseq reads outnumber ecoli reads about 5:1 in this file.

CD into the Filtah directory and run the following command to process both of these datasets at once:

![Alt text](/images/run_filter.JPG?raw=true)

This will result in the creation of two new fastq files (ecoli_sample_filtered.fastq and sample_combined_filtered.fastq)

Next, CD into test_data and create a bowtie2 index from the ecoli genome with the commands shown below:

mkdir index

![Alt text](/images/create_index.JPG?raw=true)

Then run bowtie2 on all 4 of these datasets one by one in order to obtain alignment.bam files. Remember to use include the -t parameter to see how long it takes to align each file. Filtered files should run in substantially less time. In order for the similarity Rscript to run without edits, you must keep the same names that these files already have for bowtie output (i.e. ecoli_sample_filtered.fastq -> ecoli_sample_filtered.bam) as shown below. Note the significant decrease in alignment time from both filtered files.

![Alt text](/images/run_bowtie.JPG?raw=true)

![Alt text](/images/bowtie_2.JPG?raw=true)

![Alt text](/images/bowtie_3.JPG?raw=true)

Run bowtie2 on random_subset.fastq. random_subset.fastq is a random sample of reads from sample_combined.fastq of a similar length to the result we obtain in sample_combined_filtered.fastq. The file sizes differ, but you can check that the line numbers are very similar.

You should now have 5 .bam files in this same directory. One for each of the .fastq files that were either initially supplied or created using Filtah

![Alt text](/images/bowtie_4.JPG?raw=true)

Next, CD into test_data and run summarize_similarity.r using the rscript command. Remember you must have rsubread and lsa packages installed. This script will generate a count of exon expressions levels using Rsubreads featureCounts functionality, and return [cosine similarity](https://en.wikipedia.org/wiki/Cosine_similarity) scores for selected expression levels from the fastq files present in this directory. Each score reflects the similarity of some result to the expression levels of ecoli_sample.fastq, which is unfiltered. Cosine similarity is bounded between 1 and -1, with 1 being identical and -1 being opposite.

![Alt text](/images/similarity.JPG?raw=true)

You should see the following results:

![Alt text](/images/results.JPG?raw=true)

This result demonstrates that relative expression levels from filtered outputs are highly similar to expression levels in the original, unfiltered data. Additionally, the similarity of filtered results to the original data is much higher than a random sample from the combined dataset of a similar size. This indicates that in the combined setting, filtering provides a significant speed benefit while maintaining much higher similarity than we would see from a random sample of the combined data. This demonstrates the potential usefulness of Filtah in a metagenomics context, for example. 






