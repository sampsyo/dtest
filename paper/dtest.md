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
We develop a case study that focuses on nearest-neighbor searching.  In particular, consider a variety of *locality-sensitive hashing* [TK] algorithms.  We observe that performance depends heavily on the combination of the algorithm parameters and the input distribution. 
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

The execution time of a parallel algorithms depends on the balance of the work load among the processing elements.
For the algorithmic task of finding all pairs of similar vectors (i.e. performing a similarity join), we observe that many published algorithms have a high sensitivity to the distribution of input data.
For example, most similarity join algorithms utilize a filter-then-verify pipeline, which parallelizes the workload using a set of filters over the input data.
For any fixed filter, a worst-case input distribution will lead to a very imbalanced workload. For example, prefix filters, which partition the data based on the first few vector coordinates, perform much better for random data than for data with highly modal prefixes.

Ideal outcome: Using &sysname; we demonstrate that dynamic filter selection, based on the distribution of the input data, may improve performance dramatically.

#### Accuracy: image filtering.

Approximations are commonplace in many applications.
Especially in domains that deal with sensory data like images and audio, faster but less accurate algorithms can be essential.
Many approximations are more accurate on some inputs and less accurate on others; it is sufficient for them to be accurate on *most* of the executions.

Many common image filters have widely used approximations.
For example, to compute the gradient magnitude at every point in an image for edge detection, imaging pipelines typically use a simple convolution that estimates the gradient locally.
Several such gradient approximations have been proposed: Sobel, Scharr, Prewitt, Roberts cross, and others [@costella].
Some filters are more expensive than others, and some are more accurate than others.
The complexity of the image can dictate which filter offers the best accuracy--performance trade-off.

With distribution testing, the programmer can test each possible filter using different distribution of image pixel windows.
At run time, the imaging pipeline can automatically detect the right filter for each image.

#### Precision and recall: machine learning.

Machine-learning models depend on the comprehensiveness of their training.
In a public service, they can benefit from periodic re-training.
For example, a web service might classify objects in photographs, and the kinds of photos that users upload might change over time.
Such a service needs to know when users' inputs are changing substantially with respect to earlier behavior.
The overall goal is to classify images correctly with a high probability.

Distribution testing can help answer this question.
The system can evaluate the model's precision and recall on a sample of user data, and as new data comes in, automatically decide whether the distribution has shifted from the training set.
When it has, the service can pay the cost to retrain the model to adapt to changing demands.

# Related Work

- auto-tuning (PetaBricks [@petabricks]) and run-time algorithm selection (Nitro)
- input distributions matter
- distribution testing

# Enforcing Statistical Quality Properties with Distribution Testing

~ Figure { #fig-overview; caption: "Overview of the &sysname; system." }
![overview][]
~

[overview]: fig/overview.pdf { width: 4in; }

This section describes &sysname;, our prototype system for enforcing statistical quality properties with distribution testing.
We design &sysname; as an analog to traditional testing tools, which interface with the system under test (SUT) via a *test harness*.
The programmer specifies the target correctness property for the application, and the harness lets &sysname; measure it in execution of the SUT.

The overall goal in &sysname; is to automatically choose the right application parameters according to the distribution of inputs it receives.
The design uses an off-line phase and an on-line phase.
In the off-line phase, we use a collection of candidate input distributions to measure the program, and we choose the best program configuration for each distribution.
In the on-line phase, we use distribution testing to detect which candidate distribution is closest to the current inputs.
Then, &sysname; configures the program according to the best settings found in the off-line phase for that distribution.
Figure [#fig-overview] summarizes the complete process.

#### Off-line phase.

To use &sysname;, the developer two sets of information: candidate input distributions and program configurations.
The input distributions represent possible deployment scenarios for the application; the configurations represent different choices that the program can make.
The programmer also provides a way to measure the *quality score* of a given execution of the program to capture the statistical quality property they want to enforce.
The off-line phase in &sysname; navigates the cross product of the distributions and the configurations: it runs the program with each pairing and records the corresponding quality score.
To communicate with the on-line phase, &sysname; produces a mapping that matches each candidate distribution with the best corresponding program configuration---the one with the highest quality score.

#### On-line phase.

In deployment, &sysname; uses distribution testing.
It collects a sample of the dynamic invocations of the program and uses them to infer the current input distribution.
Specifically, each candidate input distribution comes with a distribution test, and &sysname; compares the on-line sample to each candidate using the corresponding test.
It determines the candidate distribution that matches the input sample most closely.
Using the mapping from the off-line phase, &sysname; produces the best configuration for that inferred distribution.


# Case Study

implementation / usability / programmability case study

- how the user applies the system to their application
- how the tool works at run time
    - what it looks like (a) when it succeeds, and (b) when it fails
- why the property you want is actually enforced

- Ideal outcome:

# Nearest Neighbor Search using Locality Sensitive Hashing


## FALCONN LSH Parameter Selection

We overview the parameter selection algorithm in [@falconn].  First, they ``set the algorithm parameters so that the empirical probability of successfully finding the exact nearest neighbor is at least 0.9.''
Moreover, we set ``the number of LSH tables L so that the amount of additional memory occupied by the LSH data structure is comparable to the amount of memory necessary for storing the data set.''
They ``perform a grid search over the remaining parameter space and report the best combination of parameters.''

## Parallel LSH Parameter Selection
We overview the parameter selection algorithm in [@plsh].
They enumerate $k = 1, 2, . . . k_{max}$, and for each $k$ select the smallest value of $m$ satisfying Equation 7.3 (probability of correctness greater than a threshold).
The values of $E[\#unique]$ and $E[\#collisions]$ have empirical estimators (through sampling, using a random set of 1000 queries and 1000 data points).
``For large amounts of data, $k_{max}$ is determined by the amount of RAM in the system. The storage required for the hash tables increases with $L$, which in turn increases super-linearly with $k$. They also suggest that they want to store about 1000 hash tables, which provides a range of ideal values for $k$ and $m$.

For their evaluation, "The optimal LSH parameters were selected using our performance model. We use the following parameters: k = 16, m = 40, L = 780, D = 500, 000, R = 0.9, δ = 0.1."

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
