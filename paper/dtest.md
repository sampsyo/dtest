bib: dtest
title: Dynamically Enforcing Statistical Quality Properties with Distribution Testing
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
Instead, quality for this application is an aggregate, statistical property: the application succeeds if it classifies images correctly with high probability in aggregate over many images.
This kind of property applies to the distribution of behaviors that the program exhibits over time rather to any individual execution.
We call this category of correctness property a *statistical quality property*.

Statistical quality properties are more difficult to enforce at run time than traditional, deterministic correctness properties.
A program can check a function's precondition, for example, before executing the function and raise an exception if it does not hold.
But when correctness depends on the distribution of many inputs, the right dynamic check is less obvious.
If the program's developer has tested it while assuming a certain distribution of inputs, how can they check whether the inputs it sees in deployment are similarly distributed?

Programmers need tools that can ensure that *in vivo* behavior matches *in vitro* measurement.
We propose a methodology and tool for enforcing statistical quality properties.
We identify a class of application properties that, rather than constraining the behavior on any individual execution, describe statistical criteria over the aggregate behavior across many executions.
Statistical quality properties can include many different aspects of program behavior; we discuss case studies involving performance, parallelism, approximation, and learning accuracy.

In this work, we propose that software should take advantage of
a body of theoretical work on *distribution testing* [TK].
Distribution tests use a small number of samples to make a statistical statement about the overall distribution of a data set.
They can efficiently decide whether the overall distribution of inputs in production match a distribution that was anticipated in testing.

We describe &sysname;, a tool that tests and enforces statistical quality properties.
The goal is to use distribution testing to draw conclusions about aggregate program behavior using only a few input examples.
&sysname; consists of two phases:
an off-line testing phase that measures execution quality for a set of possible input distributions;
and an on-line classification phase that uses distribution testing to predict the quality for the current input conditions.

In this paper, we describe the class of statistical properties that we can enforce with &sysname;'s approach.
We develop a case study that focuses on *locality-sensitive hashing* [TK], a domain where performance depends heavily on input distributions.
We show that &sysname; can automatically adapt an LSH algorithm's parameters to match a variety of real-world data sets.

# Statistical Quality Properties

This section describes the range of properties we target with this work.
We use example applications and properties from different categories to illustrate the breadth of statistical quality properties among disparate domains and different categories of program behavior.
The rest of the paper develops a generic methodology for enforcing all of these properties.

In general, a statistical quality property is a criterion that a software developer has in mind that applies to the *probability* of any program behavior.
If $x$ is a program execution and $p$ is a predicate that decides whether execution has a certain behavior, then a statistical quality property has the form $\text{Pr}\left[p(x)\right] \ge t$ where $t$ is a probability threshold.
The predicate $p$ characterizes something desirable about about the execution's resource usage, output, or side effects.

We enumerate a range of example applications and their associated statistical quality properties.

#### Performance: hashing.

Input distributions affect the performance of many systems.
For example, a hash table's balance depends on the hash function's ability to map the input distribution uniformly onto the table's buckets.
A given hash function can be a better match for some key distributions and a worse match for others.
Recent work has evaluated the suitability of hash functions against simple key distributions [@hashcompare].

To optimize for balance, a hash table should be able to dynamically decide which hash function to use based on the keys it sees at run time.
Distribution testing can help: the developer can test the hash table against a variety of possible input distributions and a collection of well-known hash functions in development, and then a distribution test can help choose the best mapping for the real data.

#### Parallelism: similarity search.

- Goal: show that similarity search configurations perform better/worse depending on testable properties of the input distribution (of binary vectors)

- Filter-then-verify pipelines seem amenable to dtesting, since we can test the filtering capabilities, and then the verification time/quality is proportional to the filtering successfulness.

- In the all-pairs Hamming similarity join problem space, many papers have used filtering techniques that have performance directly scaling with the data distribution "randomness".

- For example, if you do filtering based on chunks of the input bits,
then if the vectors are random looking in the chunk, it will filter well,
but if the chunks are highly modal (only a few values in that chunk) then it will not filter very much at all.

#### Accuracy: image filtering.

- Goal: show that approximate image filters have better/worse quality depending on the distribution of pixels or other high-level image features.

- Concretely, could look at a data set that is mostly faces, or mostly landscapes, or mostly abstract art, and these should be testable easily, and different filters should perform differently.

- We considering the opencv set of algorithms for image gradient / edge detection (?).  There were a handful of different approximations to some sort fo standard gradient algorithm.

#### Precision and recall: machine learning.

- Goal: show that for some machine learning algorithm/application, we can determine the best time to retrain the model, in response to changes in the input data.

- I am imagining an algorithm that takes a long time to train, and has basically the same predictive power, unless the training data change significantly.  And if the data change, then retraining will be necessary, although expensive.

# Related Work

- auto-tuning (PetaBricks [@petabricks]) and run-time algorithm selection (Nitro)
- input distributions matter
- distribution testing

# Enforcing Statistical Quality Properties with Distribution Testing

~ Figure { #fig-overview; caption: "Overview of the dtest system." }
![overview][]
~

[overview]: fig/overview.pdf { width: 3in; }

system overview

# Case Study

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

# Appendix: Using the &sysname; Tool { @h1:"A" }

A kind of "user manual" goes here.

[BIB]
