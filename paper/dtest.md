bib: dtest
title: Dynamically Enforcing Statistical Quality Properties
sysname: `dtest`

[TITLE]

# Introduction

Most tools for program correctness enforce properties that must hold on every program execution.
For example, a unit test might check that a sort algorithm produces correctly sorted output;
a run-time assertion could check that a file handle is open before writing to it;
or a performance evaluation could check that a task always completes in less than a second.

Increasingly, however, applications have quality properties that are not as easy to quantify deterministically.
For example, consider an application that uses machine learning to classify images.
Intuitively, the system designer wants the algorithm to produce correct classifications, but it is impossible to require that it be correct *every time*.
Instead, quality for this application is an aggregate, statistical property: the application succeeds if it classifies images *with high probability* in aggregate over many images.

~TODO
Something about choosing variants/parameters dynamically.
~

In this paper, we propose a methodology and tool for enforcing statistical quality properties.
We identify a class of application properties that, rather than constraining the behavior on any individual execution, describe statistical criteria over the aggregate behavior across many executions.
Statistical quality properties can include many different aspects of program behavior; we discuss case studies involving performance, parallelism, approximation, and learning accuracy.

Our approach is to take advantage of a body of theoretical work on *distribution testing* [TK].
Distribution tests use a small number of samples to make a statistical guarantee about the overall distribution of a data set.

We describe &sysname;, a tool that tests and enforces statistical quality properties.
The goal is to use distribution testing to draw conclusions about aggregate program behavior using only a few input examples.
&sysname; consists of two phases:
an off-line testing phase that measures execution quality for a set of possible input distributions;
and an on-line classification phase that uses distribution testing to predict the quality for the current input conditions.

We examine statistical quality properties through four case studies from four different domains:

* Hash functions...
* Parallel similarity search...
* Approximate image filtering...
* Accuracy of machine learning models...

In each case, the application's statistical quality property cannot be enforced using a traditional, deterministic correctness tool.
We show that &syname; can enforce the property by making dynamic decisions based on changing inputs.
In each case, the &sysname;-augmented version of the program outperforms the best non-adaptive configuration by...

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

- Ideal outcome:

## Parallelism: Similarity Search

- Goal: show that similarity search configurations perform better/worse depending on testable properties of the input distribution (of binary vectors)

- Filter-then-verify pipelines seem amenable to dtesting, since we can test the filtering capabilities, and then the verification time/quality is proportional to the filtering successfulness.

- In the all-pairs Hamming similarity join problem space, many papers have used filtering techniques that have performance directly scaling with the data distribution ``randomness''.

- For example, if you do filtering based on chunks of the input bits,
then if the vectors are random looking in the chunk, it will filter well,
but if the chunks are highly modal (only a few values in that chunk) then it will not filter very much at all.

- Ideal outcome:

## Approximation: Image Filters

- Goal: show that approximate image filters have better/worse quality depending on the distribution of pixels or other high-level image features.

- Concretely, could look at a data set that is mostly faces, or mostly landscapes, or mostly abstract art, and these should be testable easily, and different filters should perform differently.

- We considering the opencv set of algorithms for image gradient / edge detection (?).  There were a handful of different approximations to some sort fo standard gradient algorithm.

## Accuracy: Machine Learning

- Goal: show that for some machine learning algorithm/application, we can determine the best time to retrain the model, in response to changes in the input data.

- I am imagining an algorithm that takes a long time to train, and has basically the same predictive power, unless the training data change significantly.  And if the data change, then retraining will be necessary, although expensive.

- Ideal outcome:

# Experience

## Implementation

implementation / usability / programmability case study

- how the user applies the system to their application
- how the tool works at run time
    - what it looks like (a) when it succeeds, and (b) when it fails
- why the property you want is actually enforced

- Ideal outcome:

# Results

- System-centric outcome
    - Example of using the system, end-to-end, sucessfully, in one of the application domains
    - Explanation about why our workflow eliminates unnecessary configuration time
    - Interpretation of the statistical guarantees, as specified to this domain
    - We trump the auto-tuning feature based approach because...
    - Some timing and stats for the pre-processing / run time / post-processing executions 

- Ideal application-centric outcomes:
    - Punchline: dtest-ified systems outperform the best single configuration
    - Similarity Search:
        - Input Distributions: Random, Clustered, Grid
        - Possible Algorithms: Chunk splitting, Random filter/hash, LSH, ...
        - Possible Schemes: parallelization ideas ...
        - System: Pick the algorithm+parallelization depending on the input distribution
        - Outcome: We have overall less communication and faster computation than a single config
    - Hashing:
        - Input Distributions: Random, Small Range, Grid, PiecewiseConstant
        - Possible Algorithms: List of 23+ Hash functions
        - Possible Schemes: See 7D Hash paper (linear, quadractic, robin hood, cuckoo)
        - System: Pick the algorithm+structure depending on the input distribution
        - Outcome: We have overall less collisions/time than a single config
    - Image Filter:
        - Input Distributions: Faces, Landscapes, Crowds, Animals, Scientific Pics, Drawings
        - Possible Algorithms:
        - Possible Schemes:
        - System: Pick the algorithm+structure depending on the input distribution
        - Outcome:
    - Accuracy Application:
        - Input Distributions:
        - Possible Algorithms:
        - Possible Schemes:
        - System: Pick the algorithm+structure depending on the input distribution
        - Outcome:
# Conclusion

[BIB]
