library("Rsubread")
library("lsa")

#need to take in working directory where filtered output stored
cwd(filtered_output_dir)

getwd()

typeof(featurecounts_unfiltered)
write.csv(featurecounts_unfiltered$counts, file="yool")

featurecounts_unfiltered <- featureCounts(files = "C:\\Users\\jonah\\School\\armin2\\ecoli\\outputs\\res_unfiltered.sam", annot.ext = "C:\\Users\\jonah\\School\\armin2\\ecoli\\annots\\annotations.gff", isGTFAnnotationFile=TRUE, GTF.attrType = 'ID' )
featurecounts_filtered <- featureCounts(files = "C:\\Users\\jonah\\School\\armin2\\ecoli\\outputs\\res_filtered.sam", annot.ext = "C:\\Users\\jonah\\School\\armin2\\ecoli\\annots\\annotations.gff", isGTFAnnotationFile=TRUE, GTF.attrType = 'ID' )
featurecounts_combined_filtered <- featureCounts(files = "C:\\Users\\jonah\\School\\armin2\\ecoli\\outputs\\res_combined_filtered.sam", annot.ext = "C:\\Users\\jonah\\School\\armin2\\ecoli\\annots\\annotations.gff", isGTFAnnotationFile=TRUE, GTF.attrType = 'ID' )
featurecounts_random_subset <- featureCounts(files = "C:\\Users\\jonah\\School\\armin2\\ecoli\\outputs\\res_random_subset.sam", annot.ext = "C:\\Users\\jonah\\School\\armin2\\ecoli\\annots\\annotations.gff", isGTFAnnotationFile=TRUE, GTF.attrType = 'ID' )
featurecounts_combined_unfiltered <- featureCounts(files = "C:\\Users\\jonah\\School\\armin2\\ecoli\\outputs\\res_combined.sam", annot.ext = "C:\\Users\\jonah\\School\\armin2\\ecoli\\annots\\annotations.gff", isGTFAnnotationFile=TRUE, GTF.attrType = 'ID' )

filtered <-as.vector(featurecounts_filtered$counts)
unfiltered <-as.vector(featurecounts_unfiltered$counts)
combined_filtered <-as.vector(featurecounts_combined_filtered$counts)
combined_unfiltered <- as.vector(featurecounts_combined_unfiltered$counts)
random_subset <- as.vector(featurecounts_random_subset$counts)



filtered_sim <- cosine(filtered,unfiltered)
combined_filtered_sim <- cosine(combined_filtered,filtered)
combined_unfiltered_sim <- cosine(filtered,combined_unfiltered)
random_subset_sim <- cosine(random_subset,filtered)

a <- paste('similarity of filtered sample: ',filtered_sim)
b <- paste('similarity of combined filtered sample: ', combined_filtered_sim)
c <- paste('siilarity of combined unfiltered sample: ', combined_unfiltered_sim)
d <- paste('siilarity of random subset of combined: ', random_subset_sim)

print(a)
print(b)
print(c)
print(d)


mouse_unfiltered <- featureCounts(files = "C:\\Users\\jonah\\School\\armin2\\Filtah\\Filtah\\mouse_unfiltered.sam", annot.inbuilt = "mm10")
mouse_filtered <- featureCounts(files = "C:\\Users\\jonah\\School\\armin2\\Filtah\\Filtah\\mouse_filtered.sam", annot.inbuilt = "mm10")


m_filt <-as.vector(mouse_filtered$counts)
m_unfilt <-as.vector(mouse_unfiltered$counts)

cosine(m_filt, m_unfilt)
