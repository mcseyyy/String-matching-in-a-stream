
String Matching in a Stream
===========================
Description
---
This is a randomised algorithm for string matching in a stream. It uses sublinear space (O(log m)) in pattern size and O(log m) time per input character (where m is the pattern size; |P|=m).

The implementation is based on Breslauer and Galil's algorithm described in:
- ACM: http://dl.acm.org/citation.cfm?id=2635814
-	Google Scholar: https://scholar.google.com/citations?view_op=view_citation&citation_for_view=i6ZjofkAAAAJ:MXK_kJrjxJIC

Currently I am working on a (1+e) randomised solution for the k-mismatches problem based on: http://arxiv.org/abs/1508.00731.
The current algorithm is one of the building blocks for it.

Files
---
- Fingerprint.h: contains a class for implementing Rabin Karp fingerprints
- Misc.h: has some helper functions that might not be class specific
- Stream_Match.h: contains the actual implementation
- Source.cpp: has an example of how to use everything

How To use it
---
###Setup
1) Standard compilation with your favourite compiler (I used MSVC++).

2) Include "Stream_Match.h".

###In code
1) Create a Stream_Match_Params based on the pattern. It generates a prime number and preprocess some information.

2) Create you Stream_Match object by feeding it the pattern and the params object.

3) Feed one character/elemnt at a time from you stream.

The template types for all the classes are:
- type that you want to be used for storing fingerprints (I recommend unsigned long long)
- type of the elements in your stream (can be pretty much anything)


Notes
---
- The pattern is not preprocessed as a stream so you need to store it at the beginning for the preprocessing; you can delete it once the Stream_Match object was created.
- I only checked if it compiles in Visual Studio 2013 (but it should work with any c++ compiler).
- As I mentioned in the description, this is part of a larger project, thus no commit history. I might make that public once it's finished.
