# libgwords - text processing library

Libgwords is a GObject-based library that allows application developers to
manipulate natural language texts easily. It is designed to supply consumers
with text iteration and manipulation data structures and algorithms, such as
dictionaries, word and sentence segmenters, sentence manipulators, grammar
trees, word conjugation and more.

## Goals

Libgwords aims to provide the following funtionality:

 - [x] Efficient associative array (ART)
 - [ ] Language abstractions
 - [ ] Word and sentence segmentation
 - [ ] Sentence manipulation
 - [ ] Grammar decomposition
 - [ ] Dictionaries
 - [ ] Operation pipelines
 - [ ] Root extractor
 - [ ] Word conjugation
 - [ ] String iteration
 - [ ] Synonims and antonyms

## Roadmap

This is a tentative roadmap of how to achieve the goals of libgwords. Since it's
an auxiliary library that I'm building as part of (but not limited to) my masters
program, this can change during the development.

### Phase 1

 - [x] Adaptative Radix Tree
   - [x] GwRadixTree
 - [ ] Dictionaries
   - [ ] GwDictionary
 - [ ] Word segmentation
   - [ ] GwWord
   - [ ] GwWordSegmenter
   - [ ] GwWordSegmenterFallback
 - [ ] String iteration
   - [ ] GwString
   - [ ] GwStringIterator
 - [ ] Sentence manipulation
   - [ ] GwSentence
   - [ ] GwSentenceModifier

### Phase 2

 - [ ] Grammar decomposition
 - [ ] Root extractor
 - [ ] Noun conjugation
   - [ ] GwNoun
