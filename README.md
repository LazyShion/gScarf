## Scaling Fine-grained Modularity Clustering on Massive Graphs
The implementation of gScarf proposed in IJCAI'19.
If you use our software, **please cite the following paper**.

 Hiroaki Shiokawa, Toshiyuki Amagasa, Hiroyuki Kitagawa, 
**"Scaling Fine-grained Modularity Clustering for Massive Graphs,"** 
In _Proceedings of the 28th International Joint Conference on Artificial Intelligence (IJCAI2019)_, pp.4597-4604, Macao, China, August 2019. 

### LICENSE
This software is released under the MIT license. Please read `LICENSE.txt` for details.

### How to Use
#### Requirements
gScarf requires the following softwares.
* gcc Version 4.8.5 (or later)
* boost C++ Libraries Version 1.53.0 (or later)

We have confirmed that our software works on the following environments.
* CentOS 7.6
* MacOS 10.14.6

#### Build
1. Install *gcc* and *boost libraries* on your environment.
2. Rewrite `Makefile`. You should add your file path of "libboost_serialization.a" in the directory of the boost libraries to `Makefile` like as follows:
```Makefile
LIB= (add your path of "libboost_serialization.a" here)
```
3. Build the software.
```
make clean; make
```

#### Usage
###### Input file
Input file must be formatted as a list of edges included in a given graph; each line represents a pair of nodes that are connected via an edge. The nodes in the same line must be spanned by `TAB` or `space`.
``` sample_graph.txt

```
