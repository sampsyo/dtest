bib: dtest
title: Enforcing Statistical Quality Properties
sysname: `dtest`

[TITLE]

# Introduction

Tools for program correctness typically...

In this paper, we propose a methodology and tool for enforcing *statistical* quality properties...
We identify application properties that, rather than constraining the behavior on any individual execution, apply to the *aggregate* behavior across many executions...
Statistical quality properties can include many different aspects of program behavior; we discuss case studies involving performance, parallelism, approximation, and learning accuracy.

Our approach is to take advantage of a body of theoretical work on *distribution testing* [TK].
Distribution tests use a small number of samples to make a statistical guarantee about the overall distribution of a data set.

We describe &sysname;, a tool that tests and enforces statistical quality properties.

intro: statistical quality properties are useful and we want to test them

# Related Work

- auto-tuning (PetaBricks [@petabricks]) and run-time algorithm selection (Nitro)
- input distributions matter
- distribution testing

# System Overview

~ Figure { #fig-overview; caption: "Overview of the dtest system." }
![overview][]
~

[overview]: fig/overview.pdf { width: 3in; }

system overview

# Distribution Tests

# Applications

introduce the application; the quality metric; the distribution test (from the previous section) that we will use

## Performance: Hash Functions

- Main related work: Seven-dimensional hash comparison, where one dimension is distribution.  They look at ``Dense'' = $\{1,2,..,n\}$, and ``Sparse'' = random 64-bit ints, and ``Grid'' = eight ints, each in $\{0,1,2,..,14\}$.
[@hashcompare]

- Goal: reproduce one of their results. 

- They claim one result about Mult-Shift being more ``stable'' than Murmur as you change the input distribution (bottom of page 103).


## Parallelism: Similarity Search

- Goal: show that similarity search configurations perform better/worse depending on testable properties of the input distribution (of binary vectors)

- Filter-then-verify pipelines seem amenable to dtesting, since we can test the filtering capabilities, and then the verification time/quality is proportional to the filtering successfulness.

- In the all-pairs Hamming similarity join problem space, many papers have used filtering techniques that have performance directly scaling with the data distribution ``randomness''.

- For example, if you do filtering based on chunks of the input bits, 
then if the vectors are random looking in the chunk, it will filter well, 
but if the chunks are highly modal (only a few values in that chunk) then it will not filter very much at all.

## Approximation: Image Filters

- Goal: show that approximate image filters have better/worse quality depending on the distribution of pixels or other high-level image features.

- Concretely, could look at a data set that is mostly faces, or mostly landscapes, or mostly abstract art, and these should be testable easily, and different filters should perform differently.

- We considering the opencv set of algorithms for image gradient / edge detection (?).  There were a handful of different approximations to some sort fo standard gradient algorithm.

## Accuracy: Machine Learning

- Goal: show that for some machine learning algorithm/application, we can determine the best time to retrain the model, in response to changes in the input data.

- I am imagining an algorithm that takes a long time to train, and has basically the same predictive power, unless the training data change significantly.  And if the data change, then retraining will be necessary, although expensive.

# Experience

## Implementation

implementation / usability / programmability case study

- how the user applies the system to their application
- how the tool works at run time
    - what it looks like (a) when it succeeds, and (b) when it fails
- why the property you want is actually enforced

# Results

dtest-ified systems outperform the best single configuration

# Conclusion

[BIB]
