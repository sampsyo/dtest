bib: dtest

# Introduction

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

## Parallelism: Similarity Search

load balancing binary vector distance comparison (parallelism)

## Approximation: Image Filters

image filter (approximation)

## Accuracy: Machine Learning

learning (accuracy)

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
