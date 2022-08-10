
import numpy as np
import pandas as pd
import pybloom_live as bloom
import time
from Bio import SeqIO as seq
import sys
import os
import time

def choose_genome_sparsity(genome_filesize, read_filesize, read_length, key_size, num_files, flex_factor=2, average_exon_size = 100):
    """
    function to choose genome indices and read indices to optimize for runtime
    genome and read filesizes only need to be in the same units

    read_length: length of RNAseq reads in fasta file
    key_size: int: size of partial keys to use
    num_files: number of RNAseq reads
    flex_factor: how many discrepancies per read we can accomodate
    average_exon_size: self explanatory

    returns: factor by which we must reduce number of keys in the genome
    """
    
    if key_size>=average_exon_size or key_size >= read_length:
        return("err: key size is too large")
    
    num = genome_filesize*(1-(key_size/average_exon_size))
    denom = (read_filesize/read_length)*flex_factor*num_files
    
    out = int((num/denom)**0.5)
    
    if out==0:
        return 1
    else:
        return out

def estimate_unique(key_size, bases,exons, sparsity):
    """
    size: total number of bases
    key_length: length of partial key
    exons: total number of exons in the genome
    sparsity_factor: 1/sparsity_factor=the proportion of bases we use as starting index when creating filter

    returns: estimate of how full a set filter is
    *****have dropped sparsity factor for now___need to think this thru******
    """
    
    samps = ((bases/exons)-key_size)*exons/sparsity
    expected_unique=(1-(1-1/samps)**(4**key_size))*samps
    return expected_unique

def generate_key_size(bases, exons, fpr=0.05):
    """
    picks the smallest key size that will still give us less than fpr% expected false positives in set filter
    bases: total number of bases in genome used to make filter
    exons: total number of exons in genome used to make filter
    fpr: max acceptable false positive rate
    sparsity_factor: 1/sparsity_factor=the proportion of bases we use as starting index when creating filter

    returns: minimum key size that meets fpr goals
    """

    #start with intial size
    key_size = 10
    

    #increase size until percentage full estimate falls below 1%
    while 1-(1-(estimate_unique(key_size, bases, exons,1)/4**key_size))**5 > fpr:
        
        key_size+=1

    return key_size

def estimate_bits_needed (key_size, ave_exon_size, num_exons, sparsity, fpr):
    """
    estmiate bits needed for bloom filter based on size of keys and how many of them we will have

    key_len: length of partial keys we are using
    ave_exon_size: average size of exon in GENOME...what we use to populate filter
    num_exons: number of exons in the genome
    sparsity: sparsity factor for choosing keys from genome exons

    returns: number of bits to set filter with
    """
    expected_unique=estimate_unique(key_size,ave_exon_size*num_exons, num_exons, sparsity)
    bits_needed = int((expected_unique * np.log(fpr)) / np.log(1 / pow(2, np.log(2))))+1
   
    return bits_needed
    

class filter_object():
    """
    object to house the filter type and its attributes
    genome: filepath to genome from which we will build filter
    """
    
    def __init__ (self, genome, key_size, sparsity, bloombits, set_filter = False, fpr=0.1):
        self.genome = genome
        self.key_size = key_size
        self.sparsity = sparsity
        self.set_filter = set_filter
        self.bloombits = bloombits
        self.fpr=fpr
    
    def generate_filter(self, outfile, verbose = False):
        """
        set_filter: implement filter using has set rather than array
        indices: generate the filter with partial keys beginning at these indices
        filter object: filter array to be populated with seen reads, can experient with augmenting existing filters later
        could have if/then stacked filter where we just keep grabbing larger partial keys (might be more efficient way
        to access reads and score them)

        """
        
        #check to see if there are indices passed to filter obj
        #assumes that all exons are the same size
        #if not, create range object that is the length of the reads in genome fasta file
        # if self.indices==False:
        #     with open(self.genome) as input:
        #         self.indices = range(len(list(seq.parse(input,"fasta").seq[0])))
           
        num_added=0  
        #use the set filter rather than array
        if self.set_filter:
            filter_ = set()

        #if using bloom    
        else:
            filter_ = bloom.BloomFilter(capacity = self.bloombits, error_rate = self.fpr)
            
        #open genome file and iterate over reads       
        with open(self.genome) as input:
            for record in seq.parse(input,"fasta"):

                if "NNN" in record.seq:
                    print("N found")

                else:
                    print("NO N")

                #initialize variable to tell us the max position we can start from
                max_index=len(record)-self.key_size-1

                #initialize helper var to iterate over key
                j=0

                while j<max_index:

                    #add key to filter and increment j 
                    filter_.add(record.seq[j:j+self.key_size])
                    num_added+=1
                    j+=self.sparsity
                    with open (outfile, "a") as out:

                        out.write(f"{record.seq[j:j+self.key_size]}\n")

                    out.close()


        print(f"{'*'*25}, {num_added},{'*'*25}")

        self.filter_ = filter_
        

    def fast_check(self, in_path, out_path, check_indices):
        """
        in_path: filepath to RNAseq input fasta file
        out_path_filepath to write new fasta file to
        check_indices: indices of the RNAseq file to check in filter
        key_size: num bases in partial key
        """  

        with open(out_path, "w") as kept_reads:
 
            #open RNAseq file and create iterator to pass over reads
            with open(in_path) as reads:
                for read in seq.parse(reads,"fastq"):
                    
                    #iterate over indices to check filter for
                    for check_index in check_indices:

                        #check to see if partial key in filter
                        if read.seq[check_index:check_index+self.key_size] in self.filter_:
                            
                            #add read to output fasta file and move on to the next read
                            seq.write(read, kept_reads, "fastq")
                            break

    def sample_indices (self, in_path, out_path):
        """
        sample possible check indices (of the minimum size we need) and determine set of indices with the lowest hit rate
        should be able to guarantee that all prpper reads align while minimizing overall hits
        """

        pass

        
def main(outfile, genome, reads_path, out_path, set_filter_,flex_factor, fpr=0.01, premade_filter = False):
    """
    main function defines the parameters of the filter, loads previously built filter or generates a new one given genome input reference
    inputs are filter object paramters plus option to point to premade filter in memory
    read_path_folder: folder to load all reads from 
    output_paths: list: same length as read paths, output file locations to write new fasta files to
    """
    timey=time.time()
    #instantiate list of read filenames
    reads_list = os.listdir(reads_path)

    #process set filter variable from system input
    if set_filter_ in {"True","true","T","t"}:
        set_filter = True
    else:
        set_filter=False

    #grab respective filesizes...assumes the first file size in reads is representative
    genome_filesize = float(os.path.getsize(genome))
    
    print(f"examined_genome_size {genome_filesize} in {time.time()-timey}")
    timey=time.time()
    
    read_filesize = 0
    num_files=0
    for file in os.listdir(reads_path):

        full_path = os.path.join(reads_path,file)
        print(full_path)
        read_filesize += float(os.path.getsize(full_path))

        num_files+=1

    print(f"gathered total read size {read_filesize} in {time.time()-timey}")
    timey=time.time()

    #decide on key size...will choose smallest with less than 1% false positives in set filter
    #first grab total bases
    bases=0
    exons=0
    with open(genome) as genome_:
        for exon in seq.parse(genome_,"fasta"):
            bases+=len(exon.seq)
            exons+=1
    ave_exon_size = int(bases/exons)

    print(f"read genome, {exons} exons of {ave_exon_size} average size")

    bases_read = 0
    exons_read=0
    
    first_1000_read_lens = np.zeros(1000)
    x=0
    with open(os.path.join(reads_path, os.listdir(reads_path)[0])) as read1:
        for exon in seq.parse(read1,"fastq"):
            if x<1000:
                first_1000_read_lens[x]=len(exon.seq)
                x+=1

                bases_read+=len(exon.seq)
                exons_read+=1
            
            #exit the loop once we have enough samples to esimate read len
            else:
                break
            

    print(f"estimated average genome and read exon len in {time.time()-timey}")
    timey=time.time()

    first_1000_read_lens.sort()
    print(f"5th and 25th percentile read lengths: {first_1000_read_lens[50]} {first_1000_read_lens[250]}")

    #determine average read length for first read file
    ave_read_length = int(bases_read/exons_read)

    print(f"average read length: {ave_read_length}")

    #generate_key_size
    key_size = generate_key_size(bases=bases, exons=exons, fpr=fpr)

    print(f"generated key size {key_size} in {time.time()-timey}")
    timey=time.time()
    
    #set flex factor to 2 for now
    print(f"flex factor: {flex_factor}")

    #decide on ideal mix or genome and transcriptome incdices
    sparsity = choose_genome_sparsity(genome_filesize, read_filesize, ave_read_length, key_size, num_files, flex_factor, ave_exon_size)

    print(f"chose sparsity: {sparsity} in {time.time()-timey}")
    timey=time.time()

    #determine number of bits for bloom filter
    bloom_bits = estimate_bits_needed(key_size = key_size, ave_exon_size=ave_exon_size, num_exons=exons, sparsity=sparsity, fpr=fpr)*5

    print(f"estimated needed filter size: {bloom_bits/1000000} mb")

    #create filter object
    filter_obj = filter_object(genome=genome, key_size=key_size, sparsity=sparsity ,set_filter = set_filter, bloombits=bloom_bits)

    #check for premade filter and generate new one if not present
    if premade_filter:
        filter_obj.filter_ = premade_filter

    else:
        filter_obj.generate_filter(outfile)

    print("generated filter ", time.time()-timey)
    timey=time.time()

    #generate indices to check...populate empty array with indices based on flex factor and filter sparsity
    check_indices = np.zeros(flex_factor*sparsity+sparsity).astype(int)


    change_ind=0
    check_ind=-1
    for i in range(flex_factor+1):
    
        if i>0:
            check_ind+=key_size-1
        for j in range(sparsity):
        
            check_ind+=1
            check_indices[change_ind] += check_ind
            change_ind+=1

    print(check_indices)
    #iterate over filepaths for RNAseq fastas and write the reads we want to keep to the corresponding output path
    for i in range(len(reads_list)):

        filter_obj.fast_check(os.path.join(reads_path, reads_list[i]), os.path.join(out_path,reads_list[i]), check_indices)

    print("checked reads ", time.time()-timey)
    timey=time.time()

if __name__ == "__main__":

    ###how to process optional params with sys.argv...tomorrow
    ##make sure we pick a sparsity that enables us to not have overlapping partial keys 
    main(sys.argv[1],sys.argv[2],sys.argv[3], sys.argv[4], sys.argv[5], int(sys.argv[6]), float(sys.argv[7]))
