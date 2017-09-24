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

### Phase 1 ✓

 - [x] Adaptative Radix Tree
   - [x] GwRadixTree
 - [x] Dictionaries
   - [x] GwDictionary (stub)
 - [x] Word segmentation
   - [x] GwSegmenter
   - [x] GwSegmenterFallback
   - [x] GwSegmenterPtBr
 - [x] Refcounted strings
   - [x] GwString
 - [x] Sentence manipulation
   - [x] GwStringEditor

### Phase 2

 - [ ] Language abstractions
   - [ ] GwGrammarClass
   - [ ] GwWord
 - [ ] Dictionaries
   - [ ] Word lookup

### Phase 3

 - [ ] Grammar abstraction
 - [ ] Grammar decomposition
 - [ ] Language abstractions
   - [ ] GwMark
   - [ ] GwParagraph
   - [ ] GwSentence
   - [ ] GwWord
 - [ ] Dictionaries
   - [ ] Word lookup
